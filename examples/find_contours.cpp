//
// Created by vivi on 27.10.22.
//

#include <opencv2/opencv.hpp>
#include <iostream>
#include <cmath>
#include <cstdio>
#include <string>
#include <vector>

#include "functions.h"

using namespace std;
using namespace cv;

class laser_spot {
public:
    laser_spot() = default;
    laser_spot(Point2f centroid, double radius) : centroid(centroid), radius(radius) {}

    Point2f centroid;
    double radius = 0;
};

Mat get_hv_thr_val_mat(const Mat &mat, double thr_coef) {
    std::vector<cv::Mat> planes(3);
    cv::split(mat, planes);

    Mat val_mat = planes[2];
    Mat hue_mat = planes[0];

    double max_val;
    cv::minMaxLoc(val_mat, nullptr, &max_val);

    double thr_val = max_val * thr_coef;

    Mat val_thr_mat;
    threshold(val_mat, val_thr_mat, thr_val, 255, THRESH_BINARY);

//    Mat hue_thr_mat;
//    threshold(hue_mat, hue_thr_mat, 60, 255, THRESH_BINARY);
//
//    val_thr_mat.setTo(0, hue_thr_mat);

    return val_thr_mat;
}

int main(int argc, char *argv[]) {
//    Mat image = imread("../data/paint/one_pixel.jpg", IMREAD_COLOR);
    Mat image = imread("/home/vivi/Downloads/" + string(argv[1]) + ".jpg", IMREAD_COLOR);

//    imshow("Initial image", image);

    Mat image_hsv;
    cvtColor(image, image_hsv, COLOR_BGR2HSV);

    Mat thr_mat = get_hv_thr_val_mat(image_hsv, 0.95);

//    imshow("With threshold", thr_mat);

    vector<vector<Point>> contours;
    findContours(thr_mat, contours, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));

    vector<Moments> mu(contours.size());
    for (int i = 0; i < contours.size(); i++) {
        mu[i] = moments(contours[i], false);
    }

    vector<Point2f> mc(contours.size());
    for (int i = 0; i < contours.size(); i++) {
        if (mu[i].m00 != 0) {
            mc[i] = Point2f(mu[i].m10 / mu[i].m00, mu[i].m01 / mu[i].m00);
        }
        else {
            int x_sum = 0, y_sum = 0;
            for (Point &p: contours[i]) {
                x_sum += p.x;
                y_sum += p.y;
            }
            mc[i] = Point2f((float)x_sum / contours[i].size(), (float)y_sum / contours[i].size());
        }
    }

    for (int i = 0; i < mc.size(); ++i) {
        Scalar color(0, 0, 255);
        circle(image, mc[i], 10, color, 1);
    }

    vector<laser_spot> spots;
    for (int i = 0; i < contours.size(); i++) {
        int radius = ceil(pow(contours[i].size(), 0.5) / 2);
        spots.emplace_back(mc[i], radius);
    }

    imshow("Contours", image);

    std::vector<cv::Mat> hsv_planes(3);
    cv::split(image_hsv, hsv_planes);

//    for (int cont_i = 0; cont_i < contours.size(); cont_i++) {
//        // plot histogram of H S V
//
//        Mat h_pixels(1, contours[cont_i].size(), CV_8UC1, Scalar(0) );
//        Mat s_pixels(1, contours[cont_i].size(), CV_8UC1, Scalar(0) );
//        for (int i = 0; i < contours[cont_i].size(); ++i) {
//            Point p = contours[cont_i][i];
//            h_pixels.at<uchar>(0, i) = hsv_planes[0].at<uchar>(p.y, p.x);
//            s_pixels.at<uchar>(0, i) = hsv_planes[1].at<uchar>(p.y, p.x);
//        }
//
//        int h_hist_size = 180;
//        int s_hist_size = 256;
//
//        float h_range[] = { 0, 180 }; //the upper boundary is exclusive
//        float s_range[] = { 0, 256 };
//
//        const float* h_hist_range[] = { h_range };
//        const float* s_hist_range[] = { s_range };
//
//        bool uniform = true, accumulate = false;
//
//        Mat h_hist, s_hist;
//        calcHist( &h_pixels, 1, 0, Mat(), h_hist, 1, &h_hist_size, h_hist_range, uniform, accumulate );
//        calcHist( &s_pixels, 1, 0, Mat(), s_hist, 1, &s_hist_size, s_hist_range, uniform, accumulate );
//
//        int hist_w = 512, hist_h = 400;
//        int bin_w = cvRound( (double) hist_w/h_hist_size );
//        Mat histImage( hist_h, hist_w, CV_8UC3, Scalar( 0, 0, 0) );
//        normalize(h_hist, h_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat() );
//        normalize(s_hist, s_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat() );
//
//        for( int i = 1; i < h_hist_size; i++ )
//        {
//            line( histImage, Point( bin_w*(i-1), hist_h - cvRound(h_hist.at<float>(i-1)) ),
//                  Point( bin_w*(i), hist_h - cvRound(h_hist.at<float>(i)) ),
//                  Scalar( 255, 0, 0), 2, 8, 0  ); // blue
//        }
//        for( int i = 1; i < s_hist_size; i++ )
//        {
//            line( histImage, Point( bin_w*(i-1), hist_h - cvRound(s_hist.at<float>(i-1)) ),
//                  Point( bin_w*(i), hist_h - cvRound(s_hist.at<float>(i)) ),
//                  Scalar( 0, 255, 0), 2, 8, 0  ); // green
//        }
//
//        imshow("Hue source image", h_pixels);
//        imshow("Saturation source image", s_pixels);
//
//        imshow("calcHist result", histImage );
//
//        waitKey();
//        destroyAllWindows();
//    }

    waitKey();
}
