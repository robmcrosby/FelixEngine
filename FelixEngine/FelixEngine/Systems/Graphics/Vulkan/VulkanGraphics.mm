//
//  VulkanGraphics.mm
//  FelixEngine
//
//  Created by Robert Crosby on 5/3/18.
//  Copyright © 2018 Robert Crosby. All rights reserved.
//

#include "VulkanGraphics.h"

#import "MetalView.h"

#include "Vulkan.h"
#include <vulkan/vulkan_ios.h>

#include "VulkanFrameBuffer.h"
#include "VulkanShaderProgram.h"
#include "VulkanVertexMesh.h"
#include "VulkanUniformBuffer.h"
#include "VulkanTextureBuffer.h"


using namespace std;
using namespace fx;

VulkanGraphics::VulkanGraphics() {
  _layers.push_back("MoltenVK");
  _extensions.push_back("VK_MVK_moltenvk");
}

VulkanGraphics::~VulkanGraphics() {
  
}

bool VulkanGraphics::initalize(UIView *view) {
  bool success = true;
  
  // Create an Embeded Metal View
  CGFloat viewWidth = view.frame.size.width;
  CGFloat viewHeight = view.frame.size.height;
  CGRect frame = CGRectMake(0, 0, viewWidth, viewHeight);
  MetalView *mtlView = [[MetalView alloc] initWithFrame:frame];
  [view addSubview:mtlView];
  
  float width = viewWidth;
  float height = viewHeight;
  
  assert(createInstance());
  assert(Vulkan::setDefaultPhysicalDevice());
  assert(Vulkan::setDefaultQueueFamily());
  assert(createSwapChain(mtlView));
  assert(Vulkan::createLogicalDevice());
  assert(Vulkan::initDeviceQueue(width, height));
  
  assert(createRenderPass());
  assert(createPipeline());
  assert(createFrameBuffers());
  assert(createCommandPool());
  assert(createCommandBuffers());
  assert(createSemaphores());
  
  return success;
}

FrameBufferPtr VulkanGraphics::createFrameBuffer() {
  shared_ptr<VulkanFrameBuffer> buffer = make_shared<VulkanFrameBuffer>();
  return buffer;
}

ShaderProgramPtr VulkanGraphics::createShaderProgram() {
  shared_ptr<VulkanShaderProgram> shader = make_shared<VulkanShaderProgram>();
  return shader;
}

VertexMeshPtr VulkanGraphics::createVertexMesh() {
  shared_ptr<VulkanVertexMesh> mesh = make_shared<VulkanVertexMesh>();
  return mesh;
}

UniformBufferPtr VulkanGraphics::createUniformBuffer() {
  shared_ptr<VulkanUniformBuffer> buffer = make_shared<VulkanUniformBuffer>();
  return buffer;
}

TextureBufferPtr VulkanGraphics::createTextureBuffer() {
  shared_ptr<VulkanTextureBuffer> texture = make_shared<VulkanTextureBuffer>();
  return texture;
}

void VulkanGraphics::nextFrame() {
  
}

void VulkanGraphics::addTask(const GraphicTask &task) {
  
}

void VulkanGraphics::presentFrame() {
  
}

bool VulkanGraphics::createInstance() {
  // Provide Application Information
  VkApplicationInfo appInfo;
  appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pApplicationName = "Felix Application";
  appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.pEngineName = "Felix Engine";
  appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.apiVersion = VK_API_VERSION_1_0;
  
  return Vulkan::createInstance(appInfo, _layers, _extensions);
}

