add_library(src-obj-loader ObjectLoader.cpp)

target_include_directories(src-obj-loader PRIVATE
    ${dep_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/src/
)

target_link_libraries(src-obj-loader PRIVATE
    daxa::daxa
)
