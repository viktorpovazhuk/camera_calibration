//
// Created by vivi on 21.10.22.
//

#include "functions.h"
#include "exceptions.h"

#include <opencv2/opencv.hpp>
#include <boost/filesystem.hpp>
#include <iostream>
#include <cmath>
#include <cstdio>
#include <string>
#include <vector>


//#define TEST_IMG
//#define DEBUG_PROJECTOR

using namespace cv;
using namespace std;
namespace fs = boost::filesystem;

Point get_laser_coords(const Mat& image) {
    Mat image_hsv;
    cvtColor(image, image_hsv, COLOR_BGR2HSV);

    Mat thr_mat = get_thr_val_mat(image_hsv, 0.95);

#ifdef DEBUG_PROJECTOR
    namedWindow("Thresholded");
    imshow("Thresholded", thr_mat);
#endif

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

    Point2f laser_pt = key_points[0].pt;

    return laser_pt;
}

vector<Point2f> get_corners_coords(int camera_idx) {
    vector<Point2f> corners;

    while (corners.size() < 4) {
#ifdef TEST_IMG
        int i = corners.size();
        string fmt = "../data/test%d.jpg";
        size_t size_s = snprintf(nullptr, 0, fmt.c_str(), i) + 1;
        char buf[size_s];
        snprintf(buf, size_s, fmt.c_str(), i);
        string fn{buf};

        Mat image = imread(fn, 1);
#else
        cin.get();

        VideoCapture video_capture{camera_idx};

        Mat image;
        bool success = video_capture.read(image);

#ifdef DEBUG_PROJECTOR
        namedWindow("Original");
        imshow("Original", image);
#endif

        if (!success) {
            throw std::runtime_error{"Can't get photo from camera"};
        }
#endif
        Point corner = get_laser_coords(image);

        corners.emplace_back(corner);

#ifdef DEBUG_PROJECTOR
        waitKey();
        destroyAllWindows();
#endif
    }

    return corners;
}

int main() {
    int screen_height = 210, screen_width = 295;
    bool use_saved_homography = true;
    int camera_idx = 2;

    if (!VideoCapture{camera_idx}.isOpened()) {
        std::cerr << "Cannot open camera" << '\n';
        exit(EXIT_FAILURE);
    }

    // calibrate camera
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

        FileStorage fs("./configs/homography.xml",FileStorage::WRITE);
        fs << "hom" << hom_mat;
    }

    // get screen coordinates
    while (true) {
#ifdef TEST_IMG
        cin.get();

        Mat aim_mat = imread("../data/test_aim.jpg", 1);

        Mat transformed_mat;
        warpPerspective(aim_mat, transformed_mat, hom_mat, Size(screen_width, screen_height));

        imshow("Transformed image", transformed_mat);

        waitKey();
        // tries to create new window with existing the same -> fails
        destroyAllWindows();
#else
        cin.get();

        Mat aim_mat;
        VideoCapture video_capture{camera_idx};
        video_capture.read(aim_mat);
#endif
        try {
            Point laser_pt = get_laser_coords(aim_mat);

            vector<Point2d> camera_pos_pts{laser_pt};
            vector<Point2d> screen_pts;

            // function inside divides by 3rd coordinate because homography works in such way
            perspectiveTransform(camera_pos_pts, screen_pts, hom_mat);

            cout << "coords of laser on screen: " << screen_pts[0] << '\n';
        } catch (std::exception &ex) {
            cerr << ex.what() << '\n';
        }
    }
}