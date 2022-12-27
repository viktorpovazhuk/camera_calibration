cmake_minimum_required(VERSION 3.22)
project(calibration)

set(CMAKE_CXX_STANDARD 17)

set(TEST_APP library_test)

set(OpenCV_DIR "D:/opencv/build")
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

include_directories(calibration/include)

add_library(${PROJECT_NAME} calibration/include/matrix_operations.h calibration/src/matrix_operations.cpp
        calibration/include/exceptions.h calibration/src/laser_spot.cpp calibration/include/laser_spot.h
        calibration/src/calibration.cpp calibration/include/calibration.h
        calibration/src/laser_detection.cpp calibration/include/laser_detection.h calibration/include/config.h)
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})

add_executable(${TEST_APP} calibration/src/main.cpp)
target_link_libraries(${TEST_APP} ${PROJECT_NAME} ${OpenCV_LIBS})

add_executable(display_image examples/display_image.cpp)
target_link_libraries(display_image ${OpenCV_LIBS})

add_executable(spot_centroid examples/diameter_centroid.cpp)
target_link_libraries(spot_centroid ${OpenCV_LIBS})

add_executable(contours examples/find_contours.cpp
        calibration/include/matrix_operations.h calibration/src/matrix_operations.cpp)
target_link_libraries(contours ${OpenCV_LIBS})

add_executable(video_capture examples/video_capture.cpp)
target_link_libraries(video_capture ${OpenCV_LIBS})

install(TARGETS ${PROJECT_NAME} ${TEST_APP} RUNTIME DESTINATION .)