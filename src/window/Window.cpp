#include "Window.hpp"

Window::Window(char const *window_name, u32 sx, u32 sy) : width{sx}, height{sy} {
  // Initialize GLFW
  glfwInit();

  // Tell GLFW to not include any other API
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

  // Tell GLFW to make the window resizable
  glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

  // Create the window
  glfw_window_ptr = glfwCreateWindow(static_cast<i32>(width), static_cast<i32>(height), window_name,
                                     nullptr, nullptr);

  // Set the user pointer to this window
  glfwSetWindowUserPointer(glfw_window_ptr, this);

  // Enable vsync (To limit the framerate to the refresh rate of the monitor)
  glfwSwapInterval(1);

  // When the window is resized, update the width and height and mark the
  // swapchain as out of date
  glfwSetWindowContentScaleCallback(
      glfw_window_ptr, [](GLFWwindow *window, float xscale, float yscale) {
        auto *win                  = static_cast<Window *>(glfwGetWindowUserPointer(window));
        win->width                 = static_cast<u32>(xscale);
        win->height                = static_cast<u32>(yscale);
        win->swapchain_out_of_date = true;
      });
}

Window::~Window() {
  glfwDestroyWindow(glfw_window_ptr);
  glfwTerminate();
}

daxa::NativeWindowHandle Window::getNativeHandle() const {
  return glfwGetWin32Window(glfw_window_ptr);
}

void Window::setMouseCapture(bool should_capture) const {
  glfwSetCursorPos(glfw_window_ptr, static_cast<f64>(width / 2.), static_cast<f64>(height / 2.));
  glfwSetInputMode(glfw_window_ptr, GLFW_CURSOR,
                   should_capture ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
  glfwSetInputMode(glfw_window_ptr, GLFW_RAW_MOUSE_MOTION, should_capture);
}

bool Window::shouldClose() const { return glfwWindowShouldClose(glfw_window_ptr); }

void Window::update() const {
  glfwPollEvents();
  glfwSwapBuffers(glfw_window_ptr);
}

GLFWwindow *Window::getGlfwWindow() const { return glfw_window_ptr; }

bool Window::shouldClose() { return glfwWindowShouldClose(glfw_window_ptr); }