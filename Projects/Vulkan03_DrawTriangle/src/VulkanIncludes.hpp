#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <vk_mem_alloc.h>


#ifndef VulkanIncludes_hpp
#define VulkanIncludes_hpp


typedef const char* CString;
typedef std::vector<CString> CStrings;
typedef std::vector<std::string> Strings;
typedef const std::string& StringRef;

typedef std::vector<uint32_t> SPIRVCode;
typedef const SPIRVCode& SPIRVCodeRef;

typedef std::vector<VkLayerProperties>            VulkanLayerProperties;
typedef std::vector<VkExtensionProperties>        VulkanExtensionProperties;
typedef std::vector<VkQueueFamilyProperties>      VulkanQueueFamilies;
typedef std::vector<VkDescriptorSet>              VkDescriptorSets;
typedef std::vector<VkDescriptorSetLayout>        VkDescriptorSetLayouts;
typedef std::vector<VkDescriptorSetLayoutBinding> VkDescriptorSetLayoutBindings;
typedef std::vector<VkAttachmentReference>        VkAttachmentReferences;
typedef std::vector<VkAttachmentDescription>      VkAttachmentDescriptions;
typedef std::vector<VkDescriptorSetLayout>        VkDescriptorSetLayouts;


struct LayoutBinding {
  std::string name;
  VkDescriptorSetLayoutBinding binding;
};
typedef std::vector<LayoutBinding>    LayoutBindingSet;
typedef std::vector<LayoutBindingSet> LayoutBindingSets;


class VulkanDevice;
typedef std::shared_ptr<VulkanDevice> VulkanDevicePtr;
typedef std::vector<VulkanDevicePtr> VulkanDevices;

class VulkanBuffer;
typedef std::shared_ptr<VulkanBuffer> VulkanBufferPtr;

class VulkanImage;
typedef std::shared_ptr<VulkanImage> VulkanImagePtr;
typedef std::vector<VulkanImagePtr> VulkanImages;

class VulkanLayout;
typedef std::shared_ptr<VulkanLayout> VulkanLayoutPtr;
typedef std::vector<VulkanLayoutPtr> VulkanLayouts;

class VulkanLayoutSet;
typedef std::shared_ptr<VulkanLayoutSet> VulkanLayoutSetPtr;

class VulkanFrameBuffer;
typedef std::shared_ptr<VulkanFrameBuffer> VulkanFrameBufferPtr;

class VulkanRenderPass;
typedef std::shared_ptr<VulkanRenderPass> VulkanRenderPassPtr;

class VulkanShader;
typedef std::shared_ptr<VulkanShader> VulkanShaderPtr;

class VulkanPipeline;
typedef std::shared_ptr<VulkanPipeline> VulkanPipelinePtr;

class VulkanQueue;
typedef std::shared_ptr<VulkanQueue> VulkanQueuePtr;
typedef std::vector<VulkanQueuePtr> VulkanQueues;

class VulkanCommand;
typedef std::shared_ptr<VulkanCommand> VulkanCommandPtr;
typedef std::vector<VulkanCommandPtr> VulkanCommands;

struct BufferLayoutBinding {
  VulkanBufferPtr buffer;
  VkDescriptorSetLayoutBinding binding;
  VkDescriptorBufferInfo bufferInfo;
};
typedef std::vector<BufferLayoutBinding> BufferLayoutBindings;

struct TextureLayoutBinding {
  VulkanImagePtr image;
  VkDescriptorSetLayoutBinding binding;
  VkDescriptorImageInfo imageInfo;
};
typedef std::vector<TextureLayoutBinding> TextureLayoutBindings;


class VulkanInstance {
private:
  VkInstance mVkInstance;

  std::string mApplicationName;
  std::string mEngineName;

  uint32_t mApplicationVersion;
  uint32_t mEngineVersion;
  uint32_t mApiVersion;

  VulkanLayerProperties     mAvailableLayers;
  VulkanExtensionProperties mAvailableExentions;

  bool mValidationEnabled;
  CStrings mEnabledLayers;
  CStrings mEnabledExtensions;

  bool mDebugMessengerEnabled;
  VkDebugUtilsMessengerEXT mDebugMessenger;

  VulkanDevices mDevices;

private:
  VulkanInstance();
  ~VulkanInstance();

public:
  static VulkanInstance& Get();

  bool init();
  void destroy();

  void setApplicationName(StringRef name);
  void setEngineName(StringRef name);

  void setApplicationVersion(int major, int minor, int patch);
  void setEngineVersion(int major, int minor, int patch);

  void enableValidation();
  bool addValidationLayer(StringRef layer);
  bool addExtension(StringRef extension);

  const CStrings& getEnabledLayers() const {return mEnabledLayers;}
  const CStrings& getEnabledExtensions() const {return mEnabledExtensions;}

