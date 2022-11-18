//
// Created by vivi on 17.11.22.
//

#include "matrix_operations.h"
#include "exceptions.h"
#include "laser_detection.h"
#include "laser_spot.h"
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

using point_vector = std::vector<cv::Point>;

vector<Point2f> calc_contours_centers(vector<point_vector> &contours) {
    vector<Moments> contours_moms(contours.size());
    vector<Point2f> contours_centers(contours.size());
    for (int i = 0; i < contours.size(); i++) {
        contours_moms[i] = moments(contours[i], false);
        if (contours_moms[i].m00 != 0) {
            contours_centers[i] = Point2f(contours_moms[i].m10 / contours_moms[i].m00,
                                          contours_moms[i].m01 / contours_moms[i].m00);
        } else {
            int x_sum = 0, y_sum = 0;
            for (Point &p: contours[i]) {
                x_sum += p.x;
                y_sum += p.y;
            }
            contours_centers[i] = Point2f((float) x_sum / contours[i].size(), (float) y_sum / contours[i].size());
        }
    }

    return contours_centers;
}

vector<laser_spot> get_laser_spots(Mat &image, vector<point_vector> &contours, vector<Point2f> &contours_centers) {
    vector<laser_spot> spots;
    for (int i = 0; i < contours.size(); i++) {
        int diameter = 2 * ceil(pow(contours[i].size() / 3.14, 0.5));
        Point2f center = contours_centers[i];

        Mat crop_mat(image, Rect(Point(center.x - diameter, center.y + diameter),
                                 Point(center.x + diameter, center.y - diameter)));

        if (check_laser_area(crop_mat)) {
            spots.emplace_back(contours_centers[i], diameter);
        }
    }

    return spots;
}

vector<point_vector> extract_contours(Mat &thr_mat) {
    vector<point_vector> all_contours;
    findContours(thr_mat, all_contours, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));

    vector<point_vector> contours;
    copy_if(all_contours.begin(), all_contours.end(), std::back_inserter(contours),
            [](point_vector &contour_pts) {
                return contour_pts.size() < 36;
            });

    return contours;
}

void check_laser_contrast(Mat &thr_mat) {
    int num_pixels_greater_thr = countNonZero(thr_mat);

    std::cout << "Num of pixels with value > 0.95 thr: " << num_pixels_greater_thr << '\n';

    if (num_pixels_greater_thr > 100) {
        throw dyn_thr_error{"Too many pixels with value > threshold: " + to_string(num_pixels_greater_thr)};
    }
}

Point get_laser_coords(Mat &image) {
    Mat image_hsv;
    cvtColor(image, image_hsv, COLOR_BGR2HSV);

    Mat thr_mat = get_thr_val_mat(image_hsv, 0.95);

#ifdef DEBUG_PROJECTOR
    namedWindow("Thresholded");
    imshow("Thresholded", thr_mat);
#endif

    check_laser_contrast(thr_mat);

    vector<point_vector> contours = extract_contours(thr_mat);

    vector<Point2f> contours_centers = calc_contours_centers(contours);

#ifdef DEBUG_PROJECTOR
    Mat spots_mat = thr_mat.clone();

    for (int i = 0; i < contours_centers.size(); ++i) {
        Scalar color(0, 0, 255);
        circle(spots_mat, contours_centers[i], 20, color, 1);
    }

    imshow("Found spots", spots_mat);
#endif

    vector<laser_spot> spots = get_laser_spots(image, contours, contours_centers);

    if (spots.size() > 1) {
        throw num_spots_error{"Too many light spots on picture: " + to_string(spots.size())};
    } else if (spots.empty()) {
        throw num_spots_error{"Didn't found spots"};
    }

    return spots[0].centroid;
}

vector<Point2f> get_corners_coords(int camera_idx) {
    vector<Point2f> corners;

    while (corners.size() < 4) {
#ifdef TEST_IMG
        int i = corners.size();
        string fmt = "../data/test/test%d.jpg";
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

#ifdef SAVE_IMAGES
        int i = corners.size();
        string fmt = "../data/samples/corner%d.jpg";
        size_t size_s = snprintf(nullptr, 0, fmt.c_str(), i) + 1;
        char buf[size_s];
        snprintf(buf, size_s, fmt.c_str(), i);
        string fn{buf};

        imwrite(fn, image);
#endif

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

Point2d get_spot_screen_coords(Mat &homography_mat, int screen_width, int screen_height, int camera_idx) {
    Mat aim_mat;

#ifdef TEST_IMG
    aim_mat = imread("../data/test/test_aim.jpg", 1);
#else
    VideoCapture video_capture{camera_idx};
    video_capture.read(aim_mat);
#endif

#ifdef SAVE_IMAGES
    imwrite("../data/samples/aim.jpg", aim_mat);
#endif

    Mat transformed_mat;
    warpPerspective(aim_mat, transformed_mat, homography_mat, Size(screen_width, screen_height));

#ifdef DEBUG_PROJECTOR
    imshow("Transformed image", transformed_mat);

    waitKey();
    // tries to create new window with existing the same -> fails
    destroyAllWindows();
#endif

#ifdef SAVE_IMAGES
    imwrite("../data/samples/aim_transformed.jpg", transformed_mat);
#endif

    try {
        Point laser_pt = get_laser_coords(aim_mat);

        vector<Point2d> camera_pos_pts{laser_pt};
        vector<Point2d> screen_pts;

        // function inside divides by 3rd coordinate because homography works in such way
        perspectiveTransform(camera_pos_pts, screen_pts, homography_mat);

        cout << "coords of laser on screen: " << screen_pts[0] << '\n';
    } catch (std::exception &ex) {
        cerr << ex.what() << '\n';
    }
}