#include <simplege/pch/precomp.h>

#include <simplege/simplege.h>

#include "firstprocessguard.h"

#include <filesystem>

#include <fmod.hpp>
#include <fmod_studio.hpp>

namespace SimpleGE
{
  struct Bank
  {
    Bank(const std::filesystem::path& path, gsl::not_null<FMOD::Studio::System*> system)
    {
      FMOD_RESULT result = system->loadBankFile(path.string().c_str(), FMOD_STUDIO_LOAD_BANK_NORMAL, &bank);
      Expects(result == FMOD_OK);
      Expects(bank != nullptr);

      result = bank->loadSampleData();
      Expects(result == FMOD_OK);
    }

    ~Bank()
    {
      FMOD_RESULT result = bank->unload();
      Expects(result == FMOD_OK);
    }

    FMOD::Studio::Bank* bank{};
  };

  struct AudioSystemImpl
  {
    virtual ~AudioSystemImpl() = default;
    virtual void Update() const = 0;
    virtual void LoadAudioBank(std::string_view name) = 0;
    virtual void SendEvent(std::string_view eventName) const = 0;
    virtual void SetProperty(std::string_view name, float value) const = 0;
  };

  struct FMODAudioSystemImpl : public AudioSystemImpl
  {
    static constexpr auto MaxChannelCount = 1024;

    FMODAudioSystemImpl()
    {
      FMOD_RESULT result = FMOD::Studio::System::create(&system);
      Expects(result == FMOD_OK);
      Expects(system != nullptr);

      result = system->initialize(MaxChannelCount, FMOD_STUDIO_INIT_NORMAL | FMOD_STUDIO_INIT_LIVEUPDATE,
                                  FMOD_INIT_NORMAL, nullptr);
      Expects(result == FMOD_OK);
    }

    ~FMODAudioSystemImpl() override
    {
      soundBank.reset();
      stringsBank.reset();
      FMOD_RESULT result = system->release();
      Expects(result == FMOD_OK);
    }

    void Update() const override
    {
      FMOD_RESULT result = system->update();
      Expects(result == FMOD_OK);
    }

    void LoadAudioBank(std::string_view name) override
    {
      static constexpr auto AudioPath = "audio/Desktop";

      std::filesystem::path audioPath = Resources::DataPath;
      audioPath.append(AudioPath);

      std::filesystem::path soundPath = audioPath;
      soundPath.append(fmt::format("{}.bank", name));
      soundBank = std::make_unique<Bank>(soundPath, system);

      std::filesystem::path stringsPath = audioPath;
      stringsPath.append(fmt::format("{}.strings.bank", name));
      stringsBank = std::make_unique<Bank>(stringsPath, system);
    }

    void SendEvent(std::string_view eventName) const override
    {
      std::string eventPath = fmt::format("event:/{}", eventName);
      FMOD::Studio::EventDescription* eventDescription{};
      FMOD_RESULT result = system->getEvent(eventPath.c_str(), &eventDescription);
      Expects(result == FMOD_OK);
      Expects(eventDescription != nullptr);

      FMOD::Studio::EventInstance* eventInstance{};
      result = eventDescription->createInstance(&eventInstance);
      Expects(result == FMOD_OK);

      result = eventInstance->start();
      Expects(result == FMOD_OK);

      result = eventInstance->release();
      Expects(result == FMOD_OK);
    }

    void SetProperty(std::string_view name, float value) const override
    {
      FMOD_RESULT result = system->setParameterByName(name.data(), value);
      Expects(result == FMOD_OK);
    }

    FMOD::Studio::System* system{};
    std::unique_ptr<Bank> soundBank;
    std::unique_ptr<Bank> stringsBank;
    bool isFirstInstance;
  };

  struct SilentAudioSystemImpl : public AudioSystemImpl
  {
    SilentAudioSystemImpl() { fmt::print("Audio désactivé car une autre instance est déjà en cours.\n"); }

    void Update() const override {}
    void LoadAudioBank(std::string_view name) override {}
    void SendEvent(std::string_view eventName) const override {}
    void SetProperty(std::string_view name, float value) const override {}
  };

  AudioSystem::AudioSystem()
      : impl(FirstProcessGuard::IsFirstProcessInstance()
                 ? std::unique_ptr<AudioSystemImpl>{std::make_unique<FMODAudioSystemImpl>()}
                 : std::unique_ptr<AudioSystemImpl>{std::make_unique<SilentAudioSystemImpl>()})
  {
  }

  AudioSystem::~AudioSystem() = default;

  void AudioSystem::Iterate(const Timing& timing) { impl->Update(); }

  void AudioSystem::LoadAudioBankImpl(std::string_view name) { impl->LoadAudioBank(name); }

  void AudioSystem::SendEventImpl(std::string_view eventName) { impl->SendEvent(eventName); }

  void AudioSystem::SetPropertyImpl(std::string_view name, float value) { impl->SetProperty(name, value); }
} // namespace SimpleGE