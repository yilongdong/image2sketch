#include "ImageToSketch.h"
#include "config.h"

namespace imageSDK {
    void imshowIF(bool condition, std::string const& name, cv::Mat const& image) {
        if (condition) {
            cv::imshow(name, image);
        }
    }
    // TODO: 支持彩色素描画
    void toSketch(cv::Mat const &image, cv::Mat& sketch, ConvertProfile const &profile, Error &error) {
        // 0. 检查参数
        if (profile.is_colorful) {
            error.is_success = false;
            error.error_msg = "暂不支持彩色素描画转换";
            return;
        }
        if (profile.use_sketch_texture && profile.sketch_texture_path.empty()) {
            error.is_success = false;
            error.error_msg = "未设置素描纹理图片的路径";
            return;
        }

        // 1. 双边滤波
        cv::Mat blur;
        cv::bilateralFilter(image, blur, -1,
                            profile.bilateral_sigma_color,
                            profile.bilateral_sigma_space);

        // 2. 转灰度图
        cv::Mat gray;
        cv::cvtColor(blur, gray, cv::COLOR_BGR2GRAY);

        // 3. 自适应二值化
        cv::Mat threshold;
        cv::adaptiveThreshold(gray, threshold, 255,
                              cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY, 11, 0);
        cv::equalizeHist(threshold, threshold);

        // 4. 边缘检测
        cv::Mat edge;
        std::vector<uint32_t> grayLevelCount(256, 0);
        for (int i = 0; i < gray.rows; ++i) {
            for (int j = 0; j < gray.cols; ++j) {
                int grayLevel = gray.at<uint8_t>(i, j);
                ++grayLevelCount[grayLevel];
            }
        }
        uint32_t totalPixelsCount = gray.rows * gray.cols;
        uint32_t curPixelsCount = 0;
        uint32_t medianGrayLevel = grayLevelCount.back();
        for (int i = 0; i < grayLevelCount.size(); ++i) {
            curPixelsCount += grayLevelCount[i];
            if (curPixelsCount > totalPixelsCount / 2) {
                medianGrayLevel = i;
                break;
            }
        }
        double CannySigma = profile.canny_sigma;
        double CannyLT = (1.0 - CannySigma) * (medianGrayLevel / 255.0) * profile.canny_factor;
        double CannyHT = (1.0 + CannySigma) * (medianGrayLevel / 255.0) * profile.canny_factor;
        cv::Canny(gray, edge, CannyLT, CannyHT);

        // 5. 纹理生成 (假设纹理比图片小)
        // TODO: 支持比纹理更小的图片
        cv::Mat texture;
        if (profile.use_sketch_texture) {
            texture = cv::imread(profile.sketch_texture_path);
            cv::detailEnhance(texture, texture);
            cv::cvtColor(texture, texture, cv::COLOR_BGR2GRAY);
            texture.convertTo(texture, CV_32F);
            if (gray.rows < texture.rows || gray.cols < texture.cols) {
                error.is_success = false;
                error.error_msg = "暂不支持比纹理更小的图片";
                return;
            }
            cv::Mat textureMap, tmpTexture, tmpSubTexture;
            cv::copyMakeBorder(texture, texture, 0, std::max(gray.rows - texture.rows, 0), 0,
                               std::max(gray.cols - texture.cols, 0), cv::BORDER_WRAP);
            tmpTexture = texture.clone();
            cv::flip(tmpTexture, textureMap, 1);
            texture += textureMap;
            cv::flip(tmpTexture, textureMap, 0);
            texture += textureMap;
            cv::flip(tmpTexture, textureMap, -1);
            texture += textureMap;
        }


        // 6. 合成灰度图与纹理，产生光影图
        cv::Mat shadow;
        gray.convertTo(shadow, CV_32F);
        cv::GaussianBlur(shadow, shadow, cv::Size(3, 3), 3, 3);
        if (profile.use_sketch_texture) {
            shadow = shadow.mul(texture);
        }
        cv::normalize(shadow, shadow, 0, 1, cv::NORM_MINMAX);
        imshowIF(profile.show_debug_picture, "shandow", shadow);

        // 7. 利用边缘信息对二值图进行降噪,生成线稿图
        cv::Mat kernel = cv::Mat::ones(cv::Size(profile.edge_dilate_size, profile.edge_dilate_size), CV_8U);
        cv::dilate(edge, edge, kernel, cv::Point(-1, -1), profile.edge_dilate_times);
        imshowIF(profile.show_debug_picture, "edge", edge);
        cv::threshold(edge, edge, 1, 1, cv::THRESH_BINARY);
        imshowIF(profile.show_debug_picture, "threshold", threshold);
        threshold = threshold.mul(edge);
        imshowIF(profile.show_debug_picture, "threshold mul edge", threshold);

        // 8. 合成光影图与线稿图，产生素描图
        shadow = shadow * 255;
        sketch.convertTo(sketch, CV_32F);
        threshold.convertTo(threshold, CV_32F);
        sketch = shadow - threshold;
        cv::normalize(sketch, sketch, 0, 255, cv::NORM_MINMAX);
        sketch.convertTo(sketch, CV_8U);
        cv::equalizeHist(sketch, sketch);

        error.is_success = true;
        error.error_msg = "success";
    }
}

