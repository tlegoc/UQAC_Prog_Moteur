#pragma once

#include "chickendodge/pch/precomp.h"

#include "chickendodge/components/backgroundloader.h"
#include "chickendodge/components/chicken.h"
#include "chickendodge/components/chickenspawner.h"
#include "chickendodge/components/countdown.h"
#include "chickendodge/components/heart.h"
#include "chickendodge/components/life.h"
#include "chickendodge/components/player.h"
#include "chickendodge/components/referee.h"
#include "chickendodge/components/rupee.h"
#include "chickendodge/components/score.h"
#include "chickendodge/components/timer.h"

namespace ChickenDodge
{
  static inline void RegisterGameComponents()
  {
    using Component = SimpleGE::Component;

    Component::Register<BackgroundLoaderComponent>();
    Component::Register<ChickenComponent>();
    Component::Register<ChickenSpawnerComponent>();
    Component::Register<CountdownComponent>();
    Component::Register<HeartComponent>();
    Component::Register<LifeComponent>();
    Component::Register<PlayerComponent>();
    Component::Register<RefereeComponent>();
    Component::Register<RupeeComponent>();
    Component::Register<ScoreComponent>();
    Component::Register<TimerComponent>();
  }
} // namespace ChickenDodge