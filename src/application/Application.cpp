#include "Application.hpp"

#include "window/Window.hpp"

#include "camera/Camera.hpp"
#include "shared/PushConstants.inl"

#include <chrono>
#include <iostream>

using namespace daxa::types;

namespace {
daxa::types::f32mat4x4 toDaxaMat4x4(glm::mat4 const &mat) {
  return daxa::types::f32mat4x4{mat[0][0], mat[0][1], mat[0][2], mat[0][3], //
                                mat[1][0], mat[1][1], mat[1][2], mat[1][3], //
                                mat[2][0], mat[2][1], mat[2][2], mat[2][3], //
                                mat[3][0], mat[3][1], mat[3][2], mat[3][3]};
}

} // namespace

struct UploadVertexDataTask {
  struct Uses {
    MyModel *model;
    daxa::BufferTransferWrite vertexBuffer{};
  } uses = {};

  std::string_view name = "upload vertices";

  void callback(daxa::TaskInterface ti) {
    auto commandList = ti.get_command_list();

    size_t vertexCount = uses.model->vertices.size();

    auto data = std::vector<G_Vertex>{};
    data.reserve(vertexCount);
    for (auto const &vertex : uses.model->vertices) {
      data.push_back(vertex);
    }

    // auto data = std::array{
    //     G_Vertex{.position = {-0.5f, +0.5f, 0.F}, .color = {1.F, 0.F, 0.F}},
    //     G_Vertex{.position = {+0.5f, +0.5f, 0.F}, .color = {0.F, 1.F, 0.F}},
    //     G_Vertex{.position = {+0.F, -0.5f, 0.F}, .color = {0.F, 0.F, 1.F}},
    // };

    u32 size = sizeof(G_Vertex) * data.size();
    std::cout << "Size: " << size << std::endl;

    auto stagingBufferId = ti.get_device().create_buffer({
        .size          = size,
        .allocate_info = daxa::MemoryFlagBits::HOST_ACCESS_RANDOM,
        .name          = "my staging buffer",
    });

    commandList.destroy_buffer_deferred(stagingBufferId);

    // TODO: fix me
    auto *bufferPtr = ti.get_device().get_host_address_as<G_Vertex *>(stagingBufferId);

    memcpy(bufferPtr, data.data(), size);

    commandList.copy_buffer_to_buffer({
        .src_buffer = stagingBufferId,
        .dst_buffer = uses.vertexBuffer.buffer(),
        .size       = size,
    });
  }
};

struct DrawToSwapchainTask {
  struct Uses {
    Camera *camera;

    u32 modelVertCount;

    // We declare a vertex buffer read. Later we assign the task vertex buffer
    // handle to this use.
    daxa::BufferVertexShaderRead vertexBuffer{};
    daxa::BufferVertexShaderRead uboBuffer{};

    // We declare a color target. We will assign the swapchain task image to
    // this later. The name `ImageColorAttachment<T_VIEW_TYPE = DEFAULT>` is a
    // typedef for
    // `daxa::TaskImageUse<daxa::TaskImageAccess::COLOR_ATTACHMENT,
    // T_VIEW_TYPE>`.
    daxa::ImageColorAttachment<> colorTarget{};
  } uses = {};

  daxa::RasterPipeline *pipeline = {};

  std::string_view name = "draw task";

  void callback(daxa::TaskInterface ti) {
    auto commandList = ti.get_command_list();

    // TODO: fix me
    auto pMat  = uses.camera->getProjectionMatrix(860.F / 640.F);
    auto vMat  = uses.camera->getViewMatrix();
    auto vpMat = pMat * vMat;

    auto modelRotMat  = glm::rotate(glm::mat4(1.F), glm::radians(-90.F), glm::vec3(1.F, 0.F, 0.F));
    auto modelRotMat2 = glm::rotate(glm::mat4(1.F), glm::radians(225.F), glm::vec3(0.F, 1.F, 0.F));
    auto modelTranslateMat = glm::translate(glm::mat4(1.F), glm::vec3(0.F, 0.F, 0.F));

    auto data = G_Ubo{
        .vpMat    = toDaxaMat4x4(vpMat),
        .modelMat = toDaxaMat4x4(modelTranslateMat * modelRotMat2 * modelRotMat),
    };

    auto stagingBufferId = ti.get_device().create_buffer({
        .size          = sizeof(data),
        .allocate_info = daxa::MemoryFlagBits::HOST_ACCESS_RANDOM,
        .name          = "my staging buffer",
    });

    commandList.destroy_buffer_deferred(stagingBufferId);

    auto *bufferPtr = ti.get_device().get_host_address_as<G_Ubo>(stagingBufferId);

    *bufferPtr = data;
    commandList.copy_buffer_to_buffer({
        .src_buffer = stagingBufferId,
        .dst_buffer = uses.uboBuffer.buffer(),
        .size       = sizeof(data),
    });

    auto const size_x = ti.get_device().info_image(uses.colorTarget.image()).size.x;
    auto const size_y = ti.get_device().info_image(uses.colorTarget.image()).size.y;

    commandList.begin_renderpass({
        .color_attachments =
            std::vector{
                daxa::RenderAttachmentInfo{
                    .image_view  = uses.colorTarget.view(),
                    .load_op     = daxa::AttachmentLoadOp::CLEAR,
                    .clear_value = std::array<daxa::f32, 4>{0.1f, 0.F, 0.5f, 1.F},
                },
            },
        .render_area = {.x = 0, .y = 0, .width = size_x, .height = size_y},
    });

    commandList.set_pipeline(*pipeline);
    commandList.push_constant(
        MyPushConstant{.vertexPtr = ti.get_device().get_device_address(uses.vertexBuffer.buffer()),
                       .uboPtr    = ti.get_device().get_device_address(uses.uboBuffer.buffer())});

    commandList.draw(daxa::DrawInfo{.vertex_count = uses.modelVertCount});
    commandList.end_renderpass();
  }
};

