#include <gfxapi/pch/precomp.h>

#include <gfxapi/gfxapi.h>

namespace SimpleGE::GFXAPI
{
  std::vector<vk::VertexInputBindingDescription> VertexInputInfo::GetNativeBindingDescriptions() const
  {
    std::vector<vk::VertexInputBindingDescription> descr;
    descr.reserve(std::size(bindings));

    std::uint32_t bindingID = 0;
    std::transform(bindings.begin(), bindings.end(), std::back_inserter(descr),
                   [&bindingID](const auto& binding)
                   {
                     return vk::VertexInputBindingDescription{
                         .binding = bindingID++,
                         .stride = binding.stride,
                         .inputRate = vk::VertexInputRate::eVertex,
                     };
                   });

    return descr;
  }

  std::vector<vk::VertexInputAttributeDescription> VertexInputInfo::GetNativeAttributeDescriptions() const
  {
    std::vector<vk::VertexInputAttributeDescription> descr;

    std::uint32_t bindingID = 0;
    for (const auto& binding : bindings)
    {
      std::uint32_t location = 0;
      std::transform(binding.attributes.begin(), binding.attributes.end(), std::back_inserter(descr),
                     [bindingID, &location](const auto& attr)
                     {
                       return vk::VertexInputAttributeDescription{
                           .location = location++,
                           .binding = bindingID,
                           .format = attr.format,
                           .offset = attr.offset,
                       };
                     });
      bindingID++;
    }

    return descr;
  }

  Pipeline::Pipeline(const PipelineConfig& config) : device(config.framebuffer.GetDevice())
  {
    const auto& vkDevice = device.GetNative();

    auto bindingDescriptions = config.vertexInputInfo.GetNativeBindingDescriptions();
    auto attributeDescriptions = config.vertexInputInfo.GetNativeAttributeDescriptions();

    vk::PipelineVertexInputStateCreateInfo vertexInputInfo{
        .vertexBindingDescriptionCount = static_cast<std::uint32_t>(std::size(bindingDescriptions)),
        .pVertexBindingDescriptions = bindingDescriptions.data(),
        .vertexAttributeDescriptionCount = static_cast<std::uint32_t>(std::size(attributeDescriptions)),
        .pVertexAttributeDescriptions = attributeDescriptions.data(),
    };

    auto dynamicStates = std::array{vk::DynamicState::eViewport, vk::DynamicState::eScissor};
    vk::PipelineDynamicStateCreateInfo dynamicState{
        .dynamicStateCount = std::size(dynamicStates),
        .pDynamicStates = dynamicStates.data(),
    };

    vk::PipelineInputAssemblyStateCreateInfo inputAssembly{
        .topology = vk::PrimitiveTopology::eTriangleList,
        .primitiveRestartEnable = VK_FALSE,
    };

    auto size = config.framebuffer.GetSize();
    vk::Viewport viewport{
        .x = 0.F,
        .y = 0.F,
        .width = (float) size.width,
        .height = (float) size.height,
        .minDepth = 0.F,
        .maxDepth = 1.F,
    };

    vk::Rect2D scissor{
        .offset = {.x = 0, .y = 0},
        .extent = size,
    };

    vk::PipelineViewportStateCreateInfo viewportState{
        .viewportCount = 1,
        .pViewports = &viewport,
        .scissorCount = 1,
        .pScissors = &scissor,
    };

    vk::PipelineRasterizationStateCreateInfo rasterizer{
        .depthClampEnable = VK_FALSE,
        .rasterizerDiscardEnable = VK_FALSE,
        .polygonMode = vk::PolygonMode::eFill,
        .cullMode = vk::CullModeFlagBits::eBack,
        .frontFace = vk::FrontFace::eClockwise,
        .depthBiasEnable = VK_FALSE,
        .depthBiasConstantFactor = 0.F,
        .depthBiasClamp = 0.F,
        .depthBiasSlopeFactor = 0.F,
        .lineWidth = 1.F,
    };

    vk::PipelineMultisampleStateCreateInfo multisampling{
        .rasterizationSamples = vk::SampleCountFlagBits::e1,
        .sampleShadingEnable = VK_FALSE,
        .minSampleShading = 1.F,
        .pSampleMask = nullptr,
        .alphaToCoverageEnable = VK_FALSE,
        .alphaToOneEnable = VK_FALSE,
    };

    vk::PipelineColorBlendAttachmentState colorBlendAttachment{
        .blendEnable = config.blend ? VK_TRUE : VK_FALSE,
        .srcColorBlendFactor = vk::BlendFactor::eSrcAlpha,
        .dstColorBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha,
        .colorBlendOp = vk::BlendOp::eAdd,
        .srcAlphaBlendFactor = vk::BlendFactor::eOne,
        .dstAlphaBlendFactor = vk::BlendFactor::eZero,
        .alphaBlendOp = vk::BlendOp::eAdd,
        .colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
                          vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA,
    };

    vk::PipelineColorBlendStateCreateInfo colorBlending{
        .logicOpEnable = VK_FALSE,
        .logicOp = vk::LogicOp::eCopy,
        .attachmentCount = 1,
        .pAttachments = &colorBlendAttachment,
        .blendConstants =
            std::array{
                0.F,
                0.F,
                0.F,
                0.F,
            },
    };

    vk::PipelineDepthStencilStateCreateInfo depthStencil{
        .depthTestEnable = VK_TRUE,
        .depthWriteEnable = VK_TRUE,
        .depthCompareOp = vk::CompareOp::eLess,
        .depthBoundsTestEnable = VK_FALSE,
        .stencilTestEnable = VK_FALSE,
        .front = {},
        .back = {},
        .minDepthBounds = 0.F,
        .maxDepthBounds = 1.F,
    };

    vk::PipelineLayoutCreateInfo pipelineLayoutInfo{
        .setLayoutCount = 1,
        .pSetLayouts = &config.descriptorSetLayout.GetNative(),
        .pushConstantRangeCount = 0,
        .pPushConstantRanges = nullptr,
    };

    pipelineLayout = vkDevice.createPipelineLayout(pipelineLayoutInfo);

    std::size_t nbShaders = std::size(config.shaders);
    std::vector<vk::PipelineShaderStageCreateInfo> shaders;
    shaders.reserve(nbShaders);
    for (const auto& shader : config.shaders)
    {
      shaders.push_back(shader->GetNativeStage());
    }

    vk::GraphicsPipelineCreateInfo pipelineInfo{
        .stageCount = static_cast<std::uint32_t>(nbShaders),
        .pStages = shaders.data(),
        .pVertexInputState = &vertexInputInfo,
        .pInputAssemblyState = &inputAssembly,
        .pViewportState = &viewportState,
        .pRasterizationState = &rasterizer,
        .pMultisampleState = &multisampling,
        .pDepthStencilState = &depthStencil,
        .pColorBlendState = &colorBlending,
        .pDynamicState = &dynamicState,
        .layout = pipelineLayout,
        .renderPass = config.framebuffer.GetNativeRenderPass(),
        .subpass = 0,
        .basePipelineHandle = nullptr,
        .basePipelineIndex = -1,
    };

    vk::Result result{};

    std::tie(result, graphicsPipeline) = vkDevice.createGraphicsPipeline(nullptr, pipelineInfo);

    if (result != vk::Result::eSuccess)
    {
      throw std::runtime_error("Échec à la création du pipeline");
    }
  }

  Pipeline::Pipeline(Pipeline&& other) noexcept
      : device(other.device), pipelineLayout(other.pipelineLayout), graphicsPipeline(other.graphicsPipeline)
  {
    other.pipelineLayout = nullptr;
    other.graphicsPipeline = nullptr;
  }

  Pipeline::~Pipeline()
  {
    const auto& vkDevice = device.GetNative();

    vkDevice.destroyPipeline(graphicsPipeline);
    vkDevice.destroyPipelineLayout(pipelineLayout);
  }

  Pipeline& Pipeline::operator=(Pipeline&& other) noexcept
  {
    Expects(&device == &other.device);
    this->~Pipeline();
    pipelineLayout = other.pipelineLayout;
    graphicsPipeline = other.graphicsPipeline;
    other.pipelineLayout = nullptr;
    other.graphicsPipeline = nullptr;
    return *this;
  }

} // namespace SimpleGE::GFXAPI