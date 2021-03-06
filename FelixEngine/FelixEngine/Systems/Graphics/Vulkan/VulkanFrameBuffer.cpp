//
//  VulkanFrameBuffer.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 5/3/18.
//  Copyright © 2018 Robert Crosby. All rights reserved.
//

#include "VulkanFrameBuffer.h"
#include "VulkanTextureBuffer.h"
#include "VulkanShaderProgram.h"
#include "VulkanVertexMesh.h"
#include "VulkanUniformBuffer.h"
#include "GraphicTask.h"
#include "Vulkan.h"


using namespace std;
using namespace fx;

VulkanFrameBuffer::VulkanFrameBuffer() {
  _pipelineLayout = VK_NULL_HANDLE;
}

VulkanFrameBuffer::~VulkanFrameBuffer() {
  clearBuffers();
}

bool VulkanFrameBuffer::loadXML(const XMLTree::Node &node) {
  return true;
}

bool VulkanFrameBuffer::setToWindow(int index) {
  _colorAttachments.clear();
  AttachmentPtr attachment = make_shared<VulkanFrameAttachment>();
  if (attachment->loadSwapBuffers(index)) {
    _frameSize = attachment->getBufferSize();
    _colorAttachments.push_back(attachment);
    return true;
  }
  return false;
}

bool VulkanFrameBuffer::resize(int width, int height) {
  return true;
}

ivec2 VulkanFrameBuffer::size() const {
  int width = (int)Vulkan::swapChainExtent.width;
  int height = (int)Vulkan::swapChainExtent.height;
  return ivec2(width, height);
}

bool VulkanFrameBuffer::addDepthBuffer() {
  _depthAttachment = make_shared<VulkanFrameAttachment>();
  return _depthAttachment->loadDepthBuffer(findDepthFormat(), _frameSize.w, _frameSize.h);
}

