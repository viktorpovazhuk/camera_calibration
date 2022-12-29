//
// Created by vivi on 17.11.22.
//

#ifndef CALIBRATION_LASER_SPOT_H
#define CALIBRATION_LASER_SPOT_H

#include <opencv2/opencv.hpp>

class laser_spot {
public:
    laser_spot() = default;
    laser_spot(cv::Point2f centroid, double diameter) : centroid(centroid), diameter(diameter) {}

    cv::Point2f centroid;
    double diameter = 0;
};


#endif //CALIBRATION_LASER_SPOT_H
