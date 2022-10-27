cmake_minimum_required(VERSION 3.22)
project(cv1)

set(CMAKE_CXX_STANDARD 14)

find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

add_executable(display_image display_image.cpp)
target_link_libraries(display_image ${OpenCV_LIBS})

add_executable(spot_centroid spot_centroid.cpp)
target_link_libraries(spot_centroid ${OpenCV_LIBS})

add_executable(calibration calibration.cpp functions.h functions.cpp)
target_link_libraries(calibration ${OpenCV_LIBS})

add_executable(contours contours.cpp functions.h functions.cpp)
target_link_libraries(contours ${OpenCV_LIBS})