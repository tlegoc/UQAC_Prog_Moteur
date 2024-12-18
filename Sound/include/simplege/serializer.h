#pragma once

#include <simplege/pch/precomp.h>

namespace SimpleGE
{
  class ISerializer
  {
  public:
    virtual ~ISerializer() = default;

    template<typename T, typename std::enable_if<std::is_trivially_copyable_v<T>, bool>::type = true>
    void Write(const T& val)
    {
      WriteBytes({std::bit_cast<const std::byte*>(&val), sizeof(T)});
    }

    void Write(std::string_view val)
    {
      Write(std::size(val));
      WriteBytes({std::bit_cast<const std::byte*>(val.data()), std::size(val)});
    }

    virtual void WriteBytes(std::span<const std::byte> val) = 0;
  };

  class IDeserializer
  {
  public:
    template<typename T, typename std::enable_if<std::is_trivially_copyable_v<T>, bool>::type = true>
    void Peek(T& val)
    {
      PeekBytes({std::bit_cast<std::byte*>(&val), sizeof(T)});
    }

    virtual void PeekBytes(std::span<std::byte> val) = 0;

    template<typename T, typename std::enable_if<std::is_trivially_copyable_v<T>, bool>::type = true>
    void Read(T& val)
    {
      ReadBytes({std::bit_cast<std::byte*>(&val), sizeof(T)});
    }

    void Read(std::string& val)
    {
      std::string_view::size_type size{};
      Read(size);
      val.resize(size);
      ReadBytes({std::bit_cast<std::byte*>(val.data()), size});
    }

    virtual void ReadBytes(std::span<std::byte> val) = 0;
  };

  class BinarySerializer : public ISerializer
  {
  public:
    BinarySerializer(std::vector<std::byte>& buffer) : buffer(buffer) {}

    void WriteBytes(std::span<const std::byte> val) override
    {
      std::copy(val.begin(), val.end(), std::back_inserter(buffer));
    }

  private:
    std::vector<std::byte>& buffer;
  };

  class BinaryDeserializer : public IDeserializer
  {
  public:
    BinaryDeserializer(std::span<std::byte> buffer) : buffer(buffer) {}

    void PeekBytes(std::span<std::byte> val) override
    {
      auto begin = buffer.begin() + offset;
      auto end = begin + (std::ptrdiff_t) val.size_bytes();
      std::copy(begin, end, val.begin());
    }

    void ReadBytes(std::span<std::byte> val) override
    {
      PeekBytes(val);
      offset += (std::ptrdiff_t) val.size_bytes();
    }

  private:
    std::span<std::byte> buffer;
    std::ptrdiff_t offset{};
  };
} // namespace SimpleGE