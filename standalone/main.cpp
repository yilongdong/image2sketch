#include <iostream>
#include <gflags/gflags.h>
#include "config.h"
#include "ImageToSketch.h"

DEFINE_string(image_path, "", "图片路径");
DEFINE_bool(color, false, "是否着色");
DEFINE_string(output, "demo.png", "输出的素描图路径");

int main(int argc, char *argv[]) {
//    FLAGS_image_path = RESOURCE + "/dyl.jpeg";
//    FLAGS_image_path = RESOURCE + "/背景图片.jpeg";
    FLAGS_image_path = RESOURCE + "/夜景.png";
//    FLAGS_image_path = RESOURCE + "/电车.webp";
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    std::cout << "from " << FLAGS_image_path << " to " << FLAGS_output << std::endl;
    cv::Mat image = cv::imread(FLAGS_image_path, -1);
    imageSDK::ConvertProfile profile;
    profile.show_debug_picture = true;
    profile.use_sketch_texture = false;
    profile.edge_dilate_times = 1;
    profile.with_shadow = false;
    imageSDK::Error error;
    cv::Mat sketch;
    imageSDK::toSketch(image, sketch, profile, error);
    if (error.is_success) {
        cv::imshow("sketch", sketch);
        cv::imwrite(FLAGS_output, sketch);
        cv::waitKey(0);
    }
//    imageSDK::pencilSketch(image);


    return 0;
}