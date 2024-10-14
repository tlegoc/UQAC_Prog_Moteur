#pragma once

#include <simplege/pch/precomp.h>

#include <simplege/components/logic.h>
#include <simplege/components/spritesheet.h>
#include <simplege/components/texture.h>
#include <simplege/components/visual.h>

#include <simplege/eventtrigger.h>

namespace SimpleGE
{
  class SpriteSheetComponent;

  class SpriteComponent : public LogicComponent, public VisualComponent
  {
  public:
    static constexpr auto Type = "Sprite";

    using ShaderProgram = TextureComponent::ShaderProgram;
    using AnimationEndedEventType = std::function<void()>;

    struct Description
    {
      int frameSkip{1};
      bool isAnimated{};
      int animWait{};
      std::string spriteName{};
      ComponentReference<SpriteSheetComponent> spriteSheet;
    };

    SpriteComponent(Entity& owner) : Component(owner) {}

    SetupResult Setup(const nlohmann::json& descr) override;
    SetupResult Setup(const Description& descr);

    void UpdateLogic(const Timing& timing) override;
    void Display(const CameraComponent& camera, const Timing& timing) override;

    void SetFrameSkip(int val) { frameSkip = val; }
    void SetAnimationFrame(int val) { animationFrame = val; }
    void SetIsAnimated(bool val) { isAnimated = val; }
    void UpdateMesh();

    void SetSpriteName(std::string_view spriteName);

    void RegisterAnimationEndedEvent(const AnimationEndedEventType& onAnimationEnded)
    {
      animationEndedEvent.Register(onAnimationEnded);
    }

    [[nodiscard]] ComponentReference<SpriteSheetComponent> GetSpriteSheet() const { return spriteSheet; }

    static std::array<ShaderProgram::Index, 6> GetIndices() { return Indices; }

    std::array<ShaderProgram::Vertex, 4> GetVertices() const { return vertices; }

  private:
    void SetupImpl();

    std::string FindNextFrameName();
    void UpdateComponents();

    ComponentReference<SpriteSheetComponent> spriteSheet;
    std::string spriteName;
    EventTrigger<AnimationEndedEventType> animationEndedEvent;
    bool isAnimated{};
    int frameSkip{};
    int animationFrame{};
    int animWait{};
    int animWaitCounter{};
    const SpriteSheetComponent::Frame* descr{};
    Size<int> spriteSize{};
    static constexpr std::array<ShaderProgram::Index, 6> Indices{0, 1, 2, 2, 3, 0};

    std::array<ShaderProgram::Vertex, 4> vertices;
    // std::unique_ptr<ShaderProgram::VertexBufferType> vertexBuffer;
    // std::unique_ptr<ShaderProgram::IndexBufferType> indexBuffer;
  };
} // namespace SimpleGE
