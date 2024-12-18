#pragma once

#include <simplege/pch/precomp.h>

#include <simplege/systems/system.h>

namespace SimpleGE
{
  struct AudioSystemImpl;

  class AudioSystem : public ISystem
  {
  public:
    static AudioSystem& Instance()
    {
      static AudioSystem instance;
      return instance;
    }

    AudioSystem(const AudioSystem&) = delete;
    AudioSystem(AudioSystem&&) = delete;
    AudioSystem& operator=(const AudioSystem&) = delete;
    AudioSystem& operator=(AudioSystem&&) = delete;
    ~AudioSystem() override;

    void Iterate(const Timing& timing) override;

    static void LoadAudioBank(std::string_view name) { Instance().LoadAudioBankImpl(name); }

    static void SendEvent(std::string_view eventName) { Instance().SendEventImpl(eventName); }

    static void SetProperty(std::string_view name, float value) { Instance().SetPropertyImpl(name, value); }

  private:
    AudioSystem();

    void LoadAudioBankImpl(std::string_view name);
    void SendEventImpl(std::string_view eventName);
    void SetPropertyImpl(std::string_view name, float value);

    std::unique_ptr<AudioSystemImpl> impl;
  };
} // namespace SimpleGE