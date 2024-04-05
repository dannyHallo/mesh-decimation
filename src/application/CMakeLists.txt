add_library(src-application STATIC
    Application.cpp
)

target_include_directories(src-application PRIVATE
    ${vcpkg_INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR}/src/
)

target_link_libraries(src-application PRIVATE
    daxa::daxa
    src-window
)
