cmake_minimum_required(VERSION 2.8)

project(divideImage)

find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

add_executable(divideImage divideImage.cxx)
target_link_libraries(divideImage ${ITK_LIBRARIES})
