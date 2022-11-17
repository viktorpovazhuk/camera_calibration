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
#include <algorithm>


//#define TEST_IMG
//#define DEBUG_PROJECTOR
//#define SAVE_IMAGES

using namespace cv;
using namespace std;
namespace fs = boost::filesystem;

class laser_spot {
public:
    laser_spot() = default;
    laser_spot(Point2f centroid, double diameter) : centroid(centroid), diameter(diameter) {}

    Point2f centroid;
    double diameter = 0;
};

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

    vector<vector<Point>> all_contours;
    vector<vector<Point>> contours;
    findContours(thr_mat, all_contours, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));
    copy_if(all_contours.begin(), all_contours.end(), std::back_inserter(contours),
            [] (vector<Point> &contour_pts) {
        return contour_pts.size() < 36;
    });

    vector<Moments> contours_moms(contours.size());
    vector<Point2f> contours_centers(contours.size());
    for (int i = 0; i < contours.size(); i++) {
        contours_moms[i] = moments(contours[i], false);
        if (contours_moms[i].m00 != 0) {
            contours_centers[i] = Point2f(contours_moms[i].m10 / contours_moms[i].m00, contours_moms[i].m01 / contours_moms[i].m00);
        }
        else {
            int x_sum = 0, y_sum = 0;
            for (Point &p: contours[i]) {
                x_sum += p.x;
                y_sum += p.y;
            }
            contours_centers[i] = Point2f((float)x_sum / contours[i].size(), (float)y_sum / contours[i].size());
        }
    }

#ifdef DEBUG_PROJECTOR
    Mat spots_mat = thr_mat.clone();

    for (int i = 0; i < contours_centers.size(); ++i) {
        Scalar color(0, 0, 255);
        circle(spots_mat, contours_centers[i], 20, color, 1);
    }

    imshow("Found spots", spots_mat);
#endif

    vector<laser_spot> spots;
    for (int i = 0; i < contours.size(); i++) {
        int diameter = 2 * ceil(pow(contours[i].size() / 3.14, 0.5));
        Point2f center = contours_centers[i];

        Mat crop_mat(image, Rect(Point(center.x-diameter, center.y+diameter), Point(center.x+diameter,center.y-diameter)));

        if (check_laser_area(crop_mat)) {
            spots.emplace_back(contours_centers[i], diameter);
        }
    }

    if (spots.size() > 1) {
        throw num_spots_error{"Too many light spots on picture: " + to_string(spots.size())};
    }
    else if (spots.empty()) {
        throw num_spots_error{"Didn't found spots"};
    }

    Point2f laser_pt = spots[0].centroid;

    return laser_pt;
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

int main() {
    int screen_height = 200, screen_width = 300;
    bool use_saved_homography = false;
    // v4l2-ctl --list-devices
    int camera_idx = 1;

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

        FileStorage fs("../data/configs/homography.xml",FileStorage::WRITE);
        fs << "hom" << hom_mat;
    }

    // get screen coordinates
    while (true) {

#ifdef TEST_IMG
        cin.get();

        Mat aim_mat = imread("../data/test/test_aim.jpg", 1);
#else
        cin.get();

        Mat aim_mat;
        VideoCapture video_capture{camera_idx};
        video_capture.read(aim_mat);
#endif

#ifdef SAVE_IMAGES
        imwrite("../data/samples/aim.jpg", aim_mat);
#endif

        Mat transformed_mat;
        warpPerspective(aim_mat, transformed_mat, hom_mat, Size(screen_width, screen_height));

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
            perspectiveTransform(camera_pos_pts, screen_pts, hom_mat);

            cout << "coords of laser on screen: " << screen_pts[0] << '\n';
        } catch (std::exception &ex) {
            cerr << ex.what() << '\n';
        }
    }
}