bool VulkanFrameBuffer::createRenderPass(const GraphicTask &task) {
  int totalColorAttachments = (int)_colorAttachments.size();
  vector<VkAttachmentReference> colorAttachmentRefrences(totalColorAttachments);
  vector<VkAttachmentDescription> attachmentInfos(totalColorAttachments);
  for (int i = 0; i < totalColorAttachments; ++i) {
    colorAttachmentRefrences[i].attachment = i;
    colorAttachmentRefrences[i].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    
    attachmentInfos[i].format = _colorAttachments[i]->getFormat();
    attachmentInfos[i].samples = VK_SAMPLE_COUNT_1_BIT;
    attachmentInfos[i].loadOp = getAttachmentLoadOp(task.colorActions[i].loadAction);
    attachmentInfos[i].storeOp = getAttachmentStoreOp(task.colorActions[i].storeAction);
    attachmentInfos[i].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachmentInfos[i].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachmentInfos[i].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachmentInfos[i].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
  }
  
  vector<VkAttachmentReference> depthAttachmentRefrences;
  if (_depthAttachment) {
    VkAttachmentReference depthAttachmentRef = {};
    depthAttachmentRef.attachment = (uint32_t)attachmentInfos.size();
    depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    depthAttachmentRefrences.push_back(depthAttachmentRef);
    
    VkAttachmentDescription depthAttachmentInfo = {};
    depthAttachmentInfo.format = _depthAttachment->getFormat();
    depthAttachmentInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    depthAttachmentInfo.loadOp = getAttachmentLoadOp(task.depthStencilAction.loadAction);
    depthAttachmentInfo.storeOp = getAttachmentStoreOp(task.depthStencilAction.storeAction);
    depthAttachmentInfo.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachmentInfo.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachmentInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachmentInfo.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    attachmentInfos.push_back(depthAttachmentInfo);
  }
  
  // Define subpass using the Color and Depth Attachment Refrences
  VkSubpassDescription subpass = {};
  subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass.colorAttachmentCount = (uint32_t)colorAttachmentRefrences.size();
  subpass.pColorAttachments = colorAttachmentRefrences.size() > 0 ? colorAttachmentRefrences.data() : nullptr;
  subpass.pDepthStencilAttachment = depthAttachmentRefrences.size() > 0 ? depthAttachmentRefrences.data() : nullptr;
  
  //
  VkSubpassDependency dependency = {};
  dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
  dependency.dstSubpass = 0;
  dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.srcAccessMask = 0;
  dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
  
  // Create the Render Pass from the Color Attachment and Subpass
  VkRenderPassCreateInfo renderPassInfo = {};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  renderPassInfo.attachmentCount = (uint32_t)attachmentInfos.size();
  renderPassInfo.pAttachments = attachmentInfos.data();
  renderPassInfo.subpassCount = 1;
  renderPassInfo.pSubpasses = &subpass;
  renderPassInfo.dependencyCount = 1;
  renderPassInfo.pDependencies = &dependency;
  
  // Create the Render Pass
  VkRenderPass renderPass;
  if (vkCreateRenderPass(Vulkan::device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
    cerr << "Error Creating Render Pass" << endl;
    return false;
  }
  _renderPasses[task.getActionStateKey()] = renderPass;
  return true;
}

bool VulkanFrameBuffer::createFrameBuffers(const GraphicTask &task) {
  int key = task.getActionStateKey();
  int totalBuffers = _colorAttachments.front()->getBufferCount();
  
  _framebuffers[key].resize(totalBuffers);
  
  for (int i = 0; i < totalBuffers; i++) {
    // Get the Attachements
    vector<VkImageView> attachments;
    for (auto &colorAttachemnt : _colorAttachments)
      attachments.push_back(colorAttachemnt->getImageView(i));
    if (_depthAttachment)
      attachments.push_back(_depthAttachment->getImageView());
    
    // Define the Frame Buffer
    VkFramebufferCreateInfo framebufferInfo = {};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = getRenderPass(task);
    framebufferInfo.attachmentCount = (uint32_t)attachments.size();
    framebufferInfo.pAttachments = attachments.data();
    framebufferInfo.width = _frameSize.width;
    framebufferInfo.height = _frameSize.height;
    framebufferInfo.layers = 1;
    
    // Create the Frame Buffer
    if (vkCreateFramebuffer(Vulkan::device, &framebufferInfo, nullptr, &_framebuffers[key][i]) != VK_SUCCESS) {
      cerr << "Error Creating Frame Buffer" << endl;
      return false;
    }
  }
  return true;
}

void VulkanFrameBuffer::clearBuffers() {
  // Clear the Color Buffers
  _colorAttachments.clear();
  
  // Clear the DepthStencil Buffer
  _depthAttachment = nullptr;
  
  // Clear the Render Passes
  for (auto &renderPass : _renderPasses)
    vkDestroyRenderPass(Vulkan::device, renderPass.second, nullptr);
  _renderPasses.clear();
}

VkFormat VulkanFrameBuffer::findDepthFormat() {
  vector<VkFormat> candidates = {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT};
  VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL;
  VkFormatFeatureFlags features = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;
  
  return Vulkan::findSupportedFormat(candidates, tiling, features);
}

bool VulkanFrameBuffer::addColorTexture() {
  return true;
}

TextureBufferPtr VulkanFrameBuffer::getColorTexture(int index) {
  shared_ptr<VulkanTextureBuffer> texture = make_shared<VulkanTextureBuffer>();
  return texture;
}

VkPipeline VulkanFrameBuffer::getVkPipelineForTask(const GraphicTask &task) {
  string key = generateKey(task);
  
  if (!_pipelines.count(key)) {
    VulkanShaderProgram *shader = static_cast<VulkanShaderProgram*>(task.shader.get());
    VulkanVertexMesh *mesh = static_cast<VulkanVertexMesh*>(task.mesh.get());
    
    // Define the Vertex Input Info
    vector<VkVertexInputAttributeDescription> vertexAttributes = mesh->getAttributeDescriptions(shader);
    VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = mesh->getBindingDescriptionsCount();;
    vertexInputInfo.pVertexBindingDescriptions = mesh->getBindingDescriptions();
    vertexInputInfo.vertexAttributeDescriptionCount = (uint32_t)vertexAttributes.size();
    vertexInputInfo.pVertexAttributeDescriptions = vertexAttributes.data();
    
    // Define to Draw Triangle Prmitives
    VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = mesh->getPrimitiveTopology();
    inputAssembly.primitiveRestartEnable = VK_FALSE;
    
    // Define the viewport
    VkViewport viewport = {};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)_frameSize.width;
    viewport.height = (float)_frameSize.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    
    // Define the scissor region
    VkRect2D scissor = {};
    scissor.offset = {0, 0};
    scissor.extent.width = (uint32_t)_frameSize.width;
    scissor.extent.height = (uint32_t)_frameSize.height;
    
    // Define the Viewport State Info
    VkPipelineViewportStateCreateInfo viewportState = {};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;
    
    // Define the Rasterization State Info
    VkPipelineRasterizationStateCreateInfo rasterizer = {};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = getCullMode(task.cullMode);
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;
    rasterizer.depthBiasConstantFactor = 0.0f; // Optional
    rasterizer.depthBiasClamp = 0.0f; // Optional
    rasterizer.depthBiasSlopeFactor = 0.0f; // Optional
    
    
    // Define Multisampling State Info
    VkPipelineMultisampleStateCreateInfo multisampling = {};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampling.minSampleShading = 1.0f; // Optional
    multisampling.pSampleMask = nullptr; // Optional
    multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
    multisampling.alphaToOneEnable = VK_FALSE; // Optional
    
    
    // Define Depth Stencil State Info
    VkPipelineDepthStencilStateCreateInfo depthStencil = {};
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable = task.depthState.enabled() ? VK_TRUE : VK_FALSE;
    depthStencil.depthWriteEnable = task.depthState.writingEnabled() ? VK_TRUE : VK_FALSE;
    depthStencil.depthCompareOp = getDepthCompareOp(task.depthState.function());
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.stencilTestEnable = VK_FALSE;
    
    
    // Define Color Blending State Info
    vector<VkPipelineColorBlendAttachmentState> colorBlendAttachments(_colorAttachments.size());
    for (int i = 0; i < colorBlendAttachments.size(); ++i) {
      colorBlendAttachments[i].colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
      colorBlendAttachments[i].blendEnable = task.blendState.enabled() ? VK_TRUE : VK_FALSE;
      colorBlendAttachments[i].srcColorBlendFactor = getBlendFactor(task.blendState.src());
      colorBlendAttachments[i].dstColorBlendFactor = getBlendFactor(task.blendState.dst());
      colorBlendAttachments[i].colorBlendOp = getBlendOp(task.blendState.equation());
      colorBlendAttachments[i].srcAlphaBlendFactor = getBlendFactor(task.blendState.srcAlpha());
      colorBlendAttachments[i].dstAlphaBlendFactor = getBlendFactor(task.blendState.dstAlpha());
      colorBlendAttachments[i].alphaBlendOp = VK_BLEND_OP_ADD; // Optional
    }
    
    // Define the Blend Color Info
    VkPipelineColorBlendStateCreateInfo colorBlending = {};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
    colorBlending.attachmentCount = (uint32_t)colorBlendAttachments.size();
    colorBlending.pAttachments = colorBlendAttachments.data();
    colorBlending.blendConstants[0] = task.blendState.color.r;
    colorBlending.blendConstants[1] = task.blendState.color.g;
    colorBlending.blendConstants[2] = task.blendState.color.b;
    colorBlending.blendConstants[3] = task.blendState.color.a;
    
    // Define Pipeline Layout for Constant Values
    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
    vector<VkDescriptorSetLayout> descriptorSetLayouts = shader->getDescriptorSetLayouts();
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = (uint32_t)descriptorSetLayouts.size();
    pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.size() == 0 ? nullptr : descriptorSetLayouts.data();
    pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
    pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional
    
    // Create the Pipeline Layout
    if (vkCreatePipelineLayout(Vulkan::device, &pipelineLayoutInfo, nullptr, &_pipelineLayout) != VK_SUCCESS) {
      cerr << "Error creating Pipeline Layout" << endl;
      assert(false);
    }
    
    // Define the Pipeline info
    VkGraphicsPipelineCreateInfo pipelineInfo = {};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = shader->getStageCount();
    pipelineInfo.pStages = shader->getStages();
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pDepthStencilState = hasDepthBuffer() ? &depthStencil : nullptr;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = nullptr; // Optional
    pipelineInfo.layout = _pipelineLayout;
    pipelineInfo.renderPass = getRenderPass(task);
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
    pipelineInfo.basePipelineIndex = -1; // Optional
    
    // Create the Graphics Pipeline
    VkPipeline pipeline;
    if (vkCreateGraphicsPipelines(Vulkan::device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline) != VK_SUCCESS) {
      cerr << "Error creating Graphics Pipeline" << endl;
      assert(false);
    }
    _pipelines[key] = pipeline;
  }
  return _pipelines.at(key);
}

