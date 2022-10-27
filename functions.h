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

using namespace cv;
using namespace std;

Mat get_thr_val_mat(const Mat &mat, double thr_coef);

Ptr<SimpleBlobDetector> create_blob_detector();

// deprecated
bool check_laser_area(const Mat &area);

#endif //CV1_FUNCTIONS_H
