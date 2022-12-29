//
// Created by vivi on 27.10.22.
//
#include <opencv2/opencv.hpp>
#include <iostream>
#include <cmath>
#include <cstdio>
#include <vector>

#include "config.h"

#include "matrix_operations.h"

using namespace cv;
using namespace std;

Mat get_thr_val_mat(const Mat &mat, double thr_coef) {
    std::vector<cv::Mat> planes(3);
    cv::split(mat, planes);

    Mat val_mat = planes[2];

    double max_val;
    cv::minMaxLoc(val_mat, nullptr, &max_val);

    double thr_val = max_val * thr_coef;

    Mat thr_mat;
    threshold(val_mat, thr_mat, thr_val, 255, THRESH_BINARY);

    return thr_mat;
}

bool check_laser_area(const Mat &area) {
    Mat thr_mat = get_thr_val_mat(area, 0.9);
    int num_pixels_greater_thr = countNonZero(thr_mat);
#ifndef UE_VERSION
    std::cout << "number of pixels greater than 0.9 thr in cropped area, total number: " <<  num_pixels_greater_thr << ", " << area.total() << '\n';
    std::cout << "proportion of greater than 0.9 thr in cropped area: " <<  num_pixels_greater_thr / (float)area.total() << '\n';
#endif
    return num_pixels_greater_thr / (float)area.total() < 0.6;
}