#include <simplege/pch/precomp.h>

#include <simplege/simplege.h>

#include "compositor.h"

using json = nlohmann::json;

namespace SimpleGE
{
  Component::SetupResult RenderCompositorComponent::Setup(const nlohmann::json& descr)
  {
    return Setup(descr.get<Description>());
  }

  Component::SetupResult RenderCompositorComponent::Setup(const Description& descr)
  {
    auto status = CompositorComponent::Setup<RenderCompositorComponent>(descr);
    Ensures(status.IsDone());

    textureSampler = GFXAPI::Sampler::Create({});

    const auto& shaderProgram = ShaderProgram::GetShaderProgram();
    auto shaders = shaderProgram.GetShaders();

    pipeline = GFXAPI::Pipeline::Create({
        .framebuffer = DisplaySystem::Instance().GetSwapChain().GetFramebuffer(0),
        .vertexInputInfo = shaderProgram.GetInputInfo(),
        .descriptorSetLayout = shaderProgram.GetDescriptorSetLayout(),
        .shaders = shaders,
        .blend = false,
    });

    Graphic::MaterialBuilder<ShaderProgram> materialBuilder{shaderProgram};
    materialBuilder.Attach(ShaderProgram::Binding::Texture, *textureSampler);
    material = Graphic::Material::Create(materialBuilder);

    return {this, {}};
  }

  GFXAPI::Framebuffer* RenderCompositorComponent::Compose(const CameraComponent& camera, GFXAPI::Framebuffer& input)
  {
    auto& commandBuffer = camera.GetCurrentCommandBuffer();

    Graphic::MaterialBuilder<ShaderProgram> materialBuilder;
    materialBuilder.Attach(ShaderProgram::Binding::Texture, *textureSampler, input.GetColorAttachments().front());
    material->Update(materialBuilder);

    commandBuffer.BeginRenderPass({
        .target = camera.GetCurrentFramebuffer(),
    });

    material->Draw({
        .commandBuffer = commandBuffer,
        .pipeline = *pipeline,
        .vertexCount = 6,
        .instanceCount = 2,
    });

    commandBuffer.EndRenderPass();

    return nullptr;
  }

  void RenderCompositorComponent::FillMaterialBuilder(Graphic::MaterialBuilder<ShaderProgram>& materialBuilder) {}
} // namespace SimpleGE