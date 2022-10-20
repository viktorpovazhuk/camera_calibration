cmake_minimum_required(VERSION 3.22)
project(cv1)

set(CMAKE_CXX_STANDARD 14)

find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

add_executable(display_image display_image.cpp)
target_link_libraries(display_image ${OpenCV_LIBS})

add_executable(cv1 main.cpp)
target_link_libraries(cv1 ${OpenCV_LIBS})