#pragma once

#include <daxa/daxa.hpp>
using namespace daxa::types;

#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_NATIVE_INCLUDE_NONE
using HWND = void *;

#include <GLFW/glfw3native.h>

struct Window {
  GLFWwindow *glfw_window_ptr;
  u32 width, height;
  bool minimized = false;
  bool swapchain_out_of_date = false;

  explicit Window(char const *window_name, u32 sx = 800, u32 sy = 600);

  ~Window();

  daxa::NativeWindowHandle getNativeHandle() const;

  void setMouseCapture(bool should_capture) const;

  bool shouldClose() const;

  void update() const;

  GLFWwindow *getGlfwWindow() const;

  bool shouldClose();
};
