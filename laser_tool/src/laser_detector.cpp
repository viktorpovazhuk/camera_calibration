#include <opencv2/opencv.hpp>
#include <filesystem>
#include <iostream>
#include <cmath>
#include <cstdio>
#include <string>
#include <vector>
#include <algorithm>

#include "matrix_operations.h"
#include "laser_spot.h"
#include "config.h"

#include "laser_detector.h"

using point_vector = std::vector<cv::Point>;

void LaserDetector::open_capture() {
	m_video_capture.open(m_camera_idx);
}

void LaserDetector::close_capture() {
	m_video_capture.release();
}

void check_laser_contrast(cv::Mat& thr_mat) {
    int num_pixels_greater_thr = cv::countNonZero(thr_mat);
#ifndef UE_VERSION
    std::cout << "Num of pixels with value > 0.95 thr: " << num_pixels_greater_thr << '\n';
#endif
    if (num_pixels_greater_thr > 100) {
        throw std::runtime_error{ "Too many pixels with value > threshold: " + std::to_string(num_pixels_greater_thr) };
    }
}

point_vector extract_contour(cv::Mat& thr_mat) {
    std::vector<point_vector> all_contours;
    cv::Mat thr_mat_copy = thr_mat.clone();
    cv::findContours(thr_mat_copy, all_contours, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE, cv::Point(0, 0));

    //std::vector<point_vector> contours;
    //copy_if(all_contours.begin(), all_contours.end(), std::back_inserter(contours),
    //    [](point_vector& contour_pts) {
    //        return contour_pts.size() < 36;
    //    });

    //if (contours.size() > 1) {
    //    throw std::runtime_error{ "Too many contours: " + std::to_string(contours.size()) };
    //}

    return all_contours[0];
}

cv::Point2f calc_contour_center(point_vector& contour) {
    cv::Moments mom;
    cv::Point2f contour_center;
    mom = cv::moments(contour, false);
    if (mom.m00 != 0) {
        contour_center = cv::Point2f(mom.m10 / mom.m00,
            mom.m01 / mom.m00);
    }
    else {
        int x_sum = 0, y_sum = 0;
        for (cv::Point& p : contour) {
            x_sum += p.x;
            y_sum += p.y;
        }
        contour_center = cv::Point2f((float)x_sum / contour.size(), (float)y_sum / contour.size());
    }

    return contour_center;
}

laser_spot get_laser_spot(cv::Mat& image, point_vector& contour, cv::Point2f& contour_center) {
    double diameter = 2 * ceil(pow(contour.size() / 3.14, 0.5));
    cv::Mat crop_mat(image, cv::Rect(cv::Point(contour_center.x - diameter, contour_center.y + diameter),
        cv::Point(contour_center.x + diameter, contour_center.y - diameter)));

    if (check_laser_area(crop_mat)) {
        return laser_spot{ contour_center, diameter };
    }
    else {
        throw std::runtime_error{ "Incorrect proportion of greater than 0.9 thr in cropped area" };
    }
}

cv::Point2f get_laser_coords(cv::Mat& image) {
    cv::Mat image_hsv;
    cvtColor(image, image_hsv, cv::COLOR_BGR2HSV);

    cv::Mat thr_mat = get_thr_val_mat(image_hsv, 0.95);

    check_laser_contrast(thr_mat);

    point_vector contour = extract_contour(thr_mat);

    cv::Point2f contours_center = calc_contour_center(contour);

    laser_spot spot = get_laser_spot(image, contour, contours_center);

    return spot.centroid;
}

void LaserDetector::locate_screen_corner_coords() {
	cv::Mat image;
	bool success = m_video_capture.read(image);

	if (!success) {
		throw std::runtime_error{ "Can't get photo from camera for corner detection" };
	}

    locate_screen_corner_coords(image);
}

std::vector<double>	LaserDetector::locate_relative_laser_coords() {
    cv::Mat aim_mat;
    bool success = m_video_capture.read(aim_mat);

    if (!success) {
        throw std::runtime_error{ "Can't get photo from camera for relative laser coordinates" };
    }

    return locate_relative_laser_coords(aim_mat);
}

void LaserDetector::locate_screen_corner_coords(cv::Mat& image) {
    cv::Point2f corner = get_laser_coords(image);

    m_screen_corners_coords.emplace_back(corner);
}

std::vector<double>	LaserDetector::locate_relative_laser_coords(cv::Mat& aim_mat) {
    if (cv::countNonZero(m_homography_matrix) < 1) {
        m_homography_matrix = cv::findHomography(m_screen_corners_coords, m_relative_corners_coords);
    }

    cv::Mat transformed_mat;
    cv::warpPerspective(aim_mat, transformed_mat, m_homography_matrix, cv::Size(m_screen_width, m_screen_height));

    std::vector<cv::Point2d> screen_pts;

    cv::Point point = get_laser_coords(aim_mat);

    std::vector<cv::Point2d> camera_pos_pts{ point };

    // function inside divides by 3rd coordinate because homography works in such way
    perspectiveTransform(camera_pos_pts, screen_pts, m_homography_matrix);

    return std::vector<double>{screen_pts[0].x, screen_pts[0].y};
}