VkRenderPass VulkanFrameBuffer::getRenderPass(const GraphicTask &task) {
  uint32_t key = task.getActionStateKey();
  if (_renderPasses.count(key) == 0)
    assert(createRenderPass(task));
  return _renderPasses.at(key);
}

VkFramebuffer VulkanFrameBuffer::getFrameBuffer(const GraphicTask &task) {
  int key = task.getActionStateKey();
  if (_framebuffers.count(key) == 0)
    createFrameBuffers(task);
  return _framebuffers[key][_colorAttachments.front()->getCurrentIndex()];
}

VkPipelineLayout VulkanFrameBuffer::getPipelineLayout() {
  return _pipelineLayout;
}

string VulkanFrameBuffer::generateKey(const GraphicTask &task) {
  stringstream ss;
  ss << "C" << task.cullMode;
  ss << "D" << task.depthState.flags;
  ss << "B" << task.blendState.flags;
  ss << task.blendState.color.hexColor();
  ss << "M" << task.mesh->getMeshId();
  ss << "S" << task.shader->getShaderId();
  return ss.str();
}

VkCullModeFlags VulkanFrameBuffer::getCullMode(CULL_MODE mode) {
  switch (mode) {
    case CULL_NONE:
      return VK_CULL_MODE_NONE;
    case CULL_BACK:
      return VK_CULL_MODE_BACK_BIT;
    case CULL_FRONT:
      return VK_CULL_MODE_FRONT_BIT;
  }
  return VK_CULL_MODE_NONE;
}

