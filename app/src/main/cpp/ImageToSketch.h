#pragma once

#include <string>
#include <opencv2/opencv.hpp>
#include <android/log.h>

#define LOG_TAG "show"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)

namespace imageSDK {
    struct ConvertProfile {
        bool show_debug_picture{false}; // 展示中间结果
        bool is_colorful{false};
        bool use_sketch_texture{true}; // 是否使用素描纹理
        std::string sketch_texture_path{ "texture.png" };
        double bilateral_sigma_color{100};
        double bilateral_sigma_space{15};
        double canny_sigma{0.4}; // 0 ~ 1.0
        double canny_factor{20};
        int edge_dilate_size{3};  // 轮廓膨胀大小
        int edge_dilate_times{5}; // 轮廓膨胀次数
    };

    struct Error {
        bool is_success;
        std::string error_msg;
    };

    void toSketch(cv::Mat const &_image, cv::Mat const &pattern, cv::Mat &sketch,
                  ConvertProfile const &profile, Error &error);
}