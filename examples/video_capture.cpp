//
// Created by vivi on 31.10.22.
//

#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main() {
    VideoCapture video_cap{0};
    Mat im;
    bool success = video_cap.read(im);
    if (success) {
        string fn = "D:\\opencv_imgs\\img.jpg";
        cv::imwrite(fn, im);
    }
    else {
        std::cout << "Error" << '\n'; 
    }
}