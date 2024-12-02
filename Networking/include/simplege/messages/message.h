#pragma once

#include <simplege/pch/precomp.h>

#include <simplege/serializer.h>

namespace SimpleGE
{
  template<typename T>
  struct Message;

  struct BaseMessage
  {
  public:
    using CreateFnType = std::function<std::unique_ptr<BaseMessage>()>;

    using MessageType = std::uint32_t;
    static constexpr MessageType FirstEngineType = 0;
    static constexpr MessageType FirstGameType = 1000;

    MessageType type;

    template<typename T>
    static void Register()
    {
      Register(T::Type, []() { return std::make_unique<T>(); });
    }

    static std::unique_ptr<BaseMessage> Create(MessageType type)
    {
      Expects(messageRegistry.find(type) != messageRegistry.end());
      const auto& createFn = messageRegistry.at(type);
      return createFn();
    }

    template<typename T>
    [[nodiscard]] bool Is() const
    {
      static_assert(std::is_base_of_v<BaseMessage, T>);
      return type == T::Type;
    }

    template<typename T>
    [[nodiscard]] T& Get()
    {
      Expects(Is<T>());
      return static_cast<T&>(*this);
    }

    template<typename T>
    [[nodiscard]] const T& Get() const
    {
      Expects(Is<T>());
      return static_cast<const T&>(*this);
    }

    virtual void Serialize(ISerializer& serializer) const { serializer.Write(type); }

    virtual void Deserialize(IDeserializer& deserializer) { deserializer.Read(type); }

  protected:
    explicit BaseMessage(MessageType type) : type(type) {}

  private:
    static void Register(MessageType type, const CreateFnType& createFn) { messageRegistry.emplace(type, createFn); }

    static inline std::unordered_map<MessageType, CreateFnType> messageRegistry;
  };

  template<typename T>
  struct Message : public BaseMessage
  {
    Message() : BaseMessage(T::Type) { static_assert(std::is_base_of_v<BaseMessage, T>); }
  };
} // namespace SimpleGE