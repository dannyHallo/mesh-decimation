add_library(src-camera Camera.cpp)
target_include_directories(src-camera PRIVATE
    ${dep_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/src/)

target_link_libraries(src-camera PRIVATE src-window glm::glm)
