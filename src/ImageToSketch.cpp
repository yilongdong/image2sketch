#include "ImageToSketch.h"
#include "config.h"

namespace imageSDK {

    std::optional<cv::Mat> toSketch(cv::Mat const& image, ConvertProfile const& profile) noexcept {
        Error error;
        cv::Mat sketch = toSketch(image, profile, error);
        if (!error.is_success) return std::nullopt;
        return sketch;
    }

    void kmeansGray(cv::Mat const& src, cv::Mat& dst, int numCluster);
    cv::Mat toSketch(cv::Mat const& image, ConvertProfile const& profile, Error& error) noexcept {
        // cv::bilateralFilter也可以
        cv::Mat blurMat;
        cv::bilateralFilter(image, blurMat, -1, 100, 15);
//        cv::blur(blurMat, blurMat, cv::Size(3, 3));
//        cv::edgePreservingFilter(blurMat, blurMat, cv::RECURS_FILTER, 60, 0.44);
//        cv::detailEnhance(blurMat, blurMat);
        cv::Mat edgeMat, grayMat, thresholdMat;
//        cv::Canny(blurMat, edgeMat, 8, 13);
        cv::cvtColor(blurMat, grayMat, cv::COLOR_BGR2GRAY);
        cv::adaptiveThreshold(grayMat, thresholdMat, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY, 11, 0);
        cv::Mat blurThresholdMat;
//        cv::blur(thresholdMat, thresholdMat, cv::Size(3, 3));

//        thresholdMat = 255 - thresholdMat;
//        edgeMat = 255 - edgeMat;
        cv::equalizeHist(thresholdMat, thresholdMat);

        cv::imshow("gray", grayMat);


        grayMat = grayMat;
        cv::Mat kmeansGrayMat;
        kmeansGray(grayMat, kmeansGrayMat, 15);

        cv::Mat multiLevelGrayMat;
        kmeansGrayMat = kmeansGrayMat * 255;
        kmeansGrayMat.convertTo(multiLevelGrayMat, thresholdMat.type());
//        cv::blur(multiLevelGrayMat, multiLevelGrayMat, cv::Size(7,7));
        cv::imshow("multiLevelGrayMat", multiLevelGrayMat);
//        cv::blur(thresholdMat, thresholdMat, cv::Size(3, 3));
        cv::normalize(thresholdMat, thresholdMat, 0, 255, cv::NORM_MINMAX);
        cv::imshow("threshold", thresholdMat);

        cv::Mat sketch, colorfulSketch;
        sketch = multiLevelGrayMat - thresholdMat;
        cv::normalize(sketch, sketch, 0, 230, cv::NORM_MINMAX);

        // grayMat颜色聚类

        colorfulSketch = sketch;
        error.is_success = true;
        return profile.is_colorful ? colorfulSketch : sketch;
    }
    void AdaptiveCanny() {

    }

    void kmeansGray(cv::Mat const& src, cv::Mat& dst, int numCluster) {
        cv::Mat source;
        cv::Mat edgeMat, dst8u;
//        dst.convertTo(dst8u, CV_8U);
        cv::Canny(src, edgeMat, 10, 20);
        cv::equalizeHist(edgeMat, edgeMat);
        cv::imshow("edge", edgeMat);
        src.convertTo(source, CV_32F);
        int width = source.cols, height = source.rows;
        cv::Mat points(width * height, 1, CV_32F, cv::Scalar(10));
        for (int row = 0; row < height; row++) {
            for (int col = 0; col < width; col++) {
                int index = row * width + col;
                points.at<float>(index, 0) = source.at<float>(row, col);
            }
        }
        //运行kmeans
        cv::Mat labels;
        cv::Mat centers;
        cv::TermCriteria criteria = cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::MAX_ITER, 10, 0.1);
        kmeans(points, numCluster, labels, criteria, 3, cv::KMEANS_PP_CENTERS, centers);

        // 显示图像分割后的结果，一维转多维
        cv::Mat texture = cv::imread(RESOURCE + "/素描纹理.png");
        cv::detailEnhance(texture, texture);
        cv::Mat grayTexture;
        cv::cvtColor(texture, grayTexture, cv::COLOR_BGR2GRAY);
        cv::Mat textureMap, tmpTexture, tmpSubTexture;
        grayTexture.convertTo(textureMap, CV_32F);
        // 假设纹理图片比输入图片小
        cv::copyMakeBorder(textureMap, textureMap, 0, std::max(source.rows - textureMap.rows, 0), 0, std::max(source.cols - textureMap.cols, 0), cv::BORDER_WRAP);
        tmpSubTexture = textureMap.clone();
        cv::flip(tmpSubTexture, tmpTexture, 1);
        textureMap += tmpTexture;
        cv::flip(tmpSubTexture, tmpTexture, 0);
        textureMap += tmpTexture;
        cv::flip(tmpSubTexture, tmpTexture, -1);
        textureMap += tmpTexture;


        cv::imwrite("new texture.png", textureMap);
        dst = cv::Mat::zeros(source.size(), source.type());
        for (int row = 0; row < height; row++) {
            for (int col = 0; col < width; col++) {
                int index = row * width + col;
                int label = labels.at<int>(index, 0);
                dst.at<float>(row, col) = centers.at<float>(label);
            }
        }
        cv::GaussianBlur(dst, dst, cv::Size(3, 3), 3,3);

//        cv::blur(dst, dst, cv::Size(3, 3));
        for (int row = 0; row < height; row++) {
            for (int col = 0; col < width; col++) {
                int index = row * width + col;
                int label = labels.at<int>(index, 0);
                dst.at<float>(row, col) = (dst.at<float>(row, col) / 255) * textureMap.at<float>(row, col);
            }
        }

        cv::normalize(dst, dst, 0, 1, cv::NORM_MINMAX);
    }
}

