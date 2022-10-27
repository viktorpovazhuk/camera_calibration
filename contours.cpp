//
// Created by vivi on 27.10.22.
//

#include <opencv2/opencv.hpp>
#include <iostream>
#include <cmath>
#include <cstdio>
#include <string>
#include <vector>

#include "functions.h"

using namespace std;
using namespace cv;

int main() {
    Mat image = imread("../data/test0.jpg", 1);

    imshow("Initial image", image);

    Mat thr_mat = image;

    Mat image_hsv;
    cvtColor(image, image_hsv, COLOR_BGR2HSV);

    thr_mat = get_thr_val_mat(image_hsv, 0.95);

    imshow("With threshold", thr_mat);

//    vector<vector<Point>> contours;
//    findContours( thr_mat, contours, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));
//
//    for (int i = 0; i < contours.size(); ++i)
//    {
//        Scalar color( 255, 255, 255 );
//        drawContours( image, contours, i, color, 3, 8 );
//    }
//
//    imshow("Contours", image);

//    double max_val;
//    cv::minMaxLoc(image, nullptr, &max_val);
//    cout << max_val << endl;

    Ptr<SimpleBlobDetector> detector = create_blob_detector();

    std::vector<KeyPoint> keypoints;
    detector->detect(thr_mat, keypoints);

    Mat im_with_keypoints;
    drawKeypoints(image, keypoints, im_with_keypoints, Scalar(0,0,255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

    cout << keypoints[0].pt << endl;

    imshow("Keypoints", im_with_keypoints);

    waitKey();
}
