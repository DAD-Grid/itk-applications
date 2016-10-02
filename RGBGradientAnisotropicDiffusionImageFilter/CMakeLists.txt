cmake_minimum_required(VERSION 2.8)

project(RGBGradientAnisotropicDiffusionFilter)

find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

add_executable(anisotropicDiffusion RGBGradientAnisotropicDiffusionImageFilter.cxx)
target_link_libraries(anisotropicDiffusion ${ITK_LIBRARIES})