void Application::_init() {
  _createInstance();
  _createDevice();
  _createSwapchain();
  _createPipelineManager();
  _createRasterPipeline();

  _loadModel("C:/Users/danny/Desktop/mesh-decimation/resources/viking_room.obj");
  _createBuffers();
  _createTaskGraphs();
}

void Application::_createInstance() { _instance = daxa::create_instance({}); }

void Application::_createDevice() {
  _device = _instance.create_device({
      .selector = [](daxa::DeviceProperties const &device_props) -> daxa::i32 {
        daxa::i32 score = 0;
        switch (device_props.device_type) {
        case daxa::DeviceType::DISCRETE_GPU:
          score += 10000;
          break;
        case daxa::DeviceType::VIRTUAL_GPU:
          score += 1000;
          break;
        case daxa::DeviceType::INTEGRATED_GPU:
          score += 100;
          break;
        default:
          break;
        }
        score += static_cast<daxa::i32>(device_props.limits.max_memory_allocation_count / 100000);
        return score;
      },
      .name = "my device",
  });
}

void Application::_createSwapchain() {
  _swapchain = _device.create_swapchain(daxa::SwapchainInfo{
      // this handle is given by the windowing API
      .native_window = _window->getNativeHandle(),

      // The platform would also be retrieved from the windowing API,
      // or by hard-coding it depending on the OS.
      .native_window_platform = daxa::NativeWindowPlatform::WIN32_API,

      // Here we can supply a user-defined surface format selection
      // function, to rate formats. If you don't care what format the
      // swapchain images are in, then you can just omit this argument
      // because it defaults to `daxa::default_format_score(...)`
      .surface_format_selector =
          [](daxa::Format format) {
            switch (format) {
            case daxa::Format::R8G8B8A8_UINT:
              return 100;
            default:
              return daxa::default_format_score(format);
            }
          },
      .present_mode = daxa::PresentMode::MAILBOX,
      .image_usage  = daxa::ImageUsageFlagBits::TRANSFER_DST,
      .name         = "my swapchain",
  });
}

void Application::_createPipelineManager() {
  _pipelineManager = daxa::PipelineManager(daxa::PipelineManagerInfo{
      .device = _device,
      .shader_compile_options =
          {
              .root_paths =
                  {
                      DAXA_SHADER_INCLUDE_DIR,
                      "C:/Users/danny/Desktop/mesh-decimation/src/shaders",
                      "C:/Users/danny/Desktop/mesh-decimation/src/shared",
                  },
              .language          = daxa::ShaderLanguage::GLSL,
              .enable_debug_info = true,
          },
      .name = "my pipeline manager",
  });
}

void Application::_createRasterPipeline() {
  auto result = _pipelineManager.add_raster_pipeline({
      .vertex_shader_info   = daxa::ShaderCompileInfo{.source = daxa::ShaderFile{"main.glsl"}},
      .fragment_shader_info = daxa::ShaderCompileInfo{.source = daxa::ShaderFile{"main.glsl"}},
      .color_attachments    = {{.format = _swapchain.get_format()}},
      .raster               = {},
      .push_constant_size   = sizeof(MyPushConstant),
      .name                 = "my pipeline",
  });
  if (result.is_err()) {
    std::cerr << result.message() << std::endl;
    return;
  }
  _rasterPipeline = result.value();
}

