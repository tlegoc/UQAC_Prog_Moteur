#include <gfxapi/pch/precomp.h>

#include <gfxapi/gfxapi.h>

namespace SimpleGE::GFXAPI
{
  class Subsystem
  {
  public:
    static Subsystem& Instance()
    {
      static Subsystem instance;
      return instance;
    }

    Subsystem()
    {
      if (glfwInit() != GLFW_TRUE)
      {
        throw std::runtime_error("Échec à l'initialisation de GLFW");
      }

      glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    }

    Subsystem(const Subsystem&) = delete;
    Subsystem(Subsystem&&) = delete;
    Subsystem& operator=(const Subsystem&) = delete;
    Subsystem& operator=(Subsystem&&) = delete;

    ~Subsystem() { glfwTerminate(); }

    static void Init()
    {
      // Nothing, handled by constructor
      std::ignore = Instance();
    }

    [[nodiscard]] static bool CheckValidationLayerSupport()
    {
      auto availableLayers = vk::enumerateInstanceLayerProperties();
      for (const auto* layerName : ValidationLayers)
      {
        if (!std::any_of(availableLayers.begin(), availableLayers.end(),
                         [layerName](const auto& l) { return std::strcmp(layerName, l.layerName); }))
        {
          return false;
        }
      }

      return true;
    }
  };

  GFXInstance::GFXInstance(const GFXInstanceConfig& config) : enableValidation(config.enableValidation)
  {
    Subsystem::Init();

    if (enableValidation && !Subsystem::CheckValidationLayerSupport())
    {
      throw std::runtime_error("Vulkan validation layers not found");
    }

    vk::ApplicationInfo appInfo{
        .pApplicationName = config.title.data(),
        .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
        .pEngineName = "SimpleGE",
        .engineVersion = VK_MAKE_VERSION(1, 0, 0),
        .apiVersion = VK_API_VERSION_1_1,
    };

    std::uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    auto glfwExtensionsSpan = std::span{glfwExtensions, glfwExtensionCount};

    std::vector<const char*> requiredExtensions;
    requiredExtensions.insert(requiredExtensions.end(), glfwExtensionsSpan.begin(), glfwExtensionsSpan.end());

    for (const auto& ext : vk::enumerateInstanceExtensionProperties())
    {
      if (std::strcmp(ext.extensionName, VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME) == 0)
      {
        requiredExtensions.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
      }
    }

    vk::InstanceCreateInfo createInfo{
        .pApplicationInfo = &appInfo,
        .enabledExtensionCount = static_cast<uint32_t>(std::size(requiredExtensions)),
        .ppEnabledExtensionNames = requiredExtensions.data(),
    };

    createInfo.flags |= vk::InstanceCreateFlagBits::eEnumeratePortabilityKHR;

    if (enableValidation)
    {
      createInfo.enabledLayerCount = std::size(ValidationLayers);
      createInfo.ppEnabledLayerNames = ValidationLayers.data();
    }
    else
    {
      createInfo.enabledLayerCount = 0;
    }

    vkInstance = vk::createInstance(createInfo, nullptr);
  }

  GFXInstance::GFXInstance(GFXInstance&& other) noexcept
      : vkInstance(other.vkInstance), enableValidation(other.enableValidation)
  {
    other.vkInstance = nullptr;
  }

  GFXInstance::~GFXInstance() { vkInstance.destroy(); }

  GFXInstance& GFXInstance::operator=(GFXInstance&& other) noexcept
  {
    this->~GFXInstance();
    vkInstance = other.vkInstance;
    enableValidation = other.enableValidation;
    other.vkInstance = nullptr;
    return *this;
  }

  std::vector<AdapterInfos> GFXInstance::ListAdapters(const Window& window) const
  {
    std::vector<AdapterInfos> adapterInfos;

    auto physicalDevices = vkInstance.enumeratePhysicalDevices();
    std::multimap<int, vk::PhysicalDevice, std::greater<>> candidates;

    std::transform(physicalDevices.begin(), physicalDevices.end(), std::inserter(candidates, candidates.begin()),
                   [&window](const auto& device)
                   { return std::make_pair(Adapter::RateDeviceSuitability(device, window), device); });

    for (auto& entry : candidates)
    {
      auto physicalDevice = entry.second;
      auto properties = physicalDevice.getProperties();

      adapterInfos.emplace_back(AdapterInfos{
          .physicalDevice = physicalDevice,
          .name = properties.deviceName,
          .score = entry.first,
      });
    }

    return adapterInfos;
  }
} // namespace SimpleGE::GFXAPI