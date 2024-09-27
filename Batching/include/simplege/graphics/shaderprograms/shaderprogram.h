#pragma once

#include <simplege/pch/precomp.h>

namespace SimpleGE::Graphic
{
  template<typename T>
  struct ShaderProgramTrait
  {
  };

  template<typename T>
  class ShaderProgram;

  class BaseShaderProgram
  {
  public:
    template<typename T>
    static void Register()
    {
      Register(T::Name, std::make_unique<T>());
    }

    template<typename T>
    [[nodiscard]] static const T& GetShaderProgram()
    {
      return dynamic_cast<const T&>(GetShaderProgram(T::Name));
    }

    static void Shutdown() { shaderProgramRegistry.clear(); }
    [[nodiscard]] static const BaseShaderProgram& GetShaderProgram(std::string_view name);

    virtual ~BaseShaderProgram() = default;

    [[nodiscard]] const GFXAPI::VertexInputInfo& GetInputInfo() const { return vertexInputInfo; }
    [[nodiscard]] const GFXAPI::UniformInfo& GetUniformInfo() const { return uniformInfo; }
    [[nodiscard]] const GFXAPI::DescriptorSetLayout& GetDescriptorSetLayout() const { return *descriptorSetLayout; }

    [[nodiscard]] std::vector<gsl::not_null<const GFXAPI::Shader*>> GetShaders() const
    {
      std::vector<gsl::not_null<const GFXAPI::Shader*>> shaderPtrs;
      shaderPtrs.reserve(std::size(shaders));
      std::transform(shaders.begin(), shaders.end(), std::back_inserter(shaderPtrs),
                     [](const auto& shader) { return &shader; });
      return shaderPtrs;
    }

  protected:
    struct Config
    {
      std::string_view vertexShader;
      std::string_view fragmentShader;
      GFXAPI::VertexInputInfo vertexInputInfo;
      std::size_t bindingCount;
    };

    explicit BaseShaderProgram(const Config& config);

    GFXAPI::VertexInputInfo vertexInputInfo;
    GFXAPI::UniformInfo uniformInfo;
    std::vector<GFXAPI::Shader> shaders;
    std::size_t bindingCount;

  private:
    static void Register(std::string_view name, std::unique_ptr<BaseShaderProgram> shaderProgram);

    void Init();

    static inline std::unordered_map<std::string, std::unique_ptr<BaseShaderProgram>> shaderProgramRegistry;

    std::unique_ptr<GFXAPI::DescriptorSetLayout> descriptorSetLayout;
  };

  template<typename T>
  class ShaderProgram : public BaseShaderProgram
  {
  private:
    using Trait = ShaderProgramTrait<T>;

  public:
    static constexpr auto Name = Trait::Name;
    static constexpr auto VertexShader = Trait::VertexShader;
    static constexpr auto FragmentShader = Trait::FragmentShader;

    using Binding = Trait::Binding;

    using Index = Trait::Index;
    using Vertex = Trait::Vertex;
    using UniformBufferObject = Trait::UniformBufferObject;

    using IndexBufferType = GFXAPI::TypedBuffer<Index>;
    using VertexBufferType = GFXAPI::TypedBuffer<Vertex>;
    using UniformBufferType = GFXAPI::UniformBuffer<UniformBufferObject>;

    [[nodiscard]] static const T& GetShaderProgram() { return BaseShaderProgram::GetShaderProgram<T>(); }

    [[nodiscard]] static std::unique_ptr<IndexBufferType> CreateIndexBuffer(std::size_t size)
    {
      return IndexBufferType::Create({
          .count = size,
          .usage = vk::BufferUsageFlagBits::eIndexBuffer,
          .properties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
      });
    }

    [[nodiscard]] static std::unique_ptr<VertexBufferType> CreateVertexBuffer(std::size_t size)
    {
      return VertexBufferType::Create({
          .count = size,
          .usage = vk::BufferUsageFlagBits::eVertexBuffer,
          .properties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
      });
    }

    [[nodiscard]] static std::unique_ptr<UniformBufferType> CreateUniformBuffer()
    {
      return UniformBufferType::Create({
          .usage = vk::BufferUsageFlagBits::eUniformBuffer,
          .properties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
      });
    }

    void AddUniformBinding(Binding bindingId, GFXAPI::ShaderType shaderType)
    {
      uniformInfo.AddBinding(
          GFXAPI::UniformBinding{.bindingId = static_cast<std::uint32_t>(bindingId), .shader = shaderType});
    }

    void AddTextureBinding(Binding bindingId, GFXAPI::ShaderType shaderType)
    {
      uniformInfo.AddBinding(
          GFXAPI::SamplerBinding{.bindingId = static_cast<std::uint32_t>(bindingId), .shader = shaderType});
    }

  protected:
    ShaderProgram()
        : BaseShaderProgram({
              .vertexShader = VertexShader,
              .fragmentShader = FragmentShader,
              .vertexInputInfo = Trait::GetVertexInputInfo(),
              .bindingCount = static_cast<std::size_t>(Trait::Binding::Count),
          })
    {
    }
  };
} // namespace SimpleGE::Graphic