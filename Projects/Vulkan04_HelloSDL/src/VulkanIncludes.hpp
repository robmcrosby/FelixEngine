#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <vk_mem_alloc.h>


#ifndef VulkanIncludes_hpp
#define VulkanIncludes_hpp

#define MAX_FRAMES_IN_FLIGHT 2


class SDL_Window;

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
typedef std::vector<VkImageMemoryBarrier>         VkImageMemoryBarriers;
typedef std::vector<VkBuffer>                     VkBuffers;
typedef std::vector<VkImage>                      VkImages;
typedef std::vector<VkImageView>                  VkImageViews;
typedef std::vector<VkFramebuffer>                VkFramebuffers;
typedef std::vector<VkRenderPass>                 VkRenderPasses;
typedef std::vector<VkDeviceSize>                 VkDeviceSizes;
typedef std::vector<VkSurfaceFormatKHR>           VkSurfaceFormats;
typedef std::vector<VkPresentModeKHR>             VkPresentModes;
typedef std::vector<VkCommandBuffer>              VkCommandBuffers;
typedef std::vector<VkSemaphore>                  VkSemaphores;
typedef std::vector<VkFence>                      VkFences;
typedef std::vector<VmaAllocation>                VmaAllocations;

typedef std::vector<VkVertexInputBindingDescription>   VkVertexInputBindingDescriptions;
typedef std::vector<VkVertexInputAttributeDescription> VkVertexInputAttributeDescriptions;


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

class VulkanMesh;
typedef std::shared_ptr<VulkanMesh> VulkanMeshPtr;

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

class VulkanSwapChain;
typedef std::shared_ptr<VulkanSwapChain> VulkanSwapChainPtr;

class VulkanFrameSync;
typedef std::shared_ptr<VulkanFrameSync> VulkanFrameSyncPtr;

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
  VulkanExtensionProperties  mAvailableExtensions;
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

  VkInstance getVkInstance() const {return VulkanInstance::Get().getVkInstance();}
  VkPhysicalDevice getVkPhysicalDevice() const {return mVkPhysicalDevice;}
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
  VulkanMeshPtr        createMesh();
  VulkanFrameBufferPtr createFrameBuffer();
  VulkanRenderPassPtr  createRenderPass();
  VulkanShaderPtr      createShader(VkShaderStageFlagBits stage);
  VulkanPipelinePtr    createPipeline();
  VulkanSwapChainPtr   createSwapChain();
  VulkanFrameSyncPtr   createFrameSync();

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
  VkDeviceSize  mSize;

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

  void copyToBuffer(VkCommandBuffer commandBuffer, VulkanBufferPtr buffer);
  void copyToBuffer(VkCommandBuffer commandBuffer, VkBuffer dst);

  VkBuffer getVkBuffer() const {return mVkBuffer;}

  VmaAllocationInfo getVmaAllocationInfo() const;
  VkMemoryPropertyFlags getVkMemoryPropertyFlags() const;
  VkDescriptorType getVkDescriptorType() const;

  bool isHostVisible() const;

  void* data();
  VkDeviceSize size() const {return mSize;}

  bool load(VulkanQueuePtr queue, const void* data, VkDeviceSize size);

  template <typename T>
  bool load(VulkanQueuePtr queue, const std::vector<T>& src) {
    return load(queue, src.data(), src.size() * sizeof(T));
  }
};


class VulkanImage {
private:
  VulkanDevice*  mDevice;
  VkFormat       mVkFormat;
  VkImages       mVkImages;
  VmaAllocations mVmaAllocations;
  VkImageViews   mVkImageViews;
  uint32_t       mWidth;
  uint32_t       mHeight;

  VkImageUsageFlags         mVkImageUsageFlags;
  VmaMemoryUsage            mVmaMemoryUsage;
  VmaAllocationCreateFlags  mVmaCreateFlags;

  VkImageLayout        mCurImageLayout;
  VkPipelineStageFlags mCurStageMask;
  VkAccessFlags        mCurAccessMask;

  VkImageMemoryBarriers mActiveBerriers;

public:
  VulkanImage(VulkanDevice* device);
  ~VulkanImage();

