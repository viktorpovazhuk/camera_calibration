//
// Created by vivi on 17.11.22.
//

#include <opencv2/opencv.hpp>

using namespace cv;

#ifndef CALIBRATION_CALIBRATION_H
#define CALIBRATION_CALIBRATION_H

Mat find_homography_matrix(int camera_idx, bool use_saved_homography, int screen_width, int screen_height);

#endif //CALIBRATION_CALIBRATION_H
