#include <jni.h>
#include <string>
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"

using namespace cv;

extern "C" JNIEXPORT jstring JNICALL
Java_com_example_camera_1test_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_camera_1test_MainActivity_grayscale(JNIEnv *env, jobject /* this */, jlong addrImg, jlong addrOut) {
    Mat& out=*(Mat*)addrOut;
    Mat& img=*(Mat*)addrImg;
    cvtColor(img, out, COLOR_RGBA2GRAY);
}