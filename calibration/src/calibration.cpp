//
// Created by vivi on 17.11.22.
//

#include "calibration.h"
#include "laser_detection.h"
#include "config.h"

#include <opencv2/opencv.hpp>
#include <boost/filesystem.hpp>
#include <iostream>
#include <cmath>
#include <cstdio>
#include <string>
#include <vector>
#include <algorithm>

using namespace cv;
using namespace std;
namespace fs = boost::filesystem;

Mat find_homography_matrix(int camera_idx, bool use_saved_homography, int screen_width, int screen_height) {
    Mat hom_mat;
    if (fs::exists("./configs/homography.xml") && use_saved_homography) {
        FileStorage fs("./configs/homography.xml",FileStorage::READ);
        fs["hom"] >> hom_mat;
    }
    else {
        vector<Point2f> camera_pos_corners = get_corners_coords(camera_idx);

        vector<Point2f> screen_corners;
        screen_corners.emplace_back(Point2f(0, 0));
        screen_corners.emplace_back(Point2f(screen_width, 0));
        screen_corners.emplace_back(Point2f(screen_width, screen_height));
        screen_corners.emplace_back(Point2f(0, screen_height));

        hom_mat = findHomography(camera_pos_corners, screen_corners);

        FileStorage fs("../data/configs/homography.xml",FileStorage::WRITE);
        fs << "hom" << hom_mat;
    }

    return hom_mat;
}