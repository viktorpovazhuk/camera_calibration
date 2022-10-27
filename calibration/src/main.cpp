//
// Created by vivi on 21.10.22.
//

#include "functions.h"
#include "exceptions.h"

#include <opencv2/opencv.hpp>
#include <iostream>
#include <cmath>
#include <cstdio>
#include <string>
#include <vector>

using namespace cv;
using namespace std;

Point get_laser_coords(const Mat& image) {
    Mat image_hsv;
    cvtColor(image, image_hsv, COLOR_BGR2HSV);

    Mat thr_mat = get_thr_val_mat(image_hsv, 0.95);

    int num_pixels_greater_thr = countNonZero(thr_mat);

    std::cout << "Num of greater than 0.95 thr: " << num_pixels_greater_thr << '\n';

    if (num_pixels_greater_thr > 100) {
        throw dyn_thr_error{"Too many pixels with value > threshold: " + to_string(num_pixels_greater_thr)};
    }

    Ptr<SimpleBlobDetector> detector = create_blob_detector();

    std::vector<KeyPoint> key_points;
    detector->detect(thr_mat, key_points);

    if (key_points.size() > 1) {
        throw num_spots_error{"Too many light spots on picture: " + to_string(key_points.size())};
    }
    else if (key_points.empty()) {
        throw num_spots_error{"Didn't found spots"};
    }

    Point2f centroid = key_points[0].pt;

    return centroid;
}

vector<Point2f> get_corners_coords() {
    vector<Point2f> corners_pts;

    for (int i = 0; i < 4; i++) {
        string fmt = "../data/test%d.jpg";
        size_t size_s = snprintf(nullptr, 0, fmt.c_str(), i) + 1;
        char buf[size_s];
        snprintf(buf, size_s, fmt.c_str(), i);
        string fn{buf};

        Mat image = imread(fn, 1);

        Point centroid = get_laser_coords(image);

        corners_pts.emplace_back(centroid);
    }

    return corners_pts;
}

void warp_aim_img(const Mat &aim_mat, const Mat &hom_mat, int screen_width, int screen_height) {
    Mat transformed_mat;

    warpPerspective(aim_mat, transformed_mat, hom_mat, Size(screen_width + 50, screen_height + 50));

    imshow("Transformed image", transformed_mat);
}

int main() {
    int screen_height = 210, screen_width = 295;

    vector<Point2f> pts_src = get_corners_coords();

    vector<Point2f> pts_dst;
    pts_dst.emplace_back(Point2f(0, 0));
    pts_dst.emplace_back(Point2f(screen_width, 0));
    pts_dst.emplace_back(Point2f(screen_width, screen_height));
    pts_dst.emplace_back(Point2f(0, screen_height));

    Mat hom_mat = findHomography(pts_src, pts_dst);

    Mat aim_mat = imread("../data/test_aim.jpg", 1);
    warp_aim_img(aim_mat, hom_mat, screen_width, screen_height);

    Point aim_pt = get_laser_coords(aim_mat);

    vector<Point2d> src_pts{aim_pt};
    vector<Point2d> transformed_pts;

    // inside divide by 3 coordinate because homography works in such way
    perspectiveTransform(src_pts, transformed_pts, hom_mat);

    cout << "projected coords of spot: " << transformed_pts[0] << '\n';

    waitKey();

    return 0;
}