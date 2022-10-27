cmake_minimum_required(VERSION 3.22)
project(calibration)

set(CMAKE_CXX_STANDARD 14)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

include_directories(calibration/include)

add_executable(calibration calibration/src/main.cpp calibration/include/functions.h calibration/src/functions.cpp calibration/include/exceptions.h)
target_link_libraries(calibration ${OpenCV_LIBS})

add_executable(display_image examples/display_image.cpp)
target_link_libraries(display_image ${OpenCV_LIBS})

add_executable(spot_centroid examples/diameter_centroid.cpp)
target_link_libraries(spot_centroid ${OpenCV_LIBS})

add_executable(contours examples/contours.cpp
        calibration/include/functions.h calibration/src/functions.cpp)
target_link_libraries(contours ${OpenCV_LIBS})