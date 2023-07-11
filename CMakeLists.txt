cmake_minimum_required (VERSION 2.8.11)
project(mlxflir)

SET(CMAKE_CXX_STANDARD 20)

set(SOURCE_FLIR
   ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/io/i2cunix.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/mlx90640/mlx90640.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/term/rawterminal.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/term/termutils.c
) 

set(SOURCE_FRAME
   ${CMAKE_CURRENT_SOURCE_DIR}/readframeraw.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/io/i2cunix.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/mlx90640/mlx90640.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/mlx90640/mlx90640util.cpp
)

add_executable(mlxflir ${SOURCE_FLIR})
add_executable(frameraw ${SOURCE_FRAME})