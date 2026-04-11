//
//  GuiContext.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 4/10/26.
//

#include "GuiContext.hpp"
#include "GpuContext.hpp"
#include "GuiEventTypes.hpp"

#include "VulkanIncludes.hpp"

#include "Tag.hpp"
#include "Parent.hpp"

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_vulkan.h"
#include <SDL3/SDL_vulkan.h>


using namespace std;
using namespace glm;
namespace Felix {

static void check_vk_result(VkResult err) {
  if (err == VK_SUCCESS)
    return;
  fprintf(stderr, "[vulkan] Error: VkResult = %d\n", err);
  if (err < 0)
    abort();
}

static vec2 ImVec2_to_vec2(const ImVec2& v) {
  return vec2(v.x, v.y);
}

static ImVec2 vec2_to_ImVec2(const vec2& v) {
  return ImVec2(v.x, v.y);
}


GuiContext::GuiContext(): mDevice(VK_NULL_HANDLE), mDescriptorPool(VK_NULL_HANDLE) {}

GuiContext::~GuiContext() {
  destory();
}

GuiContextPtr GuiContext::create() {
  GuiContextPtr context = make_shared<GuiContext>();
  return context;
}

bool GuiContext::init(GpuContext& gpuContext) {
  auto& swapChain = *gpuContext.getSwapChain();
  auto& queue = *gpuContext.getQueue();
  auto& renderPass = *gpuContext.getRenderPass();
  
  VkInstance vkInstance = VulkanInstance::Get().getVkInstance();
  float main_scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
  
  VulkanDevice& device = queue.getDevice();
  SDL_Window *window = swapChain.getWindow();
  mDevice = device.getVkDevice();
  
  if (!setupDescriptorPool()) {
    cerr << "Error creating Descriptor Pool for ImGui" << endl;
    return false;
  }
  
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO(); //(void)io;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableSetMousePos;
  //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
  //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
  
  // Setup Dear ImGui style
  ImGui::StyleColorsDark();
  //ImGui::StyleColorsLight();
  //ImGui::StyleColorsClassic();
  
  // Setup scaling
  ImGuiStyle& style = ImGui::GetStyle();
  style.ScaleAllSizes(main_scale);        // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
  style.FontScaleDpi = main_scale;        // Set initial font scale. (in docking branch: using io.ConfigDpiScaleFonts=true automatically overrides this for every window depending on the current monitor)
  
  // Set Custom Font
  style.FontSizeBase = 30.0f;
  //io.Fonts->AddFontFromFileTTF("Roboto.ttf");
  io.Fonts->AddFontFromFileTTF("ChangaOne-Regular.ttf");
  
  // Setup Platform/Renderer backends
  ImGui_ImplSDL3_InitForVulkan(window);
  ImGui_ImplVulkan_InitInfo init_info = {};
  //init_info.ApiVersion = VK_API_VERSION_1_3; // Pass in your value of VkApplicationInfo::apiVersion, otherwise will default to header version.
  init_info.Instance = vkInstance;
  init_info.PhysicalDevice = device.getVkPhysicalDevice();
  init_info.Device = device.getVkDevice();
  init_info.QueueFamily = queue.familyIndex();
  init_info.Queue = queue.getVkQueue();
  init_info.PipelineCache = VK_NULL_HANDLE;
  init_info.DescriptorPool = mDescriptorPool;
  init_info.MinImageCount = swapChain.minImageCount();
  init_info.ImageCount = swapChain.imageCount();
  init_info.Allocator = nullptr;
  init_info.PipelineInfoMain.RenderPass = renderPass.getVkRenderPass();
  init_info.PipelineInfoMain.Subpass = 0;
  init_info.PipelineInfoMain.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
  init_info.CheckVkResultFn = check_vk_result;
  ImGui_ImplVulkan_Init(&init_info);
  
  return true;
}

ImDrawData* GuiContext::drawUI(Scene& scene) const {
  ImGui_ImplVulkan_NewFrame();
  ImGui_ImplSDL3_NewFrame();
  ImGui::NewFrame();
  
  auto panels = scene.registry().view<GuiPanel, GuiBounds, Tag>();
  auto widgets = scene.registry().view<GuiWidget, Parent>();
  for (auto [panel, panelComp, boundsComp, tagComp] : panels.each()) {
    if (panelComp.visible) {
      beginPanel(panelComp, tagComp.tag);
      for (auto [widget, widgetComp, widgetParent] : widgets.each()) {
        if (widgetParent.parent == panel) {
          addWidget({widget, &scene}, widgetComp);
        }
      }
      endPanel(boundsComp);
    }
  }
  ImGui::Render();
  return ImGui::GetDrawData();
}

void GuiContext::draw(VkCommandBuffer cmd, Scene& scene) const {
  ImDrawData* drawData = drawUI(scene);
  ImGui_ImplVulkan_RenderDrawData(drawData, cmd);
}

/*
 ImDrawData* GuiContext::drawUI() const {
 ImGui_ImplVulkan_NewFrame();
 ImGui_ImplSDL3_NewFrame();
 ImGui::NewFrame();
 {
 ImGui::SetNextWindowPos(ImVec2(100.0f, 200.0f));
 
 bool open = true;
 ImGui::Begin("Hello, world!", &open, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground);
 ImGui::Text("This is some useful text.");
 ImGui::Checkbox("Example Checkbox", &mCheckboxSelection);
 ImGui::SliderFloat("float", &mFloatValue, 0.0f, 1.0f);
 
 ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f);
 ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(16.0f, 12.0f));
 if (ImGui::Button("Button")) {
 ++mButtonCount;
 }
 ImGui::PopStyleVar(2);
 
 //ImGui::SameLine();
 ImGui::Text("counter = %d", mButtonCount);
 ImGui::End();
 }
 ImGui::Render();
 
 return ImGui::GetDrawData();
 }
 */

bool GuiContext::handle(SDL_Event *event) {
  return ImGui_ImplSDL3_ProcessEvent(event);
}

bool GuiContext::setupDescriptorPool() {
  if (mDevice == VK_NULL_HANDLE) {
    return false;
  }
  
  VkDescriptorPoolSize pool_sizes[] = {
    { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, IMGUI_IMPL_VULKAN_MINIMUM_IMAGE_SAMPLER_POOL_SIZE },
  };
  
  VkDescriptorPoolCreateInfo pool_info = {};
  pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
  pool_info.maxSets = 0;
  pool_info.poolSizeCount = (uint32_t)IM_COUNTOF(pool_sizes);
  pool_info.pPoolSizes = pool_sizes;
  for (VkDescriptorPoolSize& pool_size : pool_sizes)
    pool_info.maxSets += pool_size.descriptorCount;
  
  return vkCreateDescriptorPool(mDevice, &pool_info, nullptr, &mDescriptorPool) == VK_SUCCESS;
}

void GuiContext::destory() {
  if (mDevice == VK_NULL_HANDLE)
    return;
  vkDeviceWaitIdle(mDevice);
  
  // Shutdown and Destroy ImGui Context
  ImGui_ImplVulkan_Shutdown();
  ImGui_ImplSDL3_Shutdown();
  ImGui::DestroyContext();
  
  // Cleanup Descriptor Pool and set VK handles to Null
  vkDestroyDescriptorPool(mDevice, mDescriptorPool, nullptr);
  mDescriptorPool = VK_NULL_HANDLE;
  mDevice = VK_NULL_HANDLE;
}

Entity GuiContext::addGuiDrawToPass(Entity pass) {
  auto guiItem = pass.addChild("DrawGui");
  
  auto& draw = guiItem.get<GpuDraw>();
  draw.guiContext = this;
  draw.order = INT_MAX;
  
  return guiItem;
}

void GuiContext::setNextPanel(const GuiPanel &panel) const {
  vec2 pivot(0.0f, 0.0f);
  pivot.x = panel.alignX == GuiAlignCenter ? 0.5f : panel.alignX == GuiAlignRight ? 1.0f : 0.0f;
  pivot.y = panel.alignY == GuiAlignCenter ? 0.5f : panel.alignY == GuiAlignBottom ? 1.0f : 0.0f;
  
  vec2 size = ImVec2_to_vec2(ImGui::GetMainViewport()->Size);
  vec2 position = size * pivot + panel.offset;
  
  ImGuiCond cond = panel.moveable ? ImGuiCond_Once : ImGuiCond_Always;
  ImGui::SetNextWindowPos(vec2_to_ImVec2(position), cond, vec2_to_ImVec2(pivot));
}

void GuiContext::beginPanel(GuiPanel& panel, const string& name) const {
  setNextPanel(panel);
  
  int panelFlags = 0;
  panelFlags |= panel.moveable ? 0 : ImGuiWindowFlags_NoMove;
  panelFlags |= panel.background ? 0 : ImGuiWindowFlags_NoBackground;
  panelFlags |= panel.collapsable ? 0 : ImGuiWindowFlags_NoDecoration;
  
  bool open = !panel.collapsed;
  ImGui::Begin(name.c_str(), &open, panelFlags);
  panel.collapsed = !open;
}

void GuiContext::endPanel(GuiBounds& bounds) const {
  // Capture Panel position and size
  bounds.position = ImVec2_to_vec2(ImGui::GetWindowPos());
  bounds.size = ImVec2_to_vec2(ImGui::GetWindowSize());
  
  // End the Panel
  ImGui::End();
}

void GuiContext::addWidget(Entity entity, GuiWidget& widget) const {
  switch (widget.type) {
    case GuiLabel:
      ImGui::Text("%s", widget.text.c_str());
      break;
    case GuiButton:
      if (ImGui::Button(widget.text.c_str()))
        entity.enqueue(ButtonPressEvent);
      break;
    default:
      break;
  }
}

} /* Felix */
