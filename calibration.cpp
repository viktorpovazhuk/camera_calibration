//
// Created by vivi on 21.10.22.
//

#include <opencv2/opencv.hpp>
#include <iostream>
#include <cmath>
#include <cstdio>
#include <string>
#include <vector>

using namespace cv;
using namespace std;

Point find_laser_spot(const Mat& image) {
    Mat image_hsv;
    cvtColor(image, image_hsv, COLOR_BGR2HSV);

    std::vector<cv::Mat> planes(3);
    Mat &val_mat = planes[2];

    cv::split(image_hsv, planes);
    double min, max;
    cv::minMaxLoc(val_mat, &min, &max);

    double dv_thr = max * 0.95;

    Mat val_thr;

    threshold(val_mat, val_thr, dv_thr, 255, THRESH_BINARY);
    int greater_thr = countNonZero(val_thr);

    std::cout << "num of greater than 0.95 thr: " <<  greater_thr << '\n';

    Moments m = moments(val_thr,true);
    Point centr(m.m10 / m.m00, m.m01 / m.m00);

    return centr;
}

int main() {
    vector<Point2f> pts_src;

    for (int i = 0; i < 4; i++) {
        string fmt = "../data/test%d.jpg";
        size_t size_s = snprintf(nullptr, 0, fmt.c_str(), i) + 1;
        char buf[size_s];
        snprintf(buf, size_s, fmt.c_str(), i);
        string fn{buf};

        Mat image = imread(fn, 1);

        Point centr = find_laser_spot(image);

        pts_src.emplace_back(centr);
    }

    float s_height = 210, s_width = 295;

    vector<Point2f> pts_dst;

    pts_dst.emplace_back(Point2f(0, 0));
    pts_dst.emplace_back(Point2f(s_width, 0));
    pts_dst.emplace_back(Point2f(s_width, s_height));
    pts_dst.emplace_back(Point2f(0, s_height));

    Mat hom = findHomography(pts_src, pts_dst);

    string aim_fn = "../data/test_aim.jpg";
    Mat im_src = imread(aim_fn, 1);
    Mat im_out;

    warpPerspective(im_src, im_out, hom, Size(s_width + 50, s_height + 50));

    imshow("Perspected image", im_out);

    Point aim = find_laser_spot(im_src);

//    cv::Mat_<double> aim_mat(3, 1);
//
//    aim_mat(0, 0)=aim.x;
//    aim_mat(1, 0)=aim.y;
//    aim_mat(2, 0)=1.0;
//
//    Mat_<double> proj_aim_mat = hom * aim_mat;
//    // divide because homography works in such way
//    proj_aim_mat = proj_aim_mat / proj_aim_mat(2, 0);
//
//    Point proj_aim{static_cast<int>(proj_aim_mat(0, 0)), static_cast<int>(proj_aim_mat(1, 0))};

    vector<Point2d> src_pts{aim};
    vector<Point2d> transformed_pts;

    perspectiveTransform(src_pts, transformed_pts, hom);

    cout << "projected coords of aim: " << transformed_pts[0] << endl;

    waitKey();

    return 0;
}