bool VulkanGraphics::createSwapChain(UIView *view) {
  VkIOSSurfaceCreateInfoMVK createInfo = {};
  createInfo.sType = VK_STRUCTURE_TYPE_IOS_SURFACE_CREATE_INFO_MVK;
  createInfo.pNext = NULL;
  createInfo.flags = 0;
  createInfo.pView = (__bridge const void*)view;
  
  if (vkCreateIOSSurfaceMVK(Vulkan::instance, &createInfo, nullptr, &Vulkan::surface) != VK_SUCCESS) {
    cerr << "Error creating Vulkan Surface" << endl;
    return false;
  }
  
  VkBool32 supportsPresent;
  vkGetPhysicalDeviceSurfaceSupportKHR(Vulkan::physicalDevice, Vulkan::queueFamilyIndex, Vulkan::surface, &supportsPresent);
  if (!supportsPresent) {
    cerr << "Present Surface not supported with selected Queue Family" << endl;
    return false;
  }
  
  vector<VkSurfaceFormatKHR> formats = Vulkan::getPhysicalDeviceSurfaceFormats(Vulkan::physicalDevice, Vulkan::surface);
  assert(formats.size() > 0);
  if (formats.size() == 1 && formats[0].format == VK_FORMAT_UNDEFINED)
    Vulkan::surfaceFormat = VK_FORMAT_B8G8R8A8_UNORM;
  else
    Vulkan::surfaceFormat = formats[0].format;
  
  return true;
}

bool VulkanGraphics::createRenderPass() {
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
  
  // Define subpass using the Color Attachment Refrence
  VkSubpassDescription subpass = {};
  subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass.colorAttachmentCount = 1;
  subpass.pColorAttachments = &colorAttachmentRef;
  
  // Create the Render Pass from the Color Attachment and Subpass
  VkRenderPassCreateInfo renderPassInfo = {};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  renderPassInfo.attachmentCount = 1;
  renderPassInfo.pAttachments = &colorAttachment;
  renderPassInfo.subpassCount = 1;
  renderPassInfo.pSubpasses = &subpass;
  
  // Create the Render Pass
  if (vkCreateRenderPass(Vulkan::device, &renderPassInfo, nullptr, &Vulkan::renderPass) != VK_SUCCESS) {
    cerr << "Error Creating Render Pass" << endl;
    return false;
  }
  return true;
}

bool VulkanGraphics::createPipeline() {
  // Load the Shader Modules
//  VkShaderModule vertShaderModule = Vulkan::createShaderModule("vert.spv");
//  VkShaderModule fragShaderModule = Vulkan::createShaderModule("frag.spv");
  
//  // Create Vertex Shader Stage Info
//  VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
//  vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
//  vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
//  vertShaderStageInfo.module = vertShaderModule;
//  vertShaderStageInfo.pName = "main";
//
//  // Create fragment Shader Stage Info
//  VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
//  fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
//  fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
//  fragShaderStageInfo.module = fragShaderModule;
//  fragShaderStageInfo.pName = "main";
//
//  // Create Array of Shader Stage Infos
//  VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};
//
//
//  // Define the Vertex Input Info
//  VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
//  vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
//  vertexInputInfo.vertexBindingDescriptionCount = 0;
//  vertexInputInfo.pVertexBindingDescriptions = nullptr; // Optional
//  vertexInputInfo.vertexAttributeDescriptionCount = 0;
//  vertexInputInfo.pVertexAttributeDescriptions = nullptr; // Optional
//
//
//  // Define to Draw Triangle Prmitives
//  VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
//  inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
//  inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
//  inputAssembly.primitiveRestartEnable = VK_FALSE;
//
//
//  // Define the viewport
//  VkViewport viewport = {};
//  viewport.x = 0.0f;
//  viewport.y = 0.0f;
//  viewport.width = (float)swapChainExtent.width;
//  viewport.height = (float)swapChainExtent.height;
//  viewport.minDepth = 0.0f;
//  viewport.maxDepth = 1.0f;
//
//  // Define the scissor region
//  VkRect2D scissor = {};
//  scissor.offset = {0, 0};
//  scissor.extent = swapChainExtent;
//
//  // Define the Viewport State Info
//  VkPipelineViewportStateCreateInfo viewportState = {};
//  viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
//  viewportState.viewportCount = 1;
//  viewportState.pViewports = &viewport;
//  viewportState.scissorCount = 1;
//  viewportState.pScissors = &scissor;
//
//
//  // Define the Rasterization State Info
//  VkPipelineRasterizationStateCreateInfo rasterizer = {};
//  rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
//  rasterizer.depthClampEnable = VK_FALSE;
//  rasterizer.rasterizerDiscardEnable = VK_FALSE;
//  rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
//  rasterizer.lineWidth = 1.0f;
//  rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
//  rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
//  rasterizer.depthBiasEnable = VK_FALSE;
//  rasterizer.depthBiasConstantFactor = 0.0f; // Optional
//  rasterizer.depthBiasClamp = 0.0f; // Optional
//  rasterizer.depthBiasSlopeFactor = 0.0f; // Optional
//
//
//  // Define Multisampling State Info
//  VkPipelineMultisampleStateCreateInfo multisampling = {};
//  multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
//  multisampling.sampleShadingEnable = VK_FALSE;
//  multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
//  multisampling.minSampleShading = 1.0f; // Optional
//  multisampling.pSampleMask = nullptr; // Optional
//  multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
//  multisampling.alphaToOneEnable = VK_FALSE; // Optional
//
//
//  // Define Color Blending State Info
//  VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
//  colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
//  colorBlendAttachment.blendEnable = VK_FALSE;
//  colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
//  colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
//  colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
//  colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
//  colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
//  colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional
//
//
//  // Define the Blend Color Info
//  VkPipelineColorBlendStateCreateInfo colorBlending = {};
//  colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
//  colorBlending.logicOpEnable = VK_FALSE;
//  colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
//  colorBlending.attachmentCount = 1;
//  colorBlending.pAttachments = &colorBlendAttachment;
//  colorBlending.blendConstants[0] = 0.0f; // Optional
//  colorBlending.blendConstants[1] = 0.0f; // Optional
//  colorBlending.blendConstants[2] = 0.0f; // Optional
//  colorBlending.blendConstants[3] = 0.0f; // Optional
//
//
//  // Define Pipeline Layout for Constant Values
//  VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
//  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
//  pipelineLayoutInfo.setLayoutCount = 0; // Optional
//  pipelineLayoutInfo.pSetLayouts = nullptr; // Optional
//  pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
//  pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional
//
//  // Create the Pipeline Layout
//  if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
//    cerr << "Error creating Pipeline Layout" << endl;
//    assert(false);
//  }
//
//
//  // Define the Pipeline info
//  VkGraphicsPipelineCreateInfo pipelineInfo = {};
//  pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
//  pipelineInfo.stageCount = 2;
//  pipelineInfo.pStages = shaderStages;
//  pipelineInfo.pVertexInputState = &vertexInputInfo;
//  pipelineInfo.pInputAssemblyState = &inputAssembly;
//  pipelineInfo.pViewportState = &viewportState;
//  pipelineInfo.pRasterizationState = &rasterizer;
//  pipelineInfo.pMultisampleState = &multisampling;
//  pipelineInfo.pDepthStencilState = nullptr; // Optional
//  pipelineInfo.pColorBlendState = &colorBlending;
//  pipelineInfo.pDynamicState = nullptr; // Optional
//  pipelineInfo.layout = pipelineLayout;
//  pipelineInfo.renderPass = renderPass;
//  pipelineInfo.subpass = 0;
//  pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
//  pipelineInfo.basePipelineIndex = -1; // Optional
//
//  // Create the Graphics Pipeline
//  if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS) {
//    cerr << "Error creating Graphics Pipeline" << endl;
//    assert(false);
//  }
//
//  // Cleanup the Shader Modules
//  vkDestroyShaderModule(device, fragShaderModule, nullptr);
//  vkDestroyShaderModule(device, vertShaderModule, nullptr);
  return true;
}

bool VulkanGraphics::createFrameBuffers() {
  return true;
}

bool VulkanGraphics::createCommandPool() {
  return true;
}

bool VulkanGraphics::createCommandBuffers() {
  return true;
}

bool VulkanGraphics::createSemaphores() {
  return true;
}
