cmake_minimum_required(VERSION 3.23)

project(
  rz_convert_image
  VERSION 0.3.0
  DESCRIPTION "Plugin to convert image to webp sizes"
  HOMEPAGE_URL "https://github.com/Zheng-Bote/rz_convert_image"
  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PROG_EXEC_NAME "${PROJECT_NAME}")
set(PROG_NAME "convert Image")
set(PROG_VERSION "${PROJECT_VERSION}")
set(PROG_HOMEPAGE "${PROJECT_HOMEPAGE_URL}")
set(PROG_DESCRIPTION "${PROJECT_DESCRIPTION}")
set(PROG_ORGANIZATION_NAME "ZHENG Robert")
set(PROG_ORGANIZATION_DOMAIN "net.hase-zheng")

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  set(CMAKE_CXX_COMPILER clang++)
endif()

configure_file("${CMAKE_SOURCE_DIR}/configure/rz_config.h.in"
               "${CMAKE_SOURCE_DIR}/includes/rz_config.h")

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Gui Concurrent)

qt_standard_project_setup()

add_library(
  ${PROJECT_NAME} SHARED rz_convert_image.cpp includes/rz_convert_image.hpp
                         includes/rz_photo-gallery_plugins.hpp resources.qrc)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_23)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt::Core Qt6::Gui Qt6::Concurrent)

target_compile_definitions(${PROJECT_NAME} PRIVATE RZ_CONVERT_IMAGE_LIBRARY)
