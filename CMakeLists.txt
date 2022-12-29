cmake_minimum_required(VERSION 3.22)
project(laser_tool)

set(CMAKE_CXX_STANDARD 17)

set(TEST_APP_CALIBRATION library_test)
set(BASIC_LIB calibration)
set(TEST_APP_TOOL test_tool)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

set(OpenCV_DIR "D:/opencv/build")
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_library(${BASIC_LIB} calibration/include/matrix_operations.h calibration/src/matrix_operations.cpp
        calibration/include/exceptions.h calibration/src/laser_spot.cpp calibration/include/laser_spot.h
        calibration/src/calibration.cpp calibration/include/calibration.h
        calibration/src/laser_detection.cpp calibration/include/laser_detection.h calibration/include/config.h)
target_include_directories(${BASIC_LIB} PRIVATE calibration/include)
target_link_libraries(${BASIC_LIB} ${OpenCV_LIBS})

add_executable(${TEST_APP_CALIBRATION} "examples/test_calibration.cpp")
target_link_libraries(${TEST_APP_CALIBRATION} ${BASIC_LIB} ${OpenCV_LIBS})
target_include_directories(${TEST_APP_CALIBRATION} PRIVATE calibration/include)

add_executable(display_image examples/display_image.cpp)
target_link_libraries(display_image ${OpenCV_LIBS})
target_include_directories(display_image PRIVATE calibration/include)

add_executable(spot_centroid examples/diameter_centroid.cpp)
target_link_libraries(spot_centroid ${OpenCV_LIBS})
target_include_directories(spot_centroid PRIVATE calibration/include)

add_executable(contours examples/find_contours.cpp
                calibration/include/matrix_operations.h calibration/src/matrix_operations.cpp)
target_link_libraries(contours ${OpenCV_LIBS})
target_include_directories(contours PRIVATE calibration/include)

add_executable(video_capture examples/video_capture.cpp)
target_link_libraries(video_capture ${OpenCV_LIBS})
target_include_directories(video_capture PRIVATE calibration/include)

add_library(${PROJECT_NAME} SHARED laser_tool/include/matrix_operations.h laser_tool/src/matrix_operations.cpp
            laser_tool/include/laser_detector.h laser_tool/src/laser_detector.cpp
            laser_tool/include/laser_spot.h laser_tool/include/config.h
            laser_tool/include/shared_settings.h)
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})
target_include_directories(${PROJECT_NAME} PRIVATE laser_tool/include)

add_executable(${TEST_APP_TOOL} "examples/test_tool.cpp")
target_link_libraries(${TEST_APP_TOOL} ${PROJECT_NAME} ${OpenCV_LIBS})
target_include_directories(${TEST_APP_TOOL} PRIVATE laser_tool/include)

install(TARGETS ${BASIC_LIB} ${TEST_APP_CALIBRATION} ${PROJECT_NAME} ${TEST_APP_TOOL} RUNTIME DESTINATION .)