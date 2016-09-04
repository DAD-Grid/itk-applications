cmake_minimum_required(VERSION 2.8)

project(mergeImage)

find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

add_executable(mergeImage mergeImage.cxx)
target_link_libraries(mergeImage ${ITK_LIBRARIES})
