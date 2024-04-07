#pragma once

#include <daxa/daxa.hpp>
using namespace daxa::types;

#include "GLFW/glfw3.h"
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_NATIVE_INCLUDE_NONE
using HWND = void *;

#include "GLFW/glfw3native.h"

#include <unordered_map>

class Window {
public:
  Window(char const *window_name, uint32_t sx = 800, uint32_t sy = 600);

  ~Window();

  daxa::NativeWindowHandle getNativeHandle() const;

  void setMouseCapture(bool should_capture) const;

  bool shouldClose() const;

  void update() const;

  GLFWwindow *getGlfwWindow() const;

  bool shouldClose();

  bool getSwapchainOutOfDate() const { return _swapchainOutOfDate; }
  void setSwapchainOutOfDate(bool value) { _swapchainOutOfDate = value; }

  [[nodiscard]] bool isInputBitActive(int inputBit) {
    return _keyInputMap.contains(inputBit) && _keyInputMap[inputBit];
  }

  void disableInputBit(int bitToBeDisabled) { _keyInputMap[bitToBeDisabled] = false; }

private:
  GLFWwindow *_glfwWindowPtr;
  uint32_t _width;
  uint32_t _height;
  bool _minimized          = false;
  bool _swapchainOutOfDate = false;
  std::unordered_map<int, bool> _keyInputMap;

  static void _keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
};
