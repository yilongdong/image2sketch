#pragma once

#include <string>
#include <cinttypes>
#include <opencv2/opencv.hpp>
#include "config.h"

namespace imageSDK {
    struct ConvertProfile {
        bool show_debug_picture{false}; // 展示中间结果
        bool is_colorful{false};
        bool use_sketch_texture{true}; // 是否使用素描纹理
        bool with_shadow{true}; // 是否结合明暗灰度
        std::string sketch_texture_path{ RESOURCE + "/素描纹理.png" };
        double bilateral_sigma_color{100};
        double bilateral_sigma_space{15};
        double canny_sigma{0.4}; // 0 ~ 1.0
        double canny_factor{20};
        uint32_t edge_dilate_size{3};  // 轮廓膨胀大小
        uint32_t edge_dilate_times{5}; // 轮廓膨胀次数
    };

    struct Error {
        bool is_success;
        std::string error_msg;
    };

    // TODO: 暂不支持彩色铅笔画
    // TODO: 支持比纹理更小的图片
    // TODO: 不支持单通道灰度或者四通道图
    // TODO: 素描纹理合成算法的实现
    // TODO: 线框模拟铅笔笔迹算法的实现
    // TODO: 错误处理，参数检查
    // TODO: Debug模式
    void toSketch(cv::Mat const &image, cv::Mat& sketch, ConvertProfile const &profile, Error &error) ;
}