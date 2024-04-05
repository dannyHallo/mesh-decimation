cmake_minimum_required(VERSION 3.5.0)

# include("${CMAKE_CURRENT_LIST_DIR}/cmake/deps.cmake")

project(mesh-decimation)

file(GLOB_RECURSE SRCS ${PROJECT_SOURCE_DIR}/src/*.cpp)

message("src files:")

foreach(file ${SRCS})
    message(STATUS ${file})
endforeach()

add_executable(${PROJECT_NAME} ${SRCS})

target_include_directories(${PROJECT_NAME} PUBLIC include)

find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw)

find_package(daxa CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE daxa::daxa)