  VkInstance getVkInstance() const {return mVkInstance;}

  VulkanDevicePtr pickDevice() const;

  friend std::ostream& operator<<(std::ostream& os, const VulkanInstance& instance);
  std::ostream& print(std::ostream& os) const;

private:
  bool createVkInstance();

  bool setupDebugMessenger();
  void destroyDebugMessenger();
  void initDebugMessengerInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

  bool loadDevices();
  void clearDevices();
};


class VulkanDevice {
private:
  VkPhysicalDevice mVkPhysicalDevice;
  VkDevice         mVkDevice;

  VmaAllocator mVmaAllocator;

  VkPhysicalDeviceProperties mProperties;
  VkPhysicalDeviceFeatures   mFeatures;
  CStrings                   mEnabledExtensions;
  VulkanQueueFamilies        mQueueFamilies;
  std::vector<uint32_t>      mQueueFamilyCounts;

  VkPhysicalDeviceMemoryProperties mMemoryProperties;

  VulkanQueues mQueues;

public:
  VulkanDevice(VkPhysicalDevice device);
  ~VulkanDevice();

  bool init();
  void destroy();

  std::string name() const;
  std::string type() const;

  VkPhysicalDeviceType deviceType() const {return mProperties.deviceType;}
  VkDevice getVkDevice() const {return mVkDevice;}
  VmaAllocator getVmaAllocator() const {return mVmaAllocator;}

  bool addExtension(StringRef extension);
  const CStrings& enabledExtensions() const {return mEnabledExtensions;}

  VulkanQueuePtr createQueue(VkQueueFlags flags);

  VulkanBufferPtr      createBuffer();
  VulkanImagePtr       createImage();
  VulkanLayoutPtr      createLayout();
  VulkanLayoutSetPtr   createLayoutSet();
  VulkanFrameBufferPtr createFrameBuffer();
  VulkanRenderPassPtr  createRenderPass();
  VulkanShaderPtr      createShader(VkShaderStageFlagBits stage);
  VulkanPipelinePtr    createPipeline();

  bool getVkMemoryType(uint32_t& index, VkMemoryType& type, VkMemoryPropertyFlags properties);

  friend std::ostream& operator<<(std::ostream& os, const VulkanDevicePtr& device);
  std::ostream& print(std::ostream& os) const;
  std::ostream& printQueueFlags(std::ostream& os, VkQueueFlags flags) const;
  std::ostream& printHeapFlags(std::ostream& os, VkMemoryHeapFlags flags) const;
  std::ostream& printMemoryFlags(std::ostream& os, VkMemoryPropertyFlags flags) const;

private:
  bool initVmaAllocator();
  void destoryVmaAllocator();

  bool pickQueueFamily(VkQueueFlags flags, uint32_t& familyIndex, uint32_t& queueIndex);
  void clearQueues();
};


class VulkanBuffer {
private:
  VulkanDevice* mDevice;
  VkBuffer      mVkBuffer;
  VmaAllocation mVmaAllocation;

  VkBufferUsageFlags        mVkBufferUsageFlags;
  VmaMemoryUsage            mVmaMemoryUsage;
  VmaAllocationCreateFlags  mVmaCreateFlags;

public:
  VulkanBuffer(VulkanDevice* device);
  ~VulkanBuffer();

  void setUsage(VkBufferUsageFlags flags);
  void setCreateFlags(VmaAllocationCreateFlags flags);

  bool alloc(VkDeviceSize size);
  void destroy();

  VkBuffer getVkBuffer() const {return mVkBuffer;}

  VmaAllocationInfo getVmaAllocationInfo() const;
  void* data();
};


class VulkanImage {
private:
  VulkanDevice* mDevice;
  VkFormat      mVkFormat;
  VkImage       mVkImage;
  VkImageView   mVkImageView;
  VmaAllocation mVmaAllocation;
  uint32_t      mWidth;
  uint32_t      mHeight;

  VkImageUsageFlags         mVkImageUsageFlags;
  VmaMemoryUsage            mVmaMemoryUsage;
  VmaAllocationCreateFlags  mVmaCreateFlags;

  VkImageLayout        mCurImageLayout;
  VkPipelineStageFlags mCurStageMask;
  VkAccessFlags        mCurAccessMask;

public:
  VulkanImage(VulkanDevice* device);
  ~VulkanImage();

  void setUsage(VkImageUsageFlags flags);
  void setCreateFlags(VmaAllocationCreateFlags flags);

  bool alloc(uint32_t width, uint32_t height);
  void destroy();

  VkImage getVkImage() const {return mVkImage;}
  VkFormat getVkFormat() const {return mVkFormat;}
  VkImageView getVkImageView();

  VmaAllocationInfo getVmaAllocationInfo() const;
  void* data();

