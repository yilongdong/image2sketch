#include <iostream>
#include <gflags/gflags.h>
#include "config.h"
#include "ImageToSketch.h"

DEFINE_string(image_path, "", "图片路径");
DEFINE_bool(color, false, "是否着色");
DEFINE_string(output, "./demo.png", "输出的素描图路径");

int main(int argc, char *argv[]) {
//    FLAGS_image_path = RESOURCE + "/dyl.jpeg";
//    FLAGS_image_path = RESOURCE + "/背景图片.jpeg";
//    FLAGS_image_path = RESOURCE + "/夜景.png";
    FLAGS_image_path = RESOURCE + "/电车.webp";
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    std::cout << "from " << FLAGS_image_path << " to " << FLAGS_output << std::endl;
    cv::Mat image = cv::imread(FLAGS_image_path, cv::IMREAD_COLOR);
    imageSDK::ConvertProfile profile{
        .is_colorful = FLAGS_color
    };
    imageSDK::Error error;
    auto sketchImage = imageSDK::toSketch(image, profile, error);
//    imageSDK::pencilSketch(image);
    cv::imshow("sketch", sketchImage);
    cv::imwrite(FLAGS_output, sketchImage);
    cv::waitKey(0);

    return 0;
}