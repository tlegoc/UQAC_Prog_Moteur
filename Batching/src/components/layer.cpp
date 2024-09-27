#include <simplege/pch/precomp.h>

#include <simplege/simplege.h>

namespace SimpleGE
{
  static std::vector<gsl::not_null<SpriteComponent*>> ListSprites(const EntityRef& owner)
  {
    std::queue<EntityRef> queue;
    queue.push(owner);

    std::vector<gsl::not_null<SpriteComponent*>> sprites;

    while (!queue.empty())
    {
      auto node = queue.front();
      queue.pop();

      for (const auto& child : node->GetChildren())
      {
        if (child->Active())
        {
          queue.push(child);
        }

        auto* comp = child->GetComponent<SpriteComponent>();
        if (comp != nullptr)
        {
          sprites.emplace_back(comp);
        }
      }
    }

    return sprites;
  }

  void LayerComponent::Display(const CameraComponent& camera, const Timing& timing)
  {
    auto layerSprites = ListSprites(Owner().shared_from_this());
    if (layerSprites.empty())
    {
      return;
    }

    // The sprite sheet we will render with
    ComponentReference<SpriteSheetComponent> spriteSheet;

    // Each frame we rebuild the batch
    // HORRIBLY inefficient, but simple and also not too costly because 2D rendering
    // To be totally honest I hate this but a better way would take too much time.
    int indexCount = 0;
    int vertexCount = 0;

    for (int i = 0; i < layerSprites.size(); i++)
    {
      auto spriteComponent = gsl::at(layerSprites, i);
      // newSpriteSheet because I wanted to make it work even if the sprite sheet changes
      // but we can't really compare them for now. The function is "still" here but the comparison is ignored.
      auto newSpriteSheet = spriteComponent->GetSpriteSheet();

      // C++ data structures, NOT ON GPU
      auto indices = spriteComponent->GetIndices();
      auto vertices = spriteComponent->GetVertices();

      // Draw if batch is full or if spritesheet is changing (commented out)
      if (vertexCount + std::size(vertices) > LAYER_BATCH_SIZE ||
          indexCount + std::size(indices) > LAYER_BATCH_SIZE) // | newSpriteSheet != spriteSheet)
      {
        Ensures(spriteSheet.Ready());

        auto& ubo = spriteSheet->GetUniformBuffer();
        ubo.model = glm::identity<glm::mat4>();
        ubo.view = glm::identity<glm::mat4>();
        ubo.proj = camera.Projection();

        spriteSheet->Draw(camera, *batchVertexBuffer, *batchIndexBuffer, indexCount);

        // Reset batch
        indexCount = 0;
        vertexCount = 0;
      }

      spriteSheet = newSpriteSheet;

      // Copy indices into batch index buffer
      {
        // Offset indices
        for (auto& index : indices)
        {
          index += vertexCount;
        }

        auto mapping = batchIndexBuffer->Map({indexCount * static_cast<ptrdiff_t>(sizeof(SpriteSheetComponent::ShaderProgram::Index)), std::size(indices)});
        std::copy(indices.begin(), indices.end(), mapping.Get().begin());
      }

      // Do the same for vertices
      {
        auto mapping = batchVertexBuffer->Map({vertexCount * static_cast<ptrdiff_t>(sizeof(SpriteSheetComponent::ShaderProgram::Vertex)), std::size(vertices)});

        std::copy(vertices.begin(), vertices.end(), mapping.Get().begin());
      }

      vertexCount += std::size(vertices);
      indexCount += std::size(indices);
    }

    // Draw if not emtpy (already drawn)
    if (indexCount > 0 && vertexCount > 0)
    {
      Ensures(spriteSheet.Ready());

      auto& ubo = spriteSheet->GetUniformBuffer();
      ubo.model = glm::identity<glm::mat4>();
      ubo.view = glm::identity<glm::mat4>();
      ubo.proj = camera.Projection();

      spriteSheet->Draw(camera, *batchVertexBuffer, *batchIndexBuffer, indexCount);
    }
  }
} // namespace SimpleGE