  uint32_t getWidth() const {return mWidth;}
  uint32_t getHeight() const {return mHeight;}

  void transition(
    VkCommandBuffer commandBuffer,
    VkImageLayout   newImageLayout,
    VkAccessFlags   dstAccessMask,
    VkPipelineStageFlags dstStageMask
  );
  void copyFromBuffer(VkCommandBuffer commandBuffer, VulkanBufferPtr buffer);
  void copyToBuffer(VkCommandBuffer commandBuffer, VulkanBufferPtr buffer);

private:
  VkImageView createImageView(
    VkImage  image,
    VkFormat format,
    VkImageAspectFlags aspectFlags,
    int32_t mipLevels
  ) const;
};


class VulkanLayout {
private:
  VulkanDevice*         mDevice;
  BufferLayoutBindings  mBufferLayoutBindings;
  TextureLayoutBindings mTextureLayoutBindings;
  VkDescriptorSetLayout mVkDescriptorSetLayout;
  VkDescriptorPool      mVkDescriptorPool;
  VkDescriptorSet       mVkDescriptorSet;

public:
  VulkanLayout(VulkanDevice* device);
  ~VulkanLayout();

  void setBuffer(VulkanBufferPtr buffer, uint32_t binding);
  void setTexture(VulkanImagePtr image, uint32_t binding);
  void update();

  VkDescriptorSetLayout getVkDescriptorSetLayout();
  VkDescriptorPool      getVkDescriptorPool();
  VkDescriptorSet       getVkDescriptorSet();

  void destroy();

private:
  void freeDescriptorSet();
  void destroyDescriptorPool();
  void destroyDescriptorSetLayout();
};


class VulkanLayoutSet {
private:
  VulkanDevice*    mDevice;
  VulkanLayouts    mLayouts;
  VkPipelineLayout mVkPipelineLayout;

public:
  VulkanLayoutSet(VulkanDevice* device);
  ~VulkanLayoutSet();

  VulkanLayoutPtr at(int index);

  void getVkDescriptorSetLayouts(VkDescriptorSetLayouts& setLayouts);
  void getVkDescriptorSets(VkDescriptorSets& descriptorSets);
  VkPipelineLayout getVkPipelineLayout();

  void destroy();

private:
  VkPipelineLayout createVkPipelineLayout(VkDescriptorSetLayouts& setLayouts);
  void destroyPipelineLayout();
};


class VulkanFrameBuffer {
private:
  VulkanDevice* mDevice;
  VkExtent2D    mExtent;
  VulkanImages  mColorAttachments;
  VkFramebuffer mVkFramebuffer;

public:
  VulkanFrameBuffer(VulkanDevice* device);
  ~VulkanFrameBuffer();

  void addColorAttachment(VulkanImagePtr image);

  VkFramebuffer getVkFramebuffer(VkRenderPass renderPass);

  VkExtent2D getExtent() const {return mExtent;}

  uint32_t getColorCount() const {return mColorAttachments.size();}
  void getVkAttachmentReferences(VkAttachmentReferences& references);
  void getVkAttachmentDescriptions(VkAttachmentDescriptions& descriptions);

  void destroy();

private:
  VkFramebuffer createVkFramebuffer(VkRenderPass renderPass);
};


class VulkanRenderPass {
private:
  VulkanDevice*        mDevice;
  VkRenderPass         mVkRenderPass;
  VulkanFrameBufferPtr mFramebuffer;

public:
  VulkanRenderPass(VulkanDevice* device);
  ~VulkanRenderPass();

  void setFramebuffer(VulkanFrameBufferPtr framebuffer);

  VkRenderPass  getVkRenderPass();
  VkFramebuffer getVkFramebuffer();

  VkExtent2D getExtent()     const;
  VkViewport getViewport()   const;
  VkRect2D   getScissor()    const;
  uint32_t   getColorCount() const;

  void destroy();

private:
  VkRenderPass createVkRenderPass();
};


class VulkanShader {
private:
  VulkanDevice*         mDevice;
  VkShaderModule        mVkShaderModule;
  std::string           mEntryFunction;
  LayoutBindingSets     mLayoutBindingSets;
  VkShaderStageFlagBits mVkShaderStage;

public:
  VulkanShader(VulkanDevice* device, VkShaderStageFlagBits stage);
  ~VulkanShader();

  bool load(StringRef filename, StringRef entry = "main");
  bool load(SPIRVCodeRef code, StringRef entry = "main");

  VkShaderModule getVkShaderModule() const {return mVkShaderModule;}
  CString getEntryFunction() const {return mEntryFunction.c_str();}
  LayoutBindingSets& getLayoutBindingSets() {return mLayoutBindingSets;}

  VkPipelineShaderStageCreateInfo getVkPipelineShaderStageCreateInfo();

