#pragma once

#include <simplege/pch/precomp.h>

#include <simplege/components/component.h>

namespace SimpleGE
{
  namespace Graphic
  {
    template<typename T>
    class MaterialBuilder;

    class Material;
  } // namespace Graphic

  class CameraComponent;

  class CompositorComponent : public virtual Component
  {
  public:
    struct Description
    {
      std::string shaderProgram;
    };

    CompositorComponent(Entity& owner) : Component(owner) {}

    template<typename CompositorType>
    inline SetupResult Setup(const Description& desc);

    [[nodiscard]] virtual GFXAPI::Framebuffer* Compose(const CameraComponent& camera, GFXAPI::Framebuffer& input)
    {
      return &input;
    }

  protected:
    std::unique_ptr<GFXAPI::Sampler> textureSampler;
    std::unique_ptr<GFXAPI::Pipeline> pipeline;
    std::unique_ptr<Graphic::Material> material;
  };
} // namespace SimpleGE
