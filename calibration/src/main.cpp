//
// Created by vivi on 21.10.22.
//

#include "matrix_operations.h"
#include "exceptions.h"
#include "calibration.h"
#include "laser_detection.h"
#include "config.h"

#include <opencv2/opencv.hpp>
#include <iostream>
#include <cmath>
#include <string>
#include <vector>

int main() {
    // set height and width of projector screen
    int screen_height = 200, screen_width = 300;
    // load previously created matrix from file
    bool use_saved_homography = true;
    // get camera ids: v4l2-ctl --list-devices
    int camera_idx = 0;

    if (!VideoCapture{camera_idx}.isOpened()) {
        std::cerr << "Cannot open camera" << '\n';
        exit(EXIT_FAILURE);
    }

    // calibrate camera
    Mat hom_mat = find_homography_matrix(camera_idx, use_saved_homography, screen_width, screen_height);

    std::string command;
    while (true) {
        std::getline(std::cin, command);
        if (command == "e") {
            break;
        }
        get_spot_screen_coords(hom_mat, screen_width, screen_height, camera_idx);
    }
}