  void destroy();

private:
  void reflect(SPIRVCodeRef code, StringRef entry);
  static bool readSPIRV(SPIRVCode& code, StringRef filename);
};


class VulkanPipeline {
private:
  VulkanDevice*     mDevice;
  VkPipeline        mVkPipeline;
  VkPipelineLayout  mVkPipelineLayout;

  VulkanShaderPtr mKernalShader;
  VulkanShaderPtr mVertexShader;
  VulkanShaderPtr mFragmentShader;

  VkDescriptorSetLayouts mVkDescriptorSetLayouts;

public:
  VulkanPipeline(VulkanDevice* device);
  ~VulkanPipeline();

  bool setKernalShader(StringRef filename, StringRef entry = "main");
  bool setVertexShader(StringRef filename, StringRef entry = "main");
  bool setFragmentShader(StringRef filename, StringRef entry = "main");
  void clearShaders();

  bool isCompute() const {return mKernalShader != nullptr;}
  bool isGraphics() const {return mVertexShader && mFragmentShader;}

  VkPipeline getVkPipeline();
  VkPipeline getVkPipeline(VkDescriptorSetLayouts setLayouts);
  VkPipeline getVkPipeline(VulkanRenderPassPtr renderPass);

  VkPipelineLayout getVkPipelineLayout();
  VkPipelineLayout getVkPipelineLayout(VkDescriptorSetLayouts setLayouts);

  void destroy();

private:
  VkPipeline createComputePipeline(VkDescriptorSetLayouts setLayouts);
  VkPipeline createGraphicsPipeline(VulkanRenderPassPtr renderPass);

  VkDescriptorSetLayouts& getVkDescriptorSetLayouts();
  void clearVkDescriptorSetLayouts();

  void getLayoutBindingSets(LayoutBindingSets& layoutBindingSets);

  VkPipelineLayout createVkPipelineLayout(const VkDescriptorSetLayouts& setLayouts);

  void destroyPipelineLayout();
  void destroyPipeline();
};


class VulkanQueue {
private:
  VulkanDevice* mDevice;
  VkQueueFlags  mVkQueueFlags;
  uint32_t      mFamilyIndex;
  uint32_t      mQueueIndex;
  VkQueue       mVkQueue;
  VkCommandPool mVkCommandPool;

public:
  VulkanQueue(VulkanDevice* device, VkQueueFlags flags, uint32_t familyIndex, uint32_t queueIndex);
  ~VulkanQueue();

  uint32_t familyIndex() const {return mFamilyIndex;}
  uint32_t queueIndex() const {return mQueueIndex;}

  VulkanDevice* getDevice() const {return mDevice;}

  VkQueue getVkQueue() const {return mVkQueue;}
  VkCommandPool getVkCommandPool() const {return mVkCommandPool;}

  bool init();
  void destroy();

  void transition(
    VulkanImagePtr image,
    VkImageLayout newImageLayout,
    VkAccessFlags dstAccessMask,
    VkPipelineStageFlags dstStageMask
  );
  void copyBufferToImage(VulkanBufferPtr buffer, VulkanImagePtr image);
  void copyImageToBuffer(VulkanImagePtr image, VulkanBufferPtr buffer);

  VulkanCommandPtr createCommand();
  VulkanCommandPtr beginSingleCommand();

  void submitCommand(VulkanCommandPtr command);
  void waitIdle();

private:
  void destroyCommandPool();
};


class VulkanCommand {
private:
  VulkanQueue*    mQueue;
  VkCommandPool   mVkCommandPool;
  VkCommandBuffer mVkCommandBuffer;

public:
  VulkanCommand(VulkanQueue* queue);
  ~VulkanCommand();

  VkCommandBuffer getVkCommandBuffer() const {return mVkCommandBuffer;}

  bool begin();
  void end();
  void submit();
  void endSingle();

  void bind(VulkanPipelinePtr pipeline);
  void bind(VulkanLayoutSetPtr layoutSet);
  void bind(VulkanPipelinePtr pipeline, VulkanLayoutSetPtr layoutSet);
  void dispatch(uint32_t x, uint32_t y = 1, uint32_t z = 1);

  void beginRenderPass(VulkanRenderPassPtr renderPass);
  void endRenderPass();

  void bind(VulkanPipelinePtr pipeline, VulkanRenderPassPtr renderPass);
  void draw(uint32_t vertexCount, uint32_t instances = 1);

  void transition(
    VulkanImagePtr image,
    VkImageLayout newImageLayout,
    VkAccessFlags dstAccessMask,
    VkPipelineStageFlags dstStageMask
  );
  void copyBufferToImage(VulkanBufferPtr buffer, VulkanImagePtr image);
  void copyImageToBuffer(VulkanImagePtr image, VulkanBufferPtr buffer);

  bool alloc();
  void free();

  void destroy();
};


#endif /* VulkanIncludes_hpp */
