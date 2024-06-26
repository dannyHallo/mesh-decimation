#include "Window.hpp"

Window::Window(char const *window_name, uint32_t sx, uint32_t sy) : _width{sx}, _height{sy} {
  // Initialize GLFW
  glfwInit();

  // Tell GLFW to not include any other API
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

  // Tell GLFW to make the window resizable
  glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

  // Create the window
  _glfwWindowPtr = glfwCreateWindow(static_cast<i32>(_width), static_cast<i32>(_height),
                                    window_name, nullptr, nullptr);

  // Set the user pointer to this window
  glfwSetWindowUserPointer(_glfwWindowPtr, this);

  // Enable vsync (To limit the framerate to the refresh rate of the monitor)
  glfwSwapInterval(1);

  // When the window is resized, update the width and height and mark the
  // swapchain as out of date
  glfwSetWindowContentScaleCallback(
      _glfwWindowPtr, [](GLFWwindow *window, float xscale, float yscale) {
        auto *win                = static_cast<Window *>(glfwGetWindowUserPointer(window));
        win->_width              = static_cast<uint32_t>(xscale);
        win->_height             = static_cast<uint32_t>(yscale);
        win->_swapchainOutOfDate = true;
      });

  glfwSetKeyCallback(_glfwWindowPtr, _keyCallback);
  glfwSetCursorPosCallback(_glfwWindowPtr, _cursorPosCallback);

  hideCursor();
}

Window::~Window() {
  glfwDestroyWindow(_glfwWindowPtr);
  glfwTerminate();
}

void Window::showCursor() {
  glfwSetInputMode(_glfwWindowPtr, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  _cursorState = CursorState::kVisible;
  glfwSetCursorPos(_glfwWindowPtr, static_cast<float>(_width) / 2.F,
                   static_cast<float>(_height) / 2.F);
}

void Window::hideCursor() {
  glfwSetInputMode(_glfwWindowPtr, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  if (glfwRawMouseMotionSupported() != 0) {
    glfwSetInputMode(_glfwWindowPtr, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
  }

  _cursorState = CursorState::kInvisible;
}

void Window::addMouseCallback(std::function<void(float, float)> callback) {
  _mouseCallbacks.emplace_back(std::move(callback));
}

void Window::_keyCallback(GLFWwindow *window, int key, int /*scancode*/, int action, int /*mods*/) {
  auto *thisWindowClass = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));

  if (action == GLFW_PRESS || action == GLFW_RELEASE) {
    thisWindowClass->_keyInputMap[key] = action == GLFW_PRESS;
  }
}

void Window::_cursorPosCallback(GLFWwindow *window, double xpos, double ypos) {
  auto *thisWindowClass = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));

  static float lastX;
  static float lastY;
  static bool firstMouse = true;

  if (firstMouse) {
    lastX      = static_cast<float>(xpos);
    lastY      = static_cast<float>(ypos);
    firstMouse = false;
  }

  thisWindowClass->_mouseDeltaX = static_cast<float>(xpos) - lastX;
  // inverted y axis
  thisWindowClass->_mouseDeltaY = lastY - static_cast<float>(ypos);

  lastX = static_cast<float>(xpos);
  lastY = static_cast<float>(ypos);

  if (thisWindowClass->_mouseCallbacks.empty()) {
    return;
  }

  for (auto &callback : thisWindowClass->_mouseCallbacks) {
    callback(thisWindowClass->_mouseDeltaX, thisWindowClass->_mouseDeltaY);
  }
}

daxa::NativeWindowHandle Window::getNativeHandle() const {
  return glfwGetWin32Window(_glfwWindowPtr);
}

void Window::setMouseCapture(bool should_capture) const {
  glfwSetCursorPos(_glfwWindowPtr, static_cast<f64>(_width / 2.), static_cast<f64>(_height / 2.));
  glfwSetInputMode(_glfwWindowPtr, GLFW_CURSOR,
                   should_capture ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
  glfwSetInputMode(_glfwWindowPtr, GLFW_RAW_MOUSE_MOTION, should_capture);
}

bool Window::shouldClose() const { return glfwWindowShouldClose(_glfwWindowPtr); }

void Window::update() const {
  glfwPollEvents();
  glfwSwapBuffers(_glfwWindowPtr);
}

GLFWwindow *Window::getGlfwWindow() const { return _glfwWindowPtr; }

bool Window::shouldClose() { return glfwWindowShouldClose(_glfwWindowPtr); }