void Application::_loadModel(std::string const &&path) {
  _model = std::make_unique<MyModel>(ObjectLoader::loadObjModel(std::move(path)));

  size_t vertexCount = _model->vertices.size();
  // print at most the first 10 vertices
  for (size_t i = 0; i < std::min(vertexCount, static_cast<size_t>(10)); ++i) {
    auto const &vertex = _model->vertices[i];
  }
}

void Application::_createBuffers() {
  _vertexBufferId = _device.create_buffer({
      .size = static_cast<u32>(sizeof(G_Vertex) * _model->vertices.size()),
      .name = "vertex buffers - data of vertices",
  });

  _uboBufferId = _device.create_buffer({
      .size = sizeof(G_Ubo),
      .name = "ubo buffer - all the small bindings",
  });
}

void Application::_createTaskGraphs() {
  _createTaskResources();
  _createUploadTaskGraph();
  _createRenderTaskGraph();
}

void Application::_createTaskResources() {
  _taskSwapchainImage =
      daxa::TaskImage{daxa::TaskImageInfo{.swapchain_image = true, .name = "swapchain image"}};

  _taskVertexBuffer = daxa::TaskBuffer({
      .initial_buffers = {.buffers = std::span{&_vertexBufferId, 1}},
      .name            = "task vertex buffer",
  });

  _taskUboBuffer = daxa::TaskBuffer({
      .initial_buffers = {.buffers = std::span{&_uboBufferId, 1}},
      .name            = "task ubo buffer",
  });
}

void Application::_createUploadTaskGraph() {
  _uploadTaskGraph = daxa::TaskGraph({
      .device = _device,
      .name   = "upload",
  });

  _uploadTaskGraph.use_persistent_buffer(_taskVertexBuffer);

  _uploadTaskGraph.add_task(UploadVertexDataTask{
      .uses =
          {
              .model        = _model.get(),
              .vertexBuffer = _taskVertexBuffer.view(),
          },
  });

  _uploadTaskGraph.submit({});
  _uploadTaskGraph.complete({});
}

void Application::_createRenderTaskGraph() {
  _renderTaskGraph = daxa::TaskGraph({
      .device    = _device,
      .swapchain = _swapchain,
      .name      = "loop",
  });

  _renderTaskGraph.use_persistent_buffer(_taskVertexBuffer);
  _renderTaskGraph.use_persistent_buffer(_taskUboBuffer);
  _renderTaskGraph.use_persistent_image(_taskSwapchainImage);

  _renderTaskGraph.add_task(DrawToSwapchainTask{
      .uses =
          {
              .camera         = _camera.get(),
              .modelVertCount = static_cast<u32>(_model->vertices.size()),
              .vertexBuffer   = _taskVertexBuffer.view(),
              .uboBuffer      = _taskUboBuffer.view(),
              .colorTarget    = _taskSwapchainImage.view(),
          },
      .pipeline = _rasterPipeline.get(),
  });

  _renderTaskGraph.submit({});
  _renderTaskGraph.present({});

  // we complete the task graph, which essentially compiles the
  // dependency graph between tasks, and inserts the most optimal
  // synchronization!
  _renderTaskGraph.complete({});
}

Application::Application() : _window(std::make_unique<Window>("Mesh Decimation Test", 860, 640)) {
  _camera = std::make_unique<Camera>(_window.get());

  _window->addMouseCallback([this](float mouseDeltaX, float mouseDeltaY) {
    _camera->handleMouseMovement(mouseDeltaX, mouseDeltaY);
  });

  _init();
}

Application::~Application() {
  _device.destroy_buffer(_vertexBufferId);
  _device.destroy_buffer(_uboBufferId);
}

void Application::run() {
  static std::chrono::time_point fpsRecordLastTime = std::chrono::steady_clock::now();

  _uploadTaskGraph.execute({});

  while (!_window->shouldClose()) {
    _window->update();

    auto now = std::chrono::steady_clock::now();
    auto delta =
        std::chrono::duration_cast<std::chrono::duration<double>>(now - fpsRecordLastTime).count();
    fpsRecordLastTime = now;
    _camera->processInput(delta);

    if (_window->getSwapchainOutOfDate()) {
      _swapchain.resize();
      _window->setSwapchainOutOfDate(false);
    }

    // acquire the next image
    auto swapchain_image = _swapchain.acquire_next_image();
    if (swapchain_image.is_empty()) {
      continue;
    }

    // We update the image id of the task swapchain image.
    _taskSwapchainImage.set_images(daxa::TrackedImages{.images = std::span{&swapchain_image, 1}});

    _renderTaskGraph.execute({});
    _device.collect_garbage();
  }

  _device.wait_idle();
  _device.collect_garbage();
}