  void setUsage(VkImageUsageFlags flags);
  void setCreateFlags(VmaAllocationCreateFlags flags);

  bool alloc(uint32_t width, uint32_t height, int frames = 1);
  void destroy();
  void clearImages();

  void setSwapImages(const VkImages& images, VkFormat format, uint32_t width, uint32_t height);

  VkImage     getVkImage(int index = 0) const {return mVkImages.at(index);}
  VkImageView getVkImageView(int index = 0) const {return mVkImageViews.at(index);}
  VkFormat    getVkFormat() const {return mVkFormat;}

  VmaAllocationInfo getVmaAllocationInfo(int index = 0) const;
  void* data(int index = 0);

  VkDeviceSize formatSize() const;
  VkDeviceSize size() const {return mWidth * mHeight * formatSize();}
  uint32_t width() const {return mWidth;}
  uint32_t height() const {return mHeight;}
  uint32_t frames() const {return static_cast<uint32_t>(mVkImages.size());}
  bool isSwapImage() const {return mVmaAllocations.empty() && !mVkImages.empty();}

  void transition(
    VkCommandBuffer      commandBuffer,
    VkImageLayout        newImageLayout,
    VkAccessFlags        dstAccessMask,
    VkPipelineStageFlags dstStageMask
  );
  void copyFromBuffer(VkCommandBuffer commandBuffer, VulkanBufferPtr buffer, int frame = 0);
  void copyToBuffer(VkCommandBuffer commandBuffer, VulkanBufferPtr buffer, int frame = 0);

private:
  bool allocVkImage(
    VkImage&       image,
    VmaAllocation& allocation,
    uint32_t       width,
    uint32_t       height
  ) const;
  VkImageView createImageView(
    VkImage            image,
    VkFormat           format,
    VkImageAspectFlags aspectFlags,
    int32_t            mipLevels
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

  bool setStorage(
    VulkanQueuePtr queue,
    uint32_t binding,
    const void* data,
    VkDeviceSize size
  );
  bool setUniform(
    VulkanQueuePtr queue,
    uint32_t binding,
    const void* data,
    VkDeviceSize size
  );

  template <typename T>
  bool setStorage(
    VulkanQueuePtr queue,
    uint32_t binding,
    const std::vector<T>& src
  ) {
    return setStorage(queue, binding, src.data(), src.size() * sizeof(T));
  }

  template <typename T>
  bool setUniform(
    VulkanQueuePtr queue,
    uint32_t binding,
    const std::vector<T>& src
  ) {
    return setUniform(queue, binding, src.data(), src.size() * sizeof(T));
  }

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


struct VertexBuffer {
  VulkanBufferPtr buffer;
  VkDeviceSize    offset;
  VkVertexInputBindingDescription description;
  VkVertexInputAttributeDescriptions attributes;
};
typedef std::vector<VertexBuffer> VertexBuffers;


class VulkanMesh {
private:
  VulkanDevice*   mDevice;
  VertexBuffers   mVertexBuffers;
  VulkanBufferPtr mIndexBuffer;
  uint32_t        mVertexCount;
  uint32_t        mIndexCount;
  VkPrimitiveTopology mPrimitiveTopology;

public:
  VulkanMesh(VulkanDevice* device);
  ~VulkanMesh();

  bool addBuffer(VulkanQueuePtr queue, const std::vector<float>& vertices, int vertexSize);
  void addAttribute(int buffer, int location, int size, int offset);
  bool setIndexBuffer(VulkanQueuePtr queue, const std::vector<uint32_t>& indices);
  void setTopology(VkPrimitiveTopology topology) {mPrimitiveTopology = topology;}

  void getVertexBindings(VkVertexInputBindingDescriptions& bindings);
  void getVertexAttributes(VkVertexInputAttributeDescriptions& attributes);
  VkPrimitiveTopology getVkPrimitiveTopology() const {return mPrimitiveTopology;}

  uint32_t getVertexCount() const {return mVertexCount;}
  uint32_t getIndexCount() const {return mIndexCount;}
  VulkanBufferPtr getIndexBuffer() {return mIndexBuffer;}

  void getVertexBufferOffsets(VkDeviceSizes& offsets);
  void getVertexBuffers(VkBuffers& buffers);

  void destroy();

private:
  VkFormat getVertexFormat(int size);
};


class VulkanFrameBuffer {
private:
  VulkanDevice*  mDevice;
  VkExtent2D     mExtent;
  VulkanImages   mColorAttachments;
  VkFramebuffers mVkFramebuffers;

public:
  VulkanFrameBuffer(VulkanDevice* device);
  ~VulkanFrameBuffer();

  void addColorAttachment(VulkanImagePtr image);

  VkFramebuffer getVkFramebuffer(VkRenderPass renderPass, int frame);

  VkExtent2D getExtent() const {return mExtent;}

  uint32_t getColorCount() const {return mColorAttachments.size();}
  void getVkAttachmentReferences(VkAttachmentReferences& references);
  void getVkAttachmentDescriptions(VkAttachmentDescriptions& descriptions);

  void destroy();
  void clearVkFramebuffers();

private:
  VkFramebuffer createVkFramebuffer(VkRenderPass renderPass, int frame);
};


class VulkanRenderPass {
private:
  VulkanDevice*        mDevice;
  VkRenderPasses       mVkRenderPasses;
  VulkanFrameBufferPtr mFramebuffer;

public:
  VulkanRenderPass(VulkanDevice* device);
  ~VulkanRenderPass();

  void setFramebuffer(VulkanFrameBufferPtr framebuffer);

  VkRenderPass  getVkRenderPass(int frame = 0);
  VkFramebuffer getVkFramebuffer(int frame = 0);

  VkExtent2D getExtent()     const;
  VkViewport getViewport()   const;
  VkRect2D   getScissor()    const;
  uint32_t   getColorCount() const;

  void destroy();
  void clearVkRenderPasses();

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
  VkPipeline getVkPipeline(VulkanLayoutSetPtr layoutSet);
  VkPipeline getVkPipeline(VkDescriptorSetLayouts setLayouts);
  VkPipeline getVkPipeline(
    VulkanRenderPassPtr renderPass,
    VulkanMeshPtr       mesh,
    VulkanLayoutSetPtr  layoutSet
  );

  VkPipelineLayout getVkPipelineLayout();
  VkPipelineLayout getVkPipelineLayout(VulkanLayoutSetPtr layoutSet);
  VkPipelineLayout getVkPipelineLayout(VkDescriptorSetLayouts setLayouts);

  void destroy();

private:
  VkPipeline createComputePipeline(VulkanLayoutSetPtr layoutSet);
  VkPipeline createComputePipeline(VkDescriptorSetLayouts setLayouts);
  VkPipeline createGraphicsPipeline(
    VulkanRenderPassPtr renderPass,
    VulkanMeshPtr       mesh,
    VulkanLayoutSetPtr  layoutSet
  );

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
  void copyBufferToBuffer(VulkanBufferPtr src, VulkanBufferPtr dst);

  VulkanCommandPtr createCommand(int frames = 0);
  VulkanCommandPtr beginSingleCommand();

  void submitCommand(VulkanCommandPtr command);
  void submitCommand(VulkanCommandPtr command, VulkanFrameSyncPtr frameSync);
  void waitIdle();

private:
  void destroyCommandPool();
};


class VulkanCommand {
private:
  VulkanQueue*     mQueue;
  VkCommandPool    mVkCommandPool;
  VkCommandBuffers mVkCommandBuffers;
  VkCommandBuffer  mRecordingBuffer;
  int              mRecordingFrame;

public:
  VulkanCommand(VulkanQueue* queue, int frames);
  ~VulkanCommand();

  VkCommandBuffer getVkCommandBuffer(int frame = 0) const {
    return mVkCommandBuffers.at(frame);
  }

  bool begin(int frame = 0, VkCommandBufferUsageFlags usageFlags = 0);
  void end();
  void submit(int frame = 0);
  void endSingle();

  void bind(VulkanPipelinePtr pipeline);
  void bind(VulkanLayoutSetPtr layoutSet);
  void bind(VulkanPipelinePtr pipeline, VulkanLayoutSetPtr layoutSet);
  void dispatch(uint32_t x, uint32_t y = 1, uint32_t z = 1);

  void beginRenderPass(VulkanRenderPassPtr renderPass);
  void endRenderPass();

  void bind(
    VulkanPipelinePtr   pipeline,
    VulkanRenderPassPtr renderPass,
    VulkanMeshPtr       mesh        = nullptr,
    VulkanLayoutSetPtr  layoutSet   = nullptr
  );
  void draw(uint32_t vertexCount, uint32_t instances = 1);
  void draw(VulkanBufferPtr indexBuffer, uint32_t instances = 1);
  void draw(VulkanMeshPtr mesh, uint32_t instances = 1);

  void transition(
    VulkanImagePtr image,
    VkImageLayout  newImageLayout,
    VkAccessFlags  dstAccessMask,
    VkPipelineStageFlags dstStageMask
  );
  void copyBufferToImage(VulkanBufferPtr buffer, VulkanImagePtr image);
  void copyImageToBuffer(VulkanImagePtr image, VulkanBufferPtr buffer);
  void copyBufferToBuffer(VulkanBufferPtr src, VulkanBufferPtr dst);

  bool alloc(uint32_t frames);
  void free();

  void destroy();
};


class VulkanSwapChain {
private:
  VulkanDevice*  mDevice;
  SDL_Window*    mSdlWindow;
  VkSurfaceKHR   mVkSurface;
  VkSwapchainKHR mVkSwapChain;
  uint32_t       mImageCount;
  uint32_t       mCurrentFrame;
  VulkanImagePtr mPresentImage;

  VkSurfaceFormatKHR mVkSurfaceFormat;
  VkPresentModeKHR   mVkPrsentMode;
  VulkanFrameSyncPtr mFrameSync;

public:
  VulkanSwapChain(VulkanDevice* device);
  ~VulkanSwapChain();

  void setToWindow(SDL_Window *window);

  VkSurfaceCapabilitiesKHR getVkSurfaceCapabilities() const;
  VkSurfaceFormats         getVkSurfaceFormats() const;
  VkPresentModes           getVkPresentModes() const;

  VkSurfaceFormatKHR pickSurfaceFormat() const;
  VkPresentModeKHR   pickPresentMode() const;
  uint32_t           pickImageCount() const;

  VkExtent2D getExtent() const;
  uint32_t frames() const {return !mPresentImage ? 0 : mPresentImage->frames();}

  VulkanImagePtr getPresentImage();
  VulkanFrameSyncPtr getFrameSync();

  int getNextFrame();
  void presentFrame(VulkanQueuePtr queue);

  int getNextFrame(VkSemaphore semaphore);
  int getNextFrame(VulkanFrameSyncPtr frameSync);
  void presentFrame(uint32_t frame, VkSemaphore semaphore, VulkanQueuePtr queue);
  void presentFrame(VulkanFrameSyncPtr frameSync, VulkanQueuePtr queue);

  void destroy();

private:
  void createSwapChain();
  VkSwapchainKHR createVkSwapChain(
    VkSurfaceFormatKHR surfaceFormat,
    VkPresentModeKHR   presentMode,
    uint32_t           imageCount,
    VkExtent2D         extent
  ) const;

  void destroySwapChain();
  void destroySurface();
};


class VulkanFrameSync {
  VulkanDevice* mDevice;
  VkSemaphores  mAvailableSemaphores;
  VkSemaphores  mFinishedSemaphores;
  VkFences      mInFlightFences;
  VkFences      mImagesInFlight;
  int           mFrameInFlight;
  int           mCurrentFrame;


public:
  VulkanFrameSync(VulkanDevice* device);
  ~VulkanFrameSync();

  void setup(int frames, int framesInflight);
  void destroy();

  void waitForInFlight();
  void resetInFlight();
  void assignInFlight(int frame);
  int nextInFlight();

  int currentFrame() const {return mCurrentFrame;}
  int currentInFlight() const {return mFrameInFlight;}
  VkSemaphore availableSemaphore() {return mAvailableSemaphores.at(mFrameInFlight);}
  VkSemaphore finishedSemaphore() {return mFinishedSemaphores.at(mFrameInFlight);}
  VkFence inFlightFence() {return mInFlightFences.at(mFrameInFlight);}
};


#endif /* VulkanIncludes_hpp */
