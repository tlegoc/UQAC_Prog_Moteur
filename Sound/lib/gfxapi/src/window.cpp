#include <gfxapi/pch/precomp.h>

#include <gfxapi/gfxapi.h>

namespace SimpleGE::GFXAPI
{
  Window::Window(const WindowConfig& config) : instance(config.instance)
  {
    window = glfwCreateWindow(config.size.width, config.size.height, config.title.data(), nullptr, nullptr);
    if (window == nullptr)
    {
      throw std::runtime_error("Impossible de créer une fenêtre valide.");
    }

    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, FramebufferResizeCallback);

    VkSurfaceKHR raw_surface = VK_NULL_HANDLE;
    VkResult err = glfwCreateWindowSurface(instance.GetNative(), window, nullptr, &raw_surface);
    surface = raw_surface;
    if (err != VK_SUCCESS)
    {
      const char* error_msg{};
      int ret = glfwGetError(&error_msg);
      if (ret != 0)
      {
        throw std::runtime_error(fmt::format("Échec à la création de la surface ({}). Erreur: {}\n", ret, error_msg));
      }
      surface = nullptr;
    }
  }

  Window::Window(Window&& other) noexcept : instance(other.instance), window(other.window), surface(other.surface)
  {
    other.window = nullptr;
    other.surface = nullptr;
  }

  Window::~Window()
  {
    instance.GetNative().destroySurfaceKHR(surface);
    glfwDestroyWindow(window);
  }

  Window& Window::operator=(Window&& other) noexcept
  {
    Expects(&instance == &other.instance);
    window = other.window;
    surface = other.surface;
    other.window = nullptr;
    other.surface = nullptr;
    return *this;
  }

  bool Window::IsClosed() const { return glfwWindowShouldClose(window) == GLFW_TRUE; }

  bool Window::IsKeyDown(KeyCode key) const { return glfwGetKey(window, static_cast<int>(key)) == GLFW_PRESS; }

  void Window::Present(SwapChain& swapChain) { glfwPollEvents(); }

  void Window::WaitEvents() const { glfwWaitEvents(); }

  Size<std::uint32_t> Window::GetSize() const
  {
    int width{};
    int height{};
    glfwGetFramebufferSize(window, &width, &height);

    return Size<std::uint32_t>{
        .width = static_cast<std::uint32_t>(width),
        .height = static_cast<std::uint32_t>(height),
    };
  }

  void Window::FramebufferResizeCallback(GLFWwindow* window, int width, int height)
  {
    auto* self = std::bit_cast<Window*>(glfwGetWindowUserPointer(window));
    self->framebufferResized = true;
  }
} // namespace SimpleGE::GFXAPI