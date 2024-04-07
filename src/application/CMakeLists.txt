add_library(src-application STATIC
    Application.cpp
)

target_include_directories(src-application PRIVATE
    ${dep_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/src/
)

target_link_libraries(src-application PRIVATE
    daxa::daxa
    src-window
    src-camera
    src-obj-loader
)
