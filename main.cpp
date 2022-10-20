//
// Created by vivi on 20.10.22.
//

#include <opencv2/opencv.hpp>
#include <iostream>
#include <cmath>

using namespace cv;
using namespace std;

int main() {
    Mat image;

    VideoCapture cap(2);

    if (!cap.isOpened()) {
        std::cout << "cannot open camera";
    }

    if (!cap.read(image)) {
        std::cout << "cannot read from camera";
        return 0;
    }

    Mat image_hsv;
    cvtColor(image, image_hsv, COLOR_BGR2HSV);

    std::vector<cv::Mat> planes(3);
    Mat &val_mat = planes[2];

    cv::split(image_hsv, planes);
    double min, max;
    cv::minMaxLoc(val_mat, &min, &max);

    double dv_thr = max * 0.95;

    Mat val_thr;

    threshold(val_mat, val_thr, dv_thr, 255, THRESH_BINARY);
    int greater_thr = countNonZero(val_thr);

    std::cout << greater_thr << '\n';

//    10 < num < 36

    // find moments of the image
    Moments m = moments(val_thr,true);
    Point centr(m.m10 / m.m00, m.m01 / m.m00);

// coordinates of centroid
    cout << Mat(centr) << endl;

// show the image with a point mark at the centroid
    circle(image, centr, 30, Scalar(128, 0, 0), 1);


    int blob_area = greater_thr;
    int diam = ceil(pow(blob_area, 0.5)) * 10;
    cout << diam;

    diam = 100;
    // didn't work. Don't know why
//    Mat crop_mat = val_mat(Range(centr.x-diam,centr.x+diam),Range(centr.y-diam,centr.y+diam));
    Mat crop_mat(image, Rect(Point(centr.x-diam, centr.y+diam), Point(centr.x+diam,centr.y-diam)));

//    Range r1(centr.x-diam,centr.x+diam);

    // TODO:
    //  1. save an image with laser)

//    imshow("Image with center", image);
    imshow("Cropped laser spot", crop_mat);

//    imshow("Threshold", val_thr);
//    imshow("Value", val_mat);

    waitKey();

    return 0;
}