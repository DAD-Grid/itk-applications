cmake_minimum_required(VERSION 2.8)

project(CurvatureAnisotropicDiffusionImageFilter)

find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

add_executable(curvatureAnisotropicDiffusion CurvatureAnisotropicDiffusionImageFilter.cxx)
target_link_libraries(curvatureAnisotropicDiffusion ${ITK_LIBRARIES})
