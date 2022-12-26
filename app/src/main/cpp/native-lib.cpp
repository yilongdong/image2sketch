#include <jni.h>
#include <string>
#include "ImageToSketch.h"


extern "C" JNIEXPORT void JNICALL
Java_org_opencv_android_Utils_nBitmapToMat2(JNIEnv *env, jclass clazz, jobject bitmap, jlong m_addr,
                                            jboolean un_premultiply_alpha);

extern "C" JNIEXPORT void JNICALL
Java_org_opencv_android_Utils_nMatToBitmap2(JNIEnv *env, jclass clazz, jlong m_addr, jobject bitmap,
                                            jboolean premultiply_alpha);

//extern "C" JNIEXPORT jstring JNICALL
//Java_com_scut_example_MainActivity_stringFromJNI(
//        JNIEnv* env,
//        jobject /* this */) {
//    std::string hello = "Hello from C++";
//    return env->NewStringUTF(hello.c_str());
//}

//extern "C"
//JNIEXPORT jstring JNICALL
//Java_com_scut_example_MainActivity_processString(
//        JNIEnv *env, jobject thiz, jstring str) {
//    auto tempStr = std::string (env->GetStringUTFChars(str, nullptr));
//    auto result = tempStr + " fuck";
//    return env->NewStringUTF(result.c_str());
//}
extern "C"
JNIEXPORT jboolean JNICALL
Java_com_scut_example_ImageProcessHelper_imageProcess(JNIEnv *env, jobject thiz,
                                                      jobject image, jobject pattern, jobject result) {
    cv::Mat src, texture, dst;
    Java_org_opencv_android_Utils_nBitmapToMat2(env, nullptr, image, (jlong) &src, 0);
    Java_org_opencv_android_Utils_nBitmapToMat2(env, nullptr, pattern, (jlong) &texture, 0);

    imageSDK::Error error;
    imageSDK::ConvertProfile convertProfile;
    convertProfile.show_debug_picture = false;
//    int srcType = src.type();
//    int cv_8u_type = CV_8UC3;
//    char output[1024];
//    sprintf(output, "=====%d  %d", srcType, cv_8u_type);
//
//    LOGI(output);

    imageSDK::toSketch(src, texture, dst, convertProfile, error);
    if (error.is_success) {
        Java_org_opencv_android_Utils_nMatToBitmap2(env, nullptr, (jlong) &dst, result, 0);
    } else {
        LOGI(error.error_msg.c_str());
    }
    return error.is_success;
}