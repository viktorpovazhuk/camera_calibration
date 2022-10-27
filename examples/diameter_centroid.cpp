//
// Created by vivi on 20.10.22.
//

#include <opencv2/opencv.hpp>
#include <iostream>
#include <cmath>
#include <string>

using namespace cv;
using namespace std;

int main() {
    Mat image;

//    VideoCapture cap(0);
//
//    if (!cap.isOpened()) {
//        std::cout << "cannot open camera";
//    }
//
//    if (!cap.read(image)) {
//        std::cout << "cannot read from camera";
//        return 0;
//    }
//
//    imshow("Image", image);
//
//    imwrite("../data/test_aim.jpg", image);

    image = imread("../data/test1.jpg", 1);
//
//    if (!image.data)
//    {
//        printf("No image data \n");
//        return -1;
//    }

    Mat image_hsv;
    cvtColor(image, image_hsv, COLOR_BGR2HSV);

    std::vector<cv::Mat> planes(3);
    cv::split(image_hsv, planes);

    Mat val_mat = planes[2];

    double min, max;
    cv::minMaxLoc(val_mat, &min, &max);

    double dv_thr = max * 0.95;

    Mat val_thr;

    threshold(val_mat, val_thr, dv_thr, 255, THRESH_BINARY);
    int greater_thr = countNonZero(val_thr);

    std::cout << "num of greater than 0.95 thr: " <<  greater_thr << '\n';

//    10 < num < 36

    Moments m = moments(val_thr,true);
    Point centr(m.m10 / m.m00, m.m01 / m.m00);

    cout << "coordinates of centroid: " << Mat(centr) << endl;

    circle(image, centr, 30, Scalar(128, 0, 0), 1);


    int blob_area = greater_thr;
    int diam = ceil(pow(blob_area, 0.5));
    cout << "+- diameter of spot: " << diam << endl;

    // didn't work. Don't know why
//    Mat crop_mat = val_mat(Range(centr.x-diam,centr.x+diam),Range(centr.y-diam,centr.y+diam));
    Mat crop_mat(image, Rect(Point(centr.x-diam, centr.y+diam), Point(centr.x+diam,centr.y-diam)));

    cv::split(crop_mat, planes);

    val_mat = planes[2];

    double crop_max_val;
    cv::minMaxLoc(val_mat, nullptr, &crop_max_val);

    double crop_thr = crop_max_val * 0.9;

    Mat thr_mat;
    threshold(val_mat, thr_mat, crop_thr, 255, THRESH_BINARY);
    greater_thr = countNonZero(thr_mat);

    std::cout << "number of pixels greater than 0.9 thr in cropped area, total number: " <<  greater_thr << ", " << crop_mat.total() << '\n';
    std::cout << "proportion of greater than 0.9 thr in cropped area: " <<  greater_thr / (float)crop_mat.total() << '\n';



    imshow("Image with center", image);
    imshow("Cropped laser spot", crop_mat);

    waitKey();

    return 0;
}