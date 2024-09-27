#include <simplege/pch/precomp.h>

#include <simplege/simplege.h>

#include "texture.h"

using json = nlohmann::json;

namespace SimpleGE
{
  static void from_json(const json& j, SpriteSheetComponent::Description& desc)
  {
    from_json(j, static_cast<TextureComponent::Description&>(desc));

    j.at("description").get_to(desc.description);
  }

  static void from_json(const json& j, SpriteSheetComponent::Frame& frame)
  {
    j.at("frame").get_to(frame.frame);
    j.at("sourceSize").get_to(frame.sourceSize);
  }

  static void from_json(const json& j, SpriteSheetComponent::Meta& meta) { j.at("size").get_to(meta.size); }

  static void from_json(const json& j, SpriteSheetComponent& comp)
  {
    j.at("frames").get_to(comp.sprites);
    j.at("meta").get_to(comp.meta);

    const float invScaleH = 1.F / float(comp.meta.size.width);
    const float invScaleV = 1.F / float(comp.meta.size.height);

    for (auto& f : comp.sprites)
    {
      const auto& frame = f.second.frame;
      auto& uv = f.second.uv;
      uv.position[0] = frame.x() * invScaleH;
      uv.position[1] = frame.y() * invScaleV;
      uv.size.width = frame.width() * invScaleH;
      uv.size.height = frame.height() * invScaleV;
    }
  }

  Component::SetupResult SpriteSheetComponent::Setup(const nlohmann::json& descr)
  {
    return Setup(descr.get<Description>());
  }

  Component::SetupResult SpriteSheetComponent::Setup(const Description& descr)
  {
    // On charge l'image et les shaders
    auto status = TextureComponent::Setup(descr);
    status.Append({{.execute = [this, descr]() { SetupImpl(descr); }}});
    return status;
  }

  void SpriteSheetComponent::SetupImpl(const Description& descr)
  {
    // On charge ensuite le fichier de description de l'image,
    // qui contient l'emplacement et les dimensions des sprites
    // contenues sur la feuille.
    auto content = Resources::Get<TextAsset>(descr.description);
    Ensures(content != nullptr);
    json::parse(content->Value()).get_to(*this);
  }

  const SpriteSheetComponent::Frame* SpriteSheetComponent::GetSprite(std::string_view key) const
  {
    const auto item = sprites.find(std::string{key});
    if (item != sprites.end())
    {
      return &item->second;
    }
    return nullptr;
  }

  void SpriteSheetComponent::Draw(const CameraComponent& camera, const ShaderProgram::VertexBufferType& vertexBuffer,
                                  const ShaderProgram::IndexBufferType& indexBuffer, std::size_t indexCount)
  {
    auto& commandBuffer = camera.GetCurrentCommandBuffer();

    material->Draw<ShaderProgram::Index>({
        .commandBuffer = commandBuffer,
        .pipeline = *pipeline,
        .vertexBuffer = vertexBuffer,
        .indexBuffer = indexBuffer,
        .indexCount = indexCount,
    });
  }

} // namespace SimpleGE