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
  
}

VulkanFrameBuffer::~VulkanFrameBuffer() {
  
}

bool VulkanFrameBuffer::loadXML(const XMLTree::Node &node) {
  return true;
}

bool VulkanFrameBuffer::setToWindow(int index) {
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
  return true;
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
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
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
    if (vkCreatePipelineLayout(Vulkan::device, &pipelineLayoutInfo, nullptr, &Vulkan::pipelineLayout) != VK_SUCCESS) {
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
    pipelineInfo.pDepthStencilState = nullptr; // Optional
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = nullptr; // Optional
    pipelineInfo.layout = Vulkan::pipelineLayout;
    pipelineInfo.renderPass = Vulkan::renderPass;
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
