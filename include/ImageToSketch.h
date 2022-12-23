#pragma once

#include <string>
#include <opencv2/opencv.hpp>
#include <optional>

namespace imageSDK {
    struct ConvertProfile {
        bool is_colorful;
    };

    struct Error {
        bool is_success;
        std::string error_msg;
    };

    cv::Mat toSketch(cv::Mat const& image, ConvertProfile const& profile, Error& error);
}