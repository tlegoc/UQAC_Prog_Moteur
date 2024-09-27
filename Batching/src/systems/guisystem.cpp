#include <simplege/pch/precomp.h>

#include <simplege/simplege.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

namespace SimpleGE
{
  namespace
  {
    ImGuiContext* context{};

    void OnCameraEnd(const CameraComponent& camera);

    void Init()
    {
      Expects(context == nullptr);

      IMGUI_CHECKVERSION();
      context = ImGui::CreateContext();

      auto& displaySystem = DisplaySystem::Instance();
      auto& gfxDevice = GFXAPI::Device::GetMain();
      auto& window = displaySystem.GetWindow();
      const auto& adapter = gfxDevice.GetAdapter();
      auto queueFamilyIndices = adapter.FindQueueFamilies(window);

      auto viewSize = window.GetSize();

      auto& io = ImGui::GetIO();
      io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
      io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
      io.DisplaySize = {static_cast<float>(viewSize.width), static_cast<float>(viewSize.height)};

      ImGui_ImplVulkan_InitInfo initInfo{
          .Instance = displaySystem.GetGFXInstance().GetNative(),
          .PhysicalDevice = adapter.GetNative(),
          .Device = gfxDevice.GetNative(),
          .QueueFamily = queueFamilyIndices.GetGraphicsFamily(),
          .Queue = gfxDevice.GetGraphicsQueue().GetNative(),
          .PipelineCache = {},
          .DescriptorPool = gfxDevice.GetDescriptorPool().GetNative(),
          .Subpass = 0,
          .MinImageCount = 2,
          .ImageCount = 2,
      };

      CameraComponent* camera = CameraComponent::Current();
      Expects(camera != nullptr);
      ImGui_ImplGlfw_InitForVulkan(&window.GetNativeWindow(), true);
      ImGui_ImplVulkan_Init(&initInfo, camera->GetRTTFramebuffer().GetNativeRenderPass());

      displaySystem.GetCameraEndEvent().Register(OnCameraEnd);
    }

    void Shutdown()
    {
      if (context == nullptr)
      {
        return;
      }

      ImGui_ImplVulkan_Shutdown();
      ImGui_ImplGlfw_Shutdown();
      ImGui::DestroyContext(context);
      context = nullptr;
    }

    void OnFrameBegin(unsigned int frame)
    {
      if (context == nullptr)
      {
        if (CameraComponent::Current() == nullptr || !CameraComponent::Current()->Ready())
        {
          return;
        }
        Init();
      }

      ImGui_ImplVulkan_NewFrame();
      ImGui_ImplGlfw_NewFrame();
      ImGui::NewFrame();
    }

    void OnCameraEnd(const CameraComponent& camera)
    {
      ImGui::Render();
      ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), camera.GetCurrentCommandBuffer().GetNative());
    }
  } // namespace

  GUISystem::GUISystem() { Game::GetFrameBeginEvent().Register(OnFrameBegin); }

  GUISystem::~GUISystem() { Shutdown(); }

  bool GUISystem::IsInitialized() { return context != nullptr; }
} // namespace SimpleGE