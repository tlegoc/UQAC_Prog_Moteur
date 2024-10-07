#pragma once

#include <simplege/pch/precomp.h>

#include <simplege/components/camera.h>
#include <simplege/components/collider.h>
#include <simplege/components/collision.h>
#include <simplege/components/compositor.h>
#include <simplege/components/debugdrawcalls.h>
#include <simplege/components/deformationcompositor.h>
#include <simplege/components/enabler.h>
#include <simplege/components/keyboardinput.h>
#include <simplege/components/layer.h>
#include <simplege/components/logic.h>
#include <simplege/components/position.h>
#include <simplege/components/rawsprite.h>
#include <simplege/components/rendercompositor.h>
#include <simplege/components/sprite.h>
#include <simplege/components/spritesheet.h>
#include <simplege/components/textsprite.h>
#include <simplege/components/texture.h>
#include <simplege/components/visual.h>

namespace SimpleGE
{
  static inline void RegisterGenericComponents()
  {
    Component::Register<CameraComponent>();
    Component::Register<ColliderComponent>();
    Component::Register<DebugDrawCallsComponent>();
    Component::Register<DeformationCompositorComponent>();
    Component::Register<EnablerComponent>();
    Component::Register<KeyboardInputComponent>();
    Component::Register<LayerComponent>();
    Component::Register<PositionComponent>();
    Component::Register<RawSpriteComponent>();
    Component::Register<RenderCompositorComponent>();
    Component::Register<SpriteComponent>();
    Component::Register<SpriteSheetComponent>();
    Component::Register<TextSpriteComponent>();
  }
} // namespace SimpleGE