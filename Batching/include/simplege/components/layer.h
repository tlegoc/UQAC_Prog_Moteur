#pragma once

#include <simplege/pch/precomp.h>

#include <simplege/components/visual.h>

#include <simplege/components/spritesheet.h>

#define LAYER_BATCH_SIZE 1024

namespace SimpleGE
{
  class LayerComponent : public VisualComponent
  {
  public:
    static constexpr auto Type = "Layer";

    LayerComponent(Entity& owner) : Component(owner)
    {
      batchVertexBuffer = SpriteSheetComponent::ShaderProgram::CreateVertexBuffer(LAYER_BATCH_SIZE);
      batchIndexBuffer = SpriteSheetComponent::ShaderProgram::CreateIndexBuffer(LAYER_BATCH_SIZE);
    }

    void Display(const CameraComponent& camera, const Timing& timing) override;

  private:
    std::unique_ptr<SpriteSheetComponent::ShaderProgram::VertexBufferType> batchVertexBuffer;
    std::unique_ptr<SpriteSheetComponent::ShaderProgram::IndexBufferType> batchIndexBuffer;
  };

} // namespace SimpleGE
