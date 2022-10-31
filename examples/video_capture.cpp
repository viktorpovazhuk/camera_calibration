//
// Created by vivi on 31.10.22.
//

#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main() {
    for (int i = 0; i < 3; i++) {
        cin.get();
        // 1 video capture = 1 frame ? for some reasons, yes :)
        VideoCapture video_cap{2};
        Mat im;
        video_cap >> im;
//        imshow("im", im);
        string fmt = "../data/my_img%d.jpg";
        size_t size_s = snprintf(nullptr, 0, fmt.c_str(), i) + 1;
        char buf[size_s];
        snprintf(buf, size_s, fmt.c_str(), i);
        string fn{buf};
        imwrite(fn, im);
//        waitKey();
//        destroyAllWindows();
    }
}