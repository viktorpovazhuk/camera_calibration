#include "laser_detector.h"

#include <opencv2/opencv.hpp>
#include <iostream>
#include <cmath>
#include <string>
#include <vector>

using namespace std;
using namespace cv;

#define TEST_IMG

int main() {
#ifdef TEST_IMG
    // set height and width of projector screen
    int screen_height = 200, screen_width = 300;
    // get camera ids: v4l2-ctl --list-devices
    int camera_idx = 0;

    LaserDetector laser_detector = LaserDetector(camera_idx, screen_width, screen_height);

    laser_detector.open_capture();

    for (int i = 0; i < 4; i++) {
        string fmt = "D:/VSProjects/source/repos/camera_calibration/data/test/test%d.jpg";
        char buf[300];
        snprintf(buf, sizeof(buf), fmt.c_str(), i);
        string fn{ buf };

        Mat image = imread(fn, 1);
        laser_detector.locate_screen_corner_coords(image);
    }

    std::string command;
    while (true) {
        std::getline(std::cin, command);
        if (command == "e") {
            break;
        }
        Mat aim_mat = imread("D:/VSProjects/source/repos/camera_calibration/data/test/test_aim.jpg", 1);
        std::vector<double> coords = laser_detector.locate_relative_laser_coords(aim_mat);
        std::cout << coords[0] << " " << coords[1] << endl;
    }
#else
    // set height and width of projector screen
    int screen_height = 200, screen_width = 300;
    // get camera ids: v4l2-ctl --list-devices
    int camera_idx = 0;

    LaserDetector laser_detector = LaserDetector(camera_idx, screen_width, screen_height);

    laser_detector.open_capture();

    for (int i = 0; i < 4; i++) {
        laser_detector.locate_screen_corner_coords();
    }

    std::string command;
    while (true) {
        std::getline(std::cin, command);
        if (command == "e") {
            break;
        }
        std::vector<double> coords = laser_detector.locate_relative_laser_coords();
        std::cout << coords[0] << " " << coords[1] << endl;
    }
#endif
}