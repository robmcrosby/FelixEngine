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
  _depthFormat = VK_FORMAT_UNDEFINED;
  _depthImageView = VK_NULL_HANDLE;
  _depthImage = VK_NULL_HANDLE;
  _depthImageMemory = VK_NULL_HANDLE;
}

VulkanFrameBuffer::~VulkanFrameBuffer() {
  clearBuffers();
}

bool VulkanFrameBuffer::loadXML(const XMLTree::Node &node) {
  return true;
}

bool VulkanFrameBuffer::setToWindow(int index) {
  _frameSize.w = (int)Vulkan::swapChainExtent.width;
  _frameSize.h = (int)Vulkan::swapChainExtent.height;
  
  return true;
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
  bool success = true;
  size_t width = _frameSize.w;
  size_t height = _frameSize.h;
  
  _depthFormat = findDepthFormat();
  _depthImage = Vulkan::createImage(width, height, _depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);
  _depthImageMemory = Vulkan::allocateImage(_depthImage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
  _depthImageView = Vulkan::createImageView(_depthImage, _depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
  
  Vulkan::transitionImageLayout(_depthImage, _depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
  
  success &= createRenderPass();
  success &= createFrameBuffers();
  return success;
}

bool VulkanFrameBuffer::createRenderPass() {
  // Define a single color attachment
  VkAttachmentDescription colorAttachment = {};
  colorAttachment.format = Vulkan::surfaceFormat;
  colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
  colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
  
  // Define a Color Attachment Refrence at index 0
  VkAttachmentReference colorAttachmentRef = {};
  colorAttachmentRef.attachment = 0;
  colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
  
  // Define the depth attachment
  VkAttachmentDescription depthAttachment = {};
  depthAttachment.format = _depthFormat;
  depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
  depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
  
  // Define a Depth Attachment Refrence at index 1
  VkAttachmentReference depthAttachmentRef = {};
  depthAttachmentRef.attachment = 1;
  depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
  
  // Define subpass using the Color and Depth Attachment Refrences
  VkSubpassDescription subpass = {};
  subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass.colorAttachmentCount = 1;
  subpass.pColorAttachments = &colorAttachmentRef;
  subpass.pDepthStencilAttachment = &depthAttachmentRef;
  
  //
  VkSubpassDependency dependency = {};
  dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
  dependency.dstSubpass = 0;
  dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.srcAccessMask = 0;
  dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
  
  // Create the Render Pass from the Color Attachment and Subpass
  VkAttachmentDescription attachments[] = {colorAttachment, depthAttachment};
  VkRenderPassCreateInfo renderPassInfo = {};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  renderPassInfo.attachmentCount = 2;
  renderPassInfo.pAttachments = attachments;
  renderPassInfo.subpassCount = 1;
  renderPassInfo.pSubpasses = &subpass;
  renderPassInfo.dependencyCount = 1;
  renderPassInfo.pDependencies = &dependency;
  
  // Create the Render Pass
  if (vkCreateRenderPass(Vulkan::device, &renderPassInfo, nullptr, &_renderPass) != VK_SUCCESS) {
    cerr << "Error Creating Render Pass" << endl;
    return false;
  }
  return true;
}

bool VulkanFrameBuffer::createFrameBuffers() {
  _swapChainFramebuffers.resize(Vulkan::swapChainBuffers.size());
  
  for (size_t i = 0; i < _swapChainFramebuffers.size(); i++) {
    VkImageView attachments[] = {Vulkan::swapChainBuffers[i].view, _depthImageView};
    
    VkFramebufferCreateInfo framebufferInfo = {};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = _renderPass;
    framebufferInfo.attachmentCount = 2;
    framebufferInfo.pAttachments = attachments;
    framebufferInfo.width = Vulkan::swapChainExtent.width;
    framebufferInfo.height = Vulkan::swapChainExtent.height;
    framebufferInfo.layers = 1;
    
    if (vkCreateFramebuffer(Vulkan::device, &framebufferInfo, nullptr, &_swapChainFramebuffers[i]) != VK_SUCCESS) {
      cerr << "Error Creating Frame Buffer" << endl;
      return false;
    }
  }
  return true;
}

void VulkanFrameBuffer::clearBuffers() {
  
  if (_depthImageView != VK_NULL_HANDLE) {
    vkDestroyImageView(Vulkan::device, _depthImageView, nullptr);
    _depthImageView = VK_NULL_HANDLE;
  }
  if (_depthImage != VK_NULL_HANDLE) {
    vkDestroyImage(Vulkan::device, _depthImage, nullptr);
    _depthImage = VK_NULL_HANDLE;
  }
  if (_depthImageMemory != VK_NULL_HANDLE) {
    vkFreeMemory(Vulkan::device, _depthImageMemory, nullptr);
    _depthImageMemory = VK_NULL_HANDLE;
  }
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
  // TODO: Implement Key
  int key = 0;
  
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
    viewport.width = (float)Vulkan::swapChainExtent.width;
    viewport.height = (float)Vulkan::swapChainExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    
    // Define the scissor region
    VkRect2D scissor = {};
    scissor.offset = {0, 0};
    scissor.extent = Vulkan::swapChainExtent;
    
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
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE; // VK_FRONT_FACE_COUNTER_CLOCKWISE
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
    depthStencil.depthTestEnable = VK_TRUE;
    depthStencil.depthWriteEnable = VK_TRUE;
    depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.stencilTestEnable = VK_FALSE;
    
    
    // Define Color Blending State Info
    VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional
    
    
    // Define the Blend Color Info
    VkPipelineColorBlendStateCreateInfo colorBlending = {};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f; // Optional
    colorBlending.blendConstants[1] = 0.0f; // Optional
    colorBlending.blendConstants[2] = 0.0f; // Optional
    colorBlending.blendConstants[3] = 0.0f; // Optional
    
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
    pipelineInfo.pDepthStencilState = &depthStencil; //nullptr; // Optional
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = nullptr; // Optional
    pipelineInfo.layout = _pipelineLayout;
    pipelineInfo.renderPass = _renderPass; //Vulkan::renderPass;
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
  return _pipelines[key];
}
