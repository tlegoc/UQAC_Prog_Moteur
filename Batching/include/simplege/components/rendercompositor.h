#pragma once

#include <simplege/pch/precomp.h>

#include <simplege/components/compositor.h>

namespace SimpleGE
{
  namespace Graphic
  {
    class RenderQuadShaderProgram;
  }

  class RenderCompositorComponent : public CompositorComponent
  {
  public:
    static constexpr auto Type = "RenderCompositor";

    using Description = CompositorComponent::Description;
    using ShaderProgram = Graphic::RenderQuadShaderProgram;

    RenderCompositorComponent(Entity& owner) : CompositorComponent(owner), Component(owner) {}

    SetupResult Setup(const nlohmann::json& descr) override;
    SetupResult Setup(const Description& descr);

    [[nodiscard]] GFXAPI::Framebuffer* Compose(const CameraComponent& camera, GFXAPI::Framebuffer& input) override;

  protected:
    void FillMaterialBuilder(Graphic::MaterialBuilder<ShaderProgram>& materialBuilder);

    friend class CompositorComponent;
  };
} // namespace SimpleGE
