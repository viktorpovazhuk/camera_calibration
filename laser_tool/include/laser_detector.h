#ifndef LASER_DETECTOR_H
#define LASER_DETECTOR_H

#include "shared_settings.h"

#include <vector>
#include <opencv2/opencv.hpp>

class LIB_EXPORT LaserDetector {
public:
	LaserDetector(int camera_idx, int screen_width, int screen_height) : m_camera_idx(camera_idx), m_screen_width(screen_width), m_screen_height(screen_height) {
		m_relative_corners_coords.emplace_back(cv::Point2f(0, 0));
		m_relative_corners_coords.emplace_back(cv::Point2f(screen_width, 0));
		m_relative_corners_coords.emplace_back(cv::Point2f(screen_width, screen_height));
		m_relative_corners_coords.emplace_back(cv::Point2f(0, screen_height));
	}
	~LaserDetector() {
		close_capture();
	}

	void open_capture();
	void close_capture();
	void locate_screen_corner_coords();
	void locate_screen_corner_coords(cv::Mat& image);
	std::vector<double>	locate_relative_laser_coords();
	std::vector<double>	locate_relative_laser_coords(cv::Mat& aim_mat);

private:
	int m_camera_idx = 0;
	std::vector<cv::Point2f> m_screen_corners_coords;
	std::vector<cv::Point2f> m_relative_corners_coords;
	cv::VideoCapture m_video_capture{};
	cv::Mat m_homography_matrix{};
	int m_screen_width;
	int m_screen_height;
};

#endif //LASER_DETECTOR_H