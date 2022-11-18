cmake_minimum_required(VERSION 3.22)
project(calibration)

set(CMAKE_CXX_STANDARD 14)

find_package(Boost 1.71.0 COMPONENTS system regex filesystem REQUIRED)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${Boost_INCLUDE_DIR})

include_directories(calibration/include)

add_executable(${PROJECT_NAME} calibration/src/main.cpp calibration/include/matrix_operations.h calibration/src/matrix_operations.cpp
        calibration/include/exceptions.h calibration/src/laser_spot.cpp calibration/include/laser_spot.h
        calibration/src/calibration.cpp calibration/include/calibration.h
        calibration/src/laser_detection.cpp calibration/include/laser_detection.h calibration/include/config.h)
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})
target_link_libraries(${PROJECT_NAME} Boost::system Boost::filesystem Boost::regex)

add_executable(display_image examples/display_image.cpp)
target_link_libraries(display_image ${OpenCV_LIBS})

add_executable(spot_centroid examples/diameter_centroid.cpp)
target_link_libraries(spot_centroid ${OpenCV_LIBS})

add_executable(contours examples/find_contours.cpp
        calibration/include/matrix_operations.h calibration/src/matrix_operations.cpp)
target_link_libraries(contours ${OpenCV_LIBS})

add_executable(video_capture examples/video_capture.cpp)
target_link_libraries(video_capture ${OpenCV_LIBS})

add_executable(uart_read examples/uart_read.cpp)
target_link_libraries(uart_read Boost::system Boost::filesystem Boost::regex)