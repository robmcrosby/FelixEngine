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
  mKernalShader = mDevice->createShader(VK_SHADER_STAGE_COMPUTE_BIT);
  return mKernalShader->load(filename, entry);
}

bool VulkanPipeline::setVertexShader(StringRef filename, StringRef entry) {
  mVertexShader = mDevice->createShader(VK_SHADER_STAGE_VERTEX_BIT);
  return mVertexShader->load(filename, entry);
}

bool VulkanPipeline::setFragmentShader(StringRef filename, StringRef entry) {
  mFragmentShader = mDevice->createShader(VK_SHADER_STAGE_FRAGMENT_BIT);
  return mFragmentShader->load(filename, entry);
}

void VulkanPipeline::clearShaders() {
  mKernalShader = nullptr;
  mVertexShader = nullptr;
  mFragmentShader = nullptr;
}

VkPipeline VulkanPipeline::getVkPipeline() {
  return getVkPipeline(getVkDescriptorSetLayouts());
}

VkPipeline VulkanPipeline::getVkPipeline(VkDescriptorSetLayouts setLayouts) {
  if (mVkPipeline == VK_NULL_HANDLE)
    mVkPipeline = createComputePipeline(setLayouts);
  return mVkPipeline;
}

VkPipeline VulkanPipeline::getVkPipeline(
  VulkanRenderPassPtr renderPass,
  VulkanMeshPtr       mesh,
  VulkanLayoutSetPtr  layoutSet
) {
  if (mVkPipeline == VK_NULL_HANDLE)
    mVkPipeline = createGraphicsPipeline(renderPass, mesh, layoutSet);
  return mVkPipeline;
}

VkPipelineLayout VulkanPipeline::getVkPipelineLayout() {
  if (mVkPipelineLayout == VK_NULL_HANDLE)
    mVkPipelineLayout = getVkPipelineLayout(getVkDescriptorSetLayouts());
  return mVkPipelineLayout;
}

VkPipelineLayout VulkanPipeline::getVkPipelineLayout(VkDescriptorSetLayouts setLayouts) {
  if (mVkPipelineLayout == VK_NULL_HANDLE)
    mVkPipelineLayout = createVkPipelineLayout(setLayouts);
  return mVkPipelineLayout;
}

VkPipeline VulkanPipeline::createGraphicsPipeline(
  VulkanRenderPassPtr renderPass,
  VulkanMeshPtr       mesh,
  VulkanLayoutSetPtr  layoutSet
) {
  VkPipeline pipeline = VK_NULL_HANDLE;

  VkDescriptorSetLayouts setLayouts;
  if (layoutSet != nullptr)
    layoutSet->getVkDescriptorSetLayouts(setLayouts);
  auto pipelineLayout = getVkPipelineLayout(setLayouts);
  if (pipelineLayout != VK_NULL_HANDLE) {
    auto viewport = renderPass->getViewport();
    auto scissor  = renderPass->getScissor();

    auto vertexStage = mVertexShader->getVkPipelineShaderStageCreateInfo();
    auto fragmentStage = mFragmentShader->getVkPipelineShaderStageCreateInfo();
    VkPipelineShaderStageCreateInfo shaderStages[] = {vertexStage, fragmentStage};

    VkVertexInputBindingDescriptions   vertexBindings;
    VkVertexInputAttributeDescriptions vertexAttributes;
    VkPrimitiveTopology primitiveTopology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    if (mesh != nullptr) {
      mesh->getVertexBindings(vertexBindings);
      mesh->getVertexAttributes(vertexAttributes);
      primitiveTopology = mesh->getVkPrimitiveTopology();
    }

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO};
    vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(vertexBindings.size());
    vertexInputInfo.pVertexBindingDescriptions = vertexBindings.data();
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexAttributes.size());
    vertexInputInfo.pVertexAttributeDescriptions = vertexAttributes.data();

    VkPipelineInputAssemblyStateCreateInfo inputAssembly{VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO};
    inputAssembly.topology = primitiveTopology;
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
    pipelineInfo.layout = pipelineLayout;
    pipelineInfo.renderPass = renderPass->getVkRenderPass();
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
    pipelineInfo.basePipelineIndex = -1;

    VkDevice device = mDevice->getVkDevice();
    if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline) != VK_SUCCESS)
      cerr << "Error creating graphics pipeline";
  }
  return pipeline;
}

