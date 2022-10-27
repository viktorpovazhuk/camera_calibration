//
// Created by vivi on 27.10.22.
//

#include <stdexcept>

#ifndef CALIBRATION_EXCEPTIONS_H
#define CALIBRATION_EXCEPTIONS_H

class dyn_thr_error : public std::runtime_error {
public:
    using runtime_error::runtime_error;
};

class num_spots_error : public std::runtime_error {
public:
    using runtime_error::runtime_error;
};

#endif //CALIBRATION_EXCEPTIONS_H
