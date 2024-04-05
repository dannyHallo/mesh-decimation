cmake_minimum_required(VERSION 3.5.0)

# include("${CMAKE_CURRENT_LIST_DIR}/cmake/deps.cmake")
project(mesh-decimation)

# glfw
find_package(glfw3 CONFIG REQUIRED)

# daxa::daxa
find_package(daxa CONFIG REQUIRED)

set(vcpkg_INCLUDE_DIR "${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include/")

add_subdirectory(src/)
add_subdirectory(apps/)