VkPipeline VulkanPipeline::createComputePipeline(VkDescriptorSetLayouts setLayouts) {
  VkComputePipelineCreateInfo createInfo = {VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO, 0, 0};
  createInfo.stage  = mKernalShader->getVkPipelineShaderStageCreateInfo();
  createInfo.layout = getVkPipelineLayout(setLayouts);
  createInfo.basePipelineHandle = 0;
  createInfo.basePipelineIndex = 0;

  VkDevice device = mDevice->getVkDevice();
  VkPipeline pipeline = VK_NULL_HANDLE;
  if (vkCreateComputePipelines(device, 0, 1, &createInfo, 0, &pipeline) != VK_SUCCESS)
    cerr << "Error creating compute pipeline";
  return pipeline;
}

VkPipelineLayout VulkanPipeline::createVkPipelineLayout(const VkDescriptorSetLayouts& setLayouts) {
  VkPipelineLayoutCreateInfo createInfo = {VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO, 0, 0};
  createInfo.setLayoutCount = static_cast<uint32_t>(setLayouts.size());
  createInfo.pSetLayouts = setLayouts.data();
  createInfo.pushConstantRangeCount = 0;
  createInfo.pPushConstantRanges = nullptr;

  VkDevice device = mDevice->getVkDevice();
  VkPipelineLayout pipelineLayout;
  if (vkCreatePipelineLayout(device, &createInfo, 0, &pipelineLayout) != VK_SUCCESS) {
    cerr << "Error creating Pipeline Layout" << endl;
    pipelineLayout = VK_NULL_HANDLE;
  }
  return pipelineLayout;
}

VkDescriptorSetLayouts& VulkanPipeline::getVkDescriptorSetLayouts() {
  if (mVkDescriptorSetLayouts.size() == 0) {
    VkDevice device = mDevice->getVkDevice();
    LayoutBindingSets layoutBindingSets;
    getLayoutBindingSets(layoutBindingSets);
    for (const auto& layoutSet : layoutBindingSets) {
      VkDescriptorSetLayoutBindings bindings;
      for (const auto& layout : layoutSet)
        bindings.push_back(layout.binding);

      VkDescriptorSetLayoutCreateInfo createInfo = {VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO, 0, 0};
      createInfo.bindingCount = static_cast<uint32_t>(bindings.size());
      createInfo.pBindings = bindings.data();

      VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
      if (vkCreateDescriptorSetLayout(device, &createInfo, 0, &descriptorSetLayout) != VK_SUCCESS)
        cerr << "Error creating Descriptor Set Layout" << endl;
      mVkDescriptorSetLayouts.push_back(descriptorSetLayout);
    }
  }
  return mVkDescriptorSetLayouts;
}

void VulkanPipeline::clearVkDescriptorSetLayouts() {
  VkDevice device = mDevice->getVkDevice();
  for (auto setLayout : mVkDescriptorSetLayouts)
    vkDestroyDescriptorSetLayout(device, setLayout, nullptr);
  mVkDescriptorSetLayouts.clear();
}

void VulkanPipeline::getLayoutBindingSets(LayoutBindingSets& layoutBindingSets) {
  layoutBindingSets = mKernalShader->getLayoutBindingSets();
}

void VulkanPipeline::destroy() {
  clearShaders();
  destroyPipelineLayout();
  destroyPipeline();
  clearVkDescriptorSetLayouts();
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
