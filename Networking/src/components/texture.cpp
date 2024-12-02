#include <simplege/pch/precomp.h>

#include <simplege/simplege.h>

#include "texture.h"

using json = nlohmann::json;

namespace SimpleGE
{
  Component::SetupResult TextureComponent::Setup(const nlohmann::json& descr)
  {
    return Setup(descr.get<Description>());
  }

  Component::SetupResult TextureComponent::Setup(const Description& descr)
  {
    image = Resources::Get<Image>(descr.texture);
    Ensures(image != nullptr);

    textureImage = GFXAPI::Texture::Create(
        {
            .size = image->Size(),
        },
        image->Pixels());

    textureSampler = GFXAPI::Sampler::Create({});
    uniformBuffer = ShaderProgram::CreateUniformBuffer();

    return {this,
            {{[]() { return CameraComponent::Current() != nullptr && CameraComponent::Current()->Ready(); },
              [this, descr]() { SetupImpl(descr); }}}};
  }

  void TextureComponent::SetupImpl(const Description& descr)
  {
    const auto* currentCamera = CameraComponent::Current();
    Ensures(currentCamera != nullptr);

    const auto& cameraRTT = currentCamera->GetRTTFramebuffer();

    const auto& shaderProgram = ShaderProgram::GetShaderProgram();
    auto shaders = shaderProgram.GetShaders();

    pipeline = GFXAPI::Pipeline::Create({
        .framebuffer = cameraRTT,
        .vertexInputInfo = shaderProgram.GetInputInfo(),
        .descriptorSetLayout = shaderProgram.GetDescriptorSetLayout(),
        .shaders = shaders,
        .blend = true,
    });

    Graphic::MaterialBuilder<ShaderProgram> materialBuilder{shaderProgram};
    materialBuilder.Attach(ShaderProgram::Binding::MVP, *uniformBuffer);
    materialBuilder.Attach(ShaderProgram::Binding::Texture, *textureSampler, textureImage.get());
    material = Graphic::Material::Create(materialBuilder);
  }
} // namespace SimpleGE