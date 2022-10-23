#include "VulkanIncludes.hpp"


using namespace std;


VulkanPipeline::VulkanPipeline(VulkanDevice* device):
  mDevice(device),
  mVkPipeline(VK_NULL_HANDLE),
  mVkPipelineLayout(VK_NULL_HANDLE) {

}

VulkanPipeline::~VulkanPipeline() {
  destroy();
}

bool VulkanPipeline::setKernalShader(StringRef filename, StringRef entry) {
  mKernalShader = mDevice->createShader();
  return mKernalShader->load(filename, entry);
}

bool VulkanPipeline::setVertexShader(StringRef filename, StringRef entry) {
  mVertexShader = mDevice->createShader();
  return mVertexShader->load(filename, entry);
}

bool VulkanPipeline::setFragmentShader(StringRef filename, StringRef entry) {
  mFragmentShader = mDevice->createShader();
  return mFragmentShader->load(filename, entry);
}

void VulkanPipeline::clearShaders() {
  mKernalShader = nullptr;
  mVertexShader = nullptr;
  mFragmentShader = nullptr;
}

VkPipeline VulkanPipeline::getVkPipeline(VulkanSetLayoutPtr layout) {
  if (mVkPipeline == VK_NULL_HANDLE) {
    createComputePipeline(layout);
  }
  return mVkPipeline;
}

VkPipeline VulkanPipeline::getVkPipeline(VulkanRenderPassPtr renderPass) {
  if (mVkPipeline == VK_NULL_HANDLE) {
    createGraphicsPipeline(renderPass);
  }
  return mVkPipeline;
}

VkPipelineLayout VulkanPipeline::getVkPipelineLayout(VulkanSetLayoutPtr layout) {
  if (mVkPipelineLayout == VK_NULL_HANDLE) {
    VkDescriptorSetLayout setLayout = layout->getVkDescriptorSetLayout();
    if (setLayout != VK_NULL_HANDLE) {
      VkPipelineLayoutCreateInfo createInfo = {VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO, 0, 0};
      createInfo.setLayoutCount = 1;
      createInfo.pSetLayouts = &setLayout;
      createInfo.pushConstantRangeCount = 0;
      createInfo.pPushConstantRanges = nullptr;

      VkDevice device = mDevice->getVkDevice();
      if (vkCreatePipelineLayout(device, &createInfo, 0, &mVkPipelineLayout) != VK_SUCCESS) {
        cerr << "Error creating Pipeline Layout" << endl;
        mVkPipelineLayout = VK_NULL_HANDLE;
      }
    }
  }
  return mVkPipelineLayout;
}

VkPipelineLayout VulkanPipeline::getVkPipelineLayout() {
  if (mVkPipelineLayout == VK_NULL_HANDLE) {
    VkPipelineLayoutCreateInfo createInfo = {VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO, 0, 0};
    createInfo.setLayoutCount = 0;
    createInfo.pSetLayouts = nullptr;
    createInfo.pushConstantRangeCount = 0;
    createInfo.pPushConstantRanges = nullptr;

    VkDevice device = mDevice->getVkDevice();
    if (vkCreatePipelineLayout(device, &createInfo, 0, &mVkPipelineLayout) != VK_SUCCESS) {
      cerr << "Error creating Pipeline Layout" << endl;
      mVkPipelineLayout = VK_NULL_HANDLE;
    }
  }
  return mVkPipelineLayout;
}

