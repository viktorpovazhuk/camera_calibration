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

class laser_spot {
public:
    laser_spot() = default;
    laser_spot(Point2f centroid, double radius) : centroid(centroid), radius(radius) {}

    Point2f centroid;
    double radius = 0;
};

int main() {
    Mat image = imread("../data/paint/one_pixel.jpg", IMREAD_COLOR);
//    Mat image = imread("../data/test0.jpg", IMREAD_COLOR);

//    imshow("Initial image", image);

    Mat image_hsv;
    cvtColor(image, image_hsv, COLOR_BGR2HSV);

    Mat thr_mat = get_thr_val_mat(image_hsv, 0.95);

//    imshow("With threshold", thr_mat);

    vector<vector<Point>> contours;
    findContours(thr_mat, contours, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));

    vector<Moments> mu(contours.size());
    for (int i = 0; i < contours.size(); i++) {
        mu[i] = moments(contours[i], false);
    }

    vector<Point2f> mc(contours.size());
    for (int i = 0; i < contours.size(); i++) {
        if (mu[i].m00 != 0) {
            mc[i] = Point2f(mu[i].m10 / mu[i].m00, mu[i].m01 / mu[i].m00);
        }
        else {
            int x_sum = 0, y_sum = 0;
            for (Point &p: contours[i]) {
                x_sum += p.x;
                y_sum += p.y;
            }
            mc[i] = Point2f((float)x_sum / contours[i].size(), (float)y_sum / contours[i].size());
        }
    }

    for (int i = 0; i < mc.size(); ++i) {
        Scalar color(0, 0, 255);
        circle(image, mc[i], 10, color, 1);
    }

    vector<laser_spot> spots;
    for (int i = 0; i < contours.size(); i++) {
        int radius = ceil(pow(contours[i].size(), 0.5) / 2);
        spots.emplace_back(mc[i], radius);
    }

    imshow("Contours", image);

    double max_val;
    cv::minMaxLoc(image, nullptr, &max_val);

//    Ptr<SimpleBlobDetector> detector = create_blob_detector();
//
//    std::vector<KeyPoint> keypoints;
//    detector->detect(thr_mat, keypoints);
//
//    Mat im_with_keypoints;
//    drawKeypoints(image, keypoints, im_with_keypoints, Scalar(0,0,255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
//
//    cout << keypoints[0].pt << endl;
//
//    imshow("Keypoints", im_with_keypoints);

    waitKey();
}
