//
// Created by vivi on 27.10.22.
//

#ifndef CV1_FUNCTIONS_H
#define CV1_FUNCTIONS_H

#include <opencv2/opencv.hpp>
#include <iostream>
#include <cmath>
#include <cstdio>
#include <vector>

cv::Mat get_thr_val_mat(const cv::Mat &mat, double thr_coef);

cv::Point get_laser_coords(const cv::Mat& image);

// deprecated
bool check_laser_area(const cv::Mat &area);

#endif //CV1_FUNCTIONS_H
