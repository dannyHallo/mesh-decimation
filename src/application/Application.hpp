#pragma once

#include <daxa/daxa.hpp>
#include <daxa/utils/pipeline_manager.hpp>
#include <daxa/utils/task_graph.hpp>

#include <memory>

class Window;

class Application {
public:
  Application();
  ~Application();

  // disable copy and move
  Application(Application const &)            = delete;
  Application(Application &&)                 = delete;
  Application &operator=(Application const &) = delete;
  Application &operator=(Application &&)      = delete;

  void run();

private:
  std::unique_ptr<Window> _window;

  daxa::Instance _instance;
  daxa::Device _device;
  daxa::Swapchain _swapchain;
  daxa::PipelineManager _pipelineManager;
  std::shared_ptr<daxa::RasterPipeline> _rasterPipeline;

  daxa::BufferId _vertexBufferId;

  daxa::TaskImage _taskSwapchainImage;
  daxa::TaskBuffer _taskVertexBuffer;
  daxa::TaskGraph _uploadTaskGraph;
  daxa::TaskGraph _renderTaskGraph;

  void _init();
  void _createInstance();
  void _createDevice();
  void _createSwapchain();
  void _createPipelineManager();
  void _createRasterPipeline();
  void _createBuffers();

  void _createTaskGraphs();
  void _createTaskResources();
  void _createUploadTaskGraph();
  void _createRenderTaskGraph();

  void _createCommandBuffers();
};