//
// Created by vivi on 17.11.22.
//

#ifndef CALIBRATION_LASER_DETECTION_H
#define CALIBRATION_LASER_DETECTION_H

#include "laser_spot.h"

#include <opencv2/opencv.hpp>
#include <vector>

using namespace cv;
using namespace std;

using point_vector = std::vector<cv::Point>;

vector<Point2f> calc_contours_centers(vector<point_vector> &contours);
vector<laser_spot> get_laser_spots(Mat &image, vector<point_vector> &contours, vector<Point2f> &contours_centers);
vector<point_vector> extract_contours(Mat &thr_mat);
void check_laser_contrast(Mat &thr_mat);
Point get_laser_coords(Mat& image);
vector<Point2f> get_corners_coords(int camera_idx);
Point2d get_spot_screen_coords(Mat &homography_mat, int screen_width, int screen_height, int camera_idx);

#endif //CALIBRATION_LASER_DETECTION_H
