#pragma once

#include <memory>

class Window;

class Application {
public:
  Application();
  ~Application();

  // disable copy and move
  Application(Application const &) = delete;
  Application(Application &&) = delete;
  Application &operator=(Application const &) = delete;
  Application &operator=(Application &&) = delete;

  void run();

private:
  std::unique_ptr<Window> window;
};