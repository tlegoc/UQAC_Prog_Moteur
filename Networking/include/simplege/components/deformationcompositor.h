#pragma once

#include <simplege/pch/precomp.h>

#include <simplege/components/compositor.h>

#include <simplege/graphics/shaderprograms/deformation.h>

namespace SimpleGE
{
  class DeformationCompositorComponent : public CompositorComponent
  {
  public:
    static constexpr auto Type = "DeformationCompositor";

    using ShaderProgram = Graphic::DeformationShaderProgram;

    struct Description : public CompositorComponent::Description
    {
      float speed;
      float scale;
      std::string source;
      std::string intensity;
    };

    DeformationCompositorComponent(Entity& owner) : CompositorComponent(owner), Component(owner) {}

    SetupResult Setup(const nlohmann::json& descr) override;
    SetupResult Setup(const Description& descr);

    [[nodiscard]] GFXAPI::Framebuffer* Compose(const CameraComponent& camera, GFXAPI::Framebuffer& input) override;

  protected:
    void FillMaterialBuilder(Graphic::MaterialBuilder<ShaderProgram>& materialBuilder);

  private:
    void SetupImpl(const Description& descr);
    void OnEnabled() override;

    float speed{};
    float scale{};
    std::chrono::time_point<std::chrono::steady_clock> start;

    std::unique_ptr<GFXAPI::Texture> deformationTexture;
    std::unique_ptr<GFXAPI::Sampler> deformationSampler;
    std::unique_ptr<GFXAPI::Texture> intensityTexture;
    std::unique_ptr<GFXAPI::Sampler> intensitySampler;
    std::unique_ptr<GFXAPI::Texture> colorTarget;
    std::unique_ptr<ShaderProgram::UniformBufferType> settingsBuffer;
    std::unique_ptr<GFXAPI::Framebuffer> rttFramebuffer;

    friend class CompositorComponent;
  };
} // namespace SimpleGE
