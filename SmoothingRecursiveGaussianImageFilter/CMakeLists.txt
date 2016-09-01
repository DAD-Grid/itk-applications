cmake_minimum_required(VERSION 2.8)

project(SmoothingRecursiveGaussianImageFilter)

find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

add_executable(SmoothingRecursiveGaussianImageFilter SmoothingRecursiveGaussianImageFilter.cxx)
target_link_libraries(SmoothingRecursiveGaussianImageFilter ${ITK_LIBRARIES})
