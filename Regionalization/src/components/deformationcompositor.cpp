#include <simplege/pch/precomp.h>

#include <simplege/simplege.h>

#include "compositor.h"

using json = nlohmann::json;

namespace SimpleGE
{
  static void from_json(const json& j, DeformationCompositorComponent::Description& desc)
  {
    from_json(j, static_cast<CompositorComponent::Description&>(desc));

    j.at("speed").get_to(desc.speed);
    j.at("scale").get_to(desc.scale);
    j.at("source").get_to(desc.source);
    j.at("intensity").get_to(desc.intensity);
  }

  void DeformationCompositorComponent::OnEnabled() { start = std::chrono::steady_clock::now(); }

  Component::SetupResult DeformationCompositorComponent::Setup(const nlohmann::json& descr)
  {
    return Setup(descr.get<Description>());
  }

  Component::SetupResult DeformationCompositorComponent::Setup(const Description& descr)
  {
    return {this, {{[]() { return CameraComponent::Current() != nullptr; }, [this, descr]() { SetupImpl(descr); }}}};
  }

  void DeformationCompositorComponent::SetupImpl(const Description& descr)
  {
    const auto* currentCamera = CameraComponent::Current();
    Ensures(currentCamera != nullptr);

    const auto viewSize = currentCamera->ViewSize();

    speed = descr.speed;
    scale = descr.scale;
    start = std::chrono::steady_clock::now();

    auto status = CompositorComponent::Setup<DeformationCompositorComponent>(descr);
    Ensures(status.IsDone());

    auto deformationImage = Resources::Get<Image>(descr.source);
    auto intensityImage = Resources::Get<Image>(descr.intensity);

    Ensures(deformationImage != nullptr);
    Ensures(intensityImage != nullptr);

    deformationTexture = GFXAPI::Texture::Create(
        {
            .size = deformationImage->Size(),
        },
        deformationImage->Pixels());
    intensityTexture = GFXAPI::Texture::Create(
        {
            .size = intensityImage->Size(),
        },
        intensityImage->Pixels());

    settingsBuffer = ShaderProgram::CreateUniformBuffer();

    textureSampler = GFXAPI::Sampler::Create({});
    deformationSampler = GFXAPI::Sampler::Create({
        .clamp = false,
    });
    intensitySampler = GFXAPI::Sampler::Create({});

    colorTarget = GFXAPI::Texture::Create({
        .size = viewSize,
        .usage = vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled,
    });
    auto colorTargets = std::array{gsl::make_not_null(colorTarget.get())};
    rttFramebuffer = GFXAPI::Framebuffer::Create({
        .colorAttachments = colorTargets,
    });

    const auto& shaderProgram = ShaderProgram::GetShaderProgram();
    auto shaders = shaderProgram.GetShaders();

    pipeline = GFXAPI::Pipeline::Create({
        .framebuffer = *rttFramebuffer,
        .vertexInputInfo = shaderProgram.GetInputInfo(),
        .descriptorSetLayout = shaderProgram.GetDescriptorSetLayout(),
        .shaders = shaders,
        .blend = false,
    });

    Graphic::MaterialBuilder<ShaderProgram> materialBuilder{shaderProgram};
    materialBuilder.Attach(ShaderProgram::Binding::Texture, *textureSampler);
    materialBuilder.Attach(ShaderProgram::Binding::Deformation, *deformationSampler, deformationTexture.get());
    materialBuilder.Attach(ShaderProgram::Binding::Intensity, *intensitySampler, intensityTexture.get());
    materialBuilder.Attach(ShaderProgram::Binding::Settings, *settingsBuffer);
    material = Graphic::Material::Create(materialBuilder);
  }

  GFXAPI::Framebuffer* DeformationCompositorComponent::Compose(const CameraComponent& camera,
                                                               GFXAPI::Framebuffer& input)
  {
    auto& commandBuffer = camera.GetCurrentCommandBuffer();

    const auto elapsed = std::chrono::duration<float>(std::chrono::steady_clock::now() - start).count() * speed;

    ShaderProgram::UniformBufferObject& settings = *settingsBuffer;
    settings.time = elapsed;
    settings.scale = scale;

    Graphic::MaterialBuilder<ShaderProgram> materialBuilder;
    materialBuilder.Attach(ShaderProgram::Binding::Texture, *textureSampler, input.GetColorAttachments().front());
    material->Update(materialBuilder);

    commandBuffer.BeginRenderPass({
        .target = *rttFramebuffer,
    });

    material->Draw({
        .commandBuffer = commandBuffer,
        .pipeline = *pipeline,
        .vertexCount = 6,
        .instanceCount = 2,
    });

    commandBuffer.EndRenderPass();

    if (elapsed >= 1.F)
    {
      Disable();
    }

    return rttFramebuffer.get();
  }

  void DeformationCompositorComponent::FillMaterialBuilder(Graphic::MaterialBuilder<ShaderProgram>& materialBuilder) {}

} // namespace SimpleGE