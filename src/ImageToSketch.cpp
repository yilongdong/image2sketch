#include "ImageToSketch.h"
#include "config.h"

namespace imageSDK {
    cv::Mat toSketch(cv::Mat const& image, ConvertProfile const& profile, Error& error) {
        // 1. 双边滤波
        cv::Mat blur;
        cv::bilateralFilter(image, blur, -1, 100, 15);

        // 2. 转灰度图
        cv::Mat gray;
        cv::cvtColor(blur, gray, cv::COLOR_BGR2GRAY);

        // 3. 自适应二值化
        cv::Mat threshold;
        cv::adaptiveThreshold(gray, threshold, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY, 11, 0);
        cv::equalizeHist(threshold, threshold);
        cv::imshow("threshold", threshold);

        // 4. 边缘检测
        cv::Mat edge;
        cv::Canny(gray, edge, 5, 10);
//        cv::equalizeHist(edge, edge);


        // 5. 纹理生成 (假设纹理比图片小)
        cv::Mat texture = cv::imread(RESOURCE + "/素描纹理.png");
        cv::detailEnhance(texture, texture);
        cv::cvtColor(texture, texture, cv::COLOR_BGR2GRAY);
        texture.convertTo(texture, CV_32F);
        cv::Mat textureMap, tmpTexture, tmpSubTexture;
        cv::copyMakeBorder(texture, texture, 0, std::max(gray.rows - texture.rows, 0), 0, std::max(gray.cols - texture.cols, 0), cv::BORDER_WRAP);
        tmpTexture = texture.clone();
        cv::flip(tmpTexture, textureMap, 1);
        texture += textureMap;
        cv::flip(tmpTexture, textureMap, 0);
        texture += textureMap;
        cv::flip(tmpTexture, textureMap, -1);
        texture += textureMap;

        // 6. 合成灰度图与纹理，产生光影图
        cv::Mat shadow;
        gray.convertTo(shadow, CV_32F);
        cv::GaussianBlur(shadow, shadow, cv::Size(3, 3), 3,3);
        shadow = shadow.mul(texture);
        cv::normalize(shadow, shadow, 0, 1, cv::NORM_MINMAX);
        cv::imshow("shandow", shadow);

        // 7. 利用边缘信息对二值图进行降噪,生成线稿图
        cv::Mat kernel = cv::Mat::ones(cv::Size(3, 3), CV_8U);
        cv::dilate(edge, edge, kernel, cv::Point(-1,-1), 4);
        cv::imshow("edge", edge);
        cv::threshold(edge, edge, 1, 1, cv::THRESH_BINARY);
        cv::imshow("before filter", threshold);
        threshold = threshold.mul(edge);

        cv::imshow("after filter", threshold);

        // 8. 合成光影图与线稿图，产生素描图
        shadow = shadow * 255;
        threshold.convertTo(threshold, CV_32F);
        cv::Mat sketch;
        sketch = shadow - threshold;
        cv::normalize(sketch, sketch, 0, 255, cv::NORM_MINMAX);
        sketch.convertTo(sketch, CV_8U);
        cv::equalizeHist(sketch, sketch);

        error.is_success = true;
        return sketch;
    }
}