void VulkanPipeline::createGraphicsPipeline(VulkanRenderPassPtr renderPass) {
  auto pipelineLayout = getVkPipelineLayout();
  if (pipelineLayout != VK_NULL_HANDLE) {
    auto viewport = renderPass->getViewport();
    auto scissor  = renderPass->getScissor();

    VkPipelineShaderStageCreateInfo vertexStage{VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO};
    vertexStage.stage  = VK_SHADER_STAGE_VERTEX_BIT;
    vertexStage.module = mVertexShader->getVkShaderModule();
    vertexStage.pName  = mVertexShader->getEntryFunction();
    VkPipelineShaderStageCreateInfo fragmentStage{VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO};
    fragmentStage.stage  = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragmentStage.module = mFragmentShader->getVkShaderModule();
    fragmentStage.pName  = mFragmentShader->getEntryFunction();
    VkPipelineShaderStageCreateInfo shaderStages[] = {vertexStage, fragmentStage};

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO};
    vertexInputInfo.vertexBindingDescriptionCount = 0;
    vertexInputInfo.pVertexBindingDescriptions = nullptr;
    vertexInputInfo.vertexAttributeDescriptionCount = 0;
    vertexInputInfo.pVertexAttributeDescriptions = nullptr;

    VkPipelineInputAssemblyStateCreateInfo inputAssembly{VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO};
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    VkPipelineViewportStateCreateInfo viewportState{VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO};
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;

    VkPipelineRasterizationStateCreateInfo rasterizer{VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO};
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_NONE;
    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;
    rasterizer.depthBiasConstantFactor = 0.0f;
    rasterizer.depthBiasClamp = 0.0f;
    rasterizer.depthBiasSlopeFactor = 0.0f;

    VkPipelineMultisampleStateCreateInfo multisampling{VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO};
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampling.minSampleShading = 1.0f;
    multisampling.pSampleMask = nullptr;
    multisampling.alphaToCoverageEnable = VK_FALSE;
    multisampling.alphaToOneEnable = VK_FALSE;

    VkPipelineDepthStencilStateCreateInfo depthStencil{VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO};
    depthStencil.depthTestEnable = VK_FALSE;
    depthStencil.depthWriteEnable = VK_FALSE;
    depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.minDepthBounds = 0.0f;
    depthStencil.maxDepthBounds = 1.0f;
    depthStencil.stencilTestEnable = VK_FALSE;
    depthStencil.front = {};
    depthStencil.back = {};

    VkPipelineColorBlendAttachmentState colorBlendAttachment;
    colorBlendAttachment.colorWriteMask =
      VK_COLOR_COMPONENT_R_BIT |
      VK_COLOR_COMPONENT_G_BIT |
      VK_COLOR_COMPONENT_B_BIT |
      VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable         = VK_FALSE;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachment.colorBlendOp        = VK_BLEND_OP_ADD;
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachment.alphaBlendOp        = VK_BLEND_OP_ADD;

    VkPipelineColorBlendStateCreateInfo colorBlending{VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO};
    colorBlending.logicOpEnable     = VK_FALSE;
    colorBlending.logicOp           = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount   = 1;
    colorBlending.pAttachments      = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f;
    colorBlending.blendConstants[1] = 0.0f;
    colorBlending.blendConstants[2] = 0.0f;
    colorBlending.blendConstants[3] = 0.0f;

    VkGraphicsPipelineCreateInfo pipelineInfo{VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO};
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pDepthStencilState = &depthStencil;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = nullptr;
    pipelineInfo.layout = mVkPipelineLayout;
    pipelineInfo.renderPass = renderPass->getVkRenderPass();
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
    pipelineInfo.basePipelineIndex = -1;

    VkDevice device = mDevice->getVkDevice();
    if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &mVkPipeline) != VK_SUCCESS) {
      cerr << "Error creating graphics pipeline";
      mVkPipeline = VK_NULL_HANDLE;
    }
  }
}

void VulkanPipeline::createComputePipeline(VulkanSetLayoutPtr layout) {
  auto pipelineLayout = getVkPipelineLayout(layout);
  if (pipelineLayout != VK_NULL_HANDLE) {
    VkComputePipelineCreateInfo createInfo = {VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO, 0, 0};
    createInfo.stage = {VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO};
    createInfo.stage.pNext = nullptr;
    createInfo.stage.flags = 0;
    createInfo.stage.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    createInfo.stage.module = mKernalShader->getVkShaderModule();
    createInfo.stage.pName = mKernalShader->getEntryFunction();
    createInfo.stage.pSpecializationInfo = nullptr;
    createInfo.layout = pipelineLayout;
    createInfo.basePipelineHandle = 0;
    createInfo.basePipelineIndex = 0;

    VkDevice device = mDevice->getVkDevice();
    if (vkCreateComputePipelines(device, 0, 1, &createInfo, 0, &mVkPipeline) != VK_SUCCESS) {
      cerr << "Error creating compute pipeline";
      mVkPipeline = VK_NULL_HANDLE;
    }
  }
}

void VulkanPipeline::destroy() {
  clearShaders();
  destroyPipelineLayout();
  destroyPipeline();
}

void VulkanPipeline::destroyPipelineLayout() {
  if (mVkPipelineLayout != VK_NULL_HANDLE) {
    VkDevice device = mDevice->getVkDevice();
    vkDestroyPipelineLayout(device, mVkPipelineLayout, nullptr);
    mVkPipelineLayout = VK_NULL_HANDLE;
  }
}

void VulkanPipeline::destroyPipeline() {
  if (mVkPipeline != VK_NULL_HANDLE) {
    VkDevice device = mDevice->getVkDevice();
    vkDestroyPipeline(device, mVkPipeline, nullptr);
    mVkPipeline = VK_NULL_HANDLE;
  }
}