VkAttachmentLoadOp VulkanFrameBuffer::getAttachmentLoadOp(LOAD_ACTION action) {
  switch (action) {
    case LOAD_NONE:
      return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    case LOAD_LAST:
      return VK_ATTACHMENT_LOAD_OP_LOAD;
    case LOAD_CLEAR:
      return VK_ATTACHMENT_LOAD_OP_CLEAR;
  }
  return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
}

VkAttachmentStoreOp VulkanFrameBuffer::getAttachmentStoreOp(STORE_ACTION action) {
  switch (action) {
    case STORE_DISCARD:
      return VK_ATTACHMENT_STORE_OP_DONT_CARE;
    case STORE_SAVE:
      return VK_ATTACHMENT_STORE_OP_STORE;
  }
  return VK_ATTACHMENT_STORE_OP_DONT_CARE;
}

VkCompareOp VulkanFrameBuffer::getDepthCompareOp(int operation) {
  switch (operation) {
    case DEPTH_TEST_NEVER:
      return VK_COMPARE_OP_NEVER;
    case DEPTH_TEST_EQUAL:
      return VK_COMPARE_OP_EQUAL;
    case DEPTH_TEST_LESS:
      return VK_COMPARE_OP_LESS;
    case DEPTH_TEST_GREATER:
      return VK_COMPARE_OP_GREATER;
    case DEPTH_TEST_LESS_EQ:
      return VK_COMPARE_OP_LESS_OR_EQUAL;
    case DEPTH_TEST_GREATER_EQ:
      return VK_COMPARE_OP_GREATER_OR_EQUAL;
  }
  return VK_COMPARE_OP_ALWAYS;
}

VkBlendFactor VulkanFrameBuffer::getBlendFactor(int factor) {
  switch (factor) {
    case BLEND_INPUT_ZERO:
      return VK_BLEND_FACTOR_ZERO;
    case BLEND_INPUT_ONE:
      return VK_BLEND_FACTOR_ONE;
    case BLEND_INPUT_SRC_COLOR:
      return VK_BLEND_FACTOR_SRC_COLOR;
    case BLEND_INPUT_SRC_ALPHA:
      return VK_BLEND_FACTOR_SRC_ALPHA;
    case BLEND_INPUT_DST_COLOR:
      return VK_BLEND_FACTOR_DST_COLOR;
    case BLEND_INPUT_DST_ALPHA:
      return VK_BLEND_FACTOR_DST_ALPHA;
    case BLEND_INPUT_CONST_COLOR:
      return VK_BLEND_FACTOR_CONSTANT_COLOR;
    case BLEND_INPUT_CONST_ALPHA:
      return VK_BLEND_FACTOR_CONSTANT_ALPHA;
    case BLEND_INPUT_SRC_COLOR | BLEND_INPUT_ONE_MINUS:
      return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
    case BLEND_INPUT_SRC_ALPHA | BLEND_INPUT_ONE_MINUS:
      return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    case BLEND_INPUT_DST_COLOR | BLEND_INPUT_ONE_MINUS:
      return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
    case BLEND_INPUT_DST_ALPHA | BLEND_INPUT_ONE_MINUS:
      return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
    case BLEND_INPUT_CONST_COLOR | BLEND_INPUT_ONE_MINUS:
      return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR;
    case BLEND_INPUT_CONST_ALPHA | BLEND_INPUT_ONE_MINUS:
      return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA;
  }
  return VK_BLEND_FACTOR_SRC_ALPHA_SATURATE;
}

VkBlendOp VulkanFrameBuffer::getBlendOp(int equation) {
  switch (equation) {
    case BLEND_EQ_ADD:
      return VK_BLEND_OP_ADD;
    case BLEND_EQ_SUBTRACT:
      return VK_BLEND_OP_SUBTRACT;
    case BLEND_EQ_REV_SUBTRACT:
      return VK_BLEND_OP_REVERSE_SUBTRACT;
    case BLEND_EQ_MIN:
      return VK_BLEND_OP_MIN;
    case BLEND_EQ_MAX:
      return VK_BLEND_OP_MAX;
  }
  return VK_BLEND_OP_ZERO_EXT;
}
