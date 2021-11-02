cmake_minimum_required( VERSION 2.8 )
project(technical_test_JS)
find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

add_executable(tech_test_JS main.cpp)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

target_link_libraries( tech_test_JS ${OpenCV_LIBS} )
