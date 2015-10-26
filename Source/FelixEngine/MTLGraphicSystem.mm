//
//  MTLGraphicSystem.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 10/18/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#include "MTLGraphicSystem.h"
#include "UniformMap.h"
#include "TextureMap.h"
#include "ImageLoader.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>

#import "MTLGraphicResources.h"


namespace fx
{
  struct MTLContextInfo
  {
    MTLViewport   mViewport;
    
    id <MTLDevice>       mDevice;
    id <MTLCommandQueue> mCommandQueue;
    id <MTLLibrary>      mDefaultLibrary;
    
    id <MTLCommandBuffer> mCommandBuffer;
    
    NSMutableDictionary *mDepthStencilStates;
    
    dispatch_semaphore_t mInflightSemaphore;
    
    MTLPipelineKey *mPipelineKey;
    MTLDescriptorKey *mDescriptorKey;
  };
  
  
  
  struct MTLTextureInterface: Texture
  {
    MTLTextureInterface(MTLGraphicSystem *system): mSystem(system)
    {
      MTLContextInfo *info = mSystem->getContextInfo();
      mMTLTexture = [[MTLTexture alloc] initWithDevice:info->mDevice];
    }
    virtual ~MTLTextureInterface() {mMTLTexture = nil;}
    
    void update()
    {
      if (isLoading())
      {
        if (load())
          setLoaded();
        else
          setNotLoading();
      }
    }
    bool load()
    {
      bool success = true;
      
      ImageRGBA image;
      if (ImageLoader::LoadImageFromFile(image, mFile))
      {
        image.invert();
        success = [mMTLTexture loadImage:image.ptr() Width:image.width() Height:image.height()];
      }
      
      return success;
    }
    
    MTLTexture *mMTLTexture;
    MTLGraphicSystem *mSystem;
  };
  
  
  
  struct MTLFrameInterface: Frame
  {
    MTLFrameInterface(MTLGraphicSystem *system): mSystem(system)
    {
      MTLContextInfo *info = mSystem->getContextInfo();
      mMTLFrame = [[MTLFrame alloc] initWithDevice:info->mDevice];
    }
    virtual ~MTLFrameInterface() {mMTLFrame = nil;}
    
    void update()
    {
      if (isLoading())
      {
        if (load())
          setLoaded();
        else
          setNotLoading();
      }
    }
    bool load()
    {
      bool success = true;
      
      for (std::list<Buffer>::const_iterator itr = mBuffers.begin(); itr != mBuffers.end(); ++itr)
      {
        if (itr->format == COLOR_RGBA)
          success &= addColorBuffer(*itr);
        else if (itr->format == COLOR_DEPTH32F)
          success &= setDepthBuffer(*itr);
      }
      [mMTLFrame resizeToWidth:mSize.w Height:mSize.h];
      
      return success;
    }
    bool addColorBuffer(const Buffer &buffer)
    {
      if (buffer.name != "")
      {
        MTLTextureInterface *texture = static_cast<MTLTextureInterface*>(mSystem->getTexture(buffer.name));
        texture->retain();
        texture->setLoaded();
        texture->setSampler(buffer.sampler);
        mTextures.push_back(texture);
        
        [texture->mMTLTexture setFormat:MTLPixelFormatRGBA8Unorm];
        [mMTLFrame addColorBuffer:texture->mMTLTexture];
      }
      else
      {
        MTLTexture *texture = [[MTLTexture alloc] initWithDevice:mSystem->getContextInfo()->mDevice];
        [texture setFormat:MTLPixelFormatRGBA8Unorm];
        [mMTLFrame addColorBuffer:texture];
      }
      
      return true;
    }
    bool setDepthBuffer(const Buffer &buffer)
    {
      if (buffer.name != "")
      {
        MTLTextureInterface *texture = static_cast<MTLTextureInterface*>(mSystem->getTexture(buffer.name));
        texture->retain();
        texture->setLoaded();
        texture->setSampler(buffer.sampler);
        mTextures.push_back(texture);
        
        [texture->mMTLTexture setFormat:MTLPixelFormatDepth32Float];
        [mMTLFrame setDepthBuffer:texture->mMTLTexture];
      }
      else
      {
        MTLTexture *texture = [[MTLTexture alloc] initWithDevice:mSystem->getContextInfo()->mDevice];
        [texture setFormat:MTLPixelFormatDepth32Float];
        [mMTLFrame setDepthBuffer:texture];
      }
      return true;
    }
    
    MTLFrame *mMTLFrame;
    MTLGraphicSystem *mSystem;
    std::list<MTLTextureInterface*> mTextures;
  };
  
  
  
  
  struct MTLWindowInterface: Window
  {
    MTLWindowInterface(MTLGraphicSystem *system): mSystem(system), mMTLFrame(0), mSDLWindow(0)
    {
      mMTLWindow = [[MTLWindow alloc] init];
    }
    virtual ~MTLWindowInterface() {mMTLWindow = nil;}
    
    bool init()
    {
      bool success = true;
      
      // Create the window with SDL
      mSDLWindow = SDL_CreateWindow(mTitle.c_str(), mPosition.x, mPosition.y, mSize.w, mSize.h, SDL_WINDOW_RESIZABLE);
      
      // Get the NSView of the Window
      SDL_SysWMinfo *info = (SDL_SysWMinfo*)malloc(sizeof(SDL_SysWMinfo));
      SDL_VERSION(&info->version);
      SDL_GetWindowWMInfo(mSDLWindow, info);
      
      success = [mMTLWindow setNSWindow:info->info.cocoa.window];
      free(info);
      
      if (success)
        setLoaded();
      return success;
    }
    void update() {}
    
    MTLWindow *mMTLWindow;
    MTLFrameInterface *mMTLFrame;
    
    SDL_Window *mSDLWindow;
    MTLGraphicSystem *mSystem;
  };
  
  
  
  struct MTLShaderInterface: Shader
  {
    MTLShaderInterface(MTLGraphicSystem *system): mSystem(system)
    {
      MTLContextInfo *info = mSystem->getContextInfo();
      mMTLShader = [[MTLShader alloc] initWithDevice:info->mDevice andLibrary:info->mDefaultLibrary];
    }
    virtual ~MTLShaderInterface() {mMTLShader = nil;}
    
    void update()
    {
      if (isLoading())
      {
        if (load())
          setLoaded();
        else
          setNotLoading();
      }
    }
    bool load()
    {
      bool success = true;
      
      success &= [mMTLShader setVertexFunction:[NSString stringWithUTF8String:mParts[SHADER_VERTEX].c_str()]];
      success &= [mMTLShader setFragmentFunction:[NSString stringWithUTF8String:mParts[SHADER_FRAGMENT].c_str()]];
      
      return success;
    }
    
    MTLShader *mMTLShader;
    MTLGraphicSystem *mSystem;
  };
  
  
  
  struct MTLMeshInterface: public Mesh
  {
    MTLMeshInterface(MTLGraphicSystem *system): mSystem(system)
    {
      MTLContextInfo *info = mSystem->getContextInfo();
      mMTLMesh = [[MTLMesh alloc] initWithDevice:info->mDevice];
    }
    virtual ~MTLMeshInterface() {mMTLMesh = nil;}
    
    void update()
    {
      if (isLoading())
      {
        if (load())
          setLoaded();
        else
          setNotLoading();
      }
    }
    bool load()
    {
      bool success = true;
      
      // Set the Vertex Buffers
      for (VertexBufferMap::iterator itr = mBufferMap.begin(); itr != mBufferMap.end(); ++itr)
      {
        success &= [mMTLMesh addVertexBuffer:itr->second.ptr()
                             withElementSize:itr->second.components()
                             withNumElements:itr->second.count()
                             forName:[NSString stringWithUTF8String:itr->first.c_str()]];
      }
      
      // Set the Index Buffer if there is one
      if (mBufferMap.indexBuffer().size())
      {
        success &= [mMTLMesh setIndexBuffer:&mBufferMap.indexBuffer().at(0)
                             withNumIndices:mBufferMap.indexBuffer().size()];
      }
      
      // Set the Primitive Type of the mesh
      if (mBufferMap.primitiveType() == VERTEX_TRIANGLES)
        [mMTLMesh setPrimitiveType:MTLPrimitiveTypeTriangle];
      else if (mBufferMap.primitiveType() == VERTEX_TRIANGLE_STRIP)
        [mMTLMesh setPrimitiveType:MTLPrimitiveTypeTriangleStrip];
      
      return success;
    }
    
    MTLMesh *mMTLMesh;
    MTLGraphicSystem *mSystem;
  };
  
  
  
  struct MTLSamplerInterface
  {
    MTLSamplerInterface() {mSampler = nil;}
    ~MTLSamplerInterface() {mSampler = nil;}
    
    id <MTLSamplerState> mSampler;
  };
  
  
  
  struct MTLUniformMapInterface: InternalUniformMap
  {
    MTLUniformMapInterface(UniformMap *map): mUniformMap(map) {mMTLUniformMap = nil;}
    virtual ~MTLUniformMapInterface() {mMTLUniformMap = nil;}
    
    virtual void release() {mUniformMap = nullptr;}
    
    void update()
    {
      if (inUse() && mMTLUniformMap != nil)
      {
        [mMTLUniformMap setUniformsSize:mUniformMap->size()];
        [mMTLUniformMap setBufferSize:mUniformMap->sizeInBytes()];
        
        if (mMTLUniformMap.buffer != nil)
        {
          NSUInteger offset = 0;
          char *buffer = (char*)[mMTLUniformMap.buffer contents];
          UniformMap::const_iterator itr = mUniformMap->begin();
          for (MTLUniform *uniform in mMTLUniformMap.uniforms)
          {
            // Set the Uniform information
            [uniform setName:[NSString stringWithUTF8String:itr->first.c_str()]];
            [uniform setOffset:offset];
            
            // Copy the data to the buffer
            size_t size = itr->second.sizeInBytes();
            void *ptr = (void*)(buffer + offset);
            memcpy(ptr, itr->second.ptr(), size);
            offset += (NSUInteger)size;
          }
        }
      }
    }
    bool inUse() const {return mUniformMap;}
    
    UniformMap    *mUniformMap;
    MTLUniformMap *mMTLUniformMap;
  };
}


using namespace fx;
using namespace std;


DEFINE_SYSTEM_ID(MTLGraphicSystem)


MTLGraphicSystem::MTLGraphicSystem(): mMainWindow(0)
{
  mContextInfo = new MTLContextInfo();
  mSDLInitFlags |= SDL_INIT_VIDEO;
}

MTLGraphicSystem::~MTLGraphicSystem()
{
  delete mContextInfo;
}

Window* MTLGraphicSystem::getWindow(const std::string &name)
{
  if (!mWindows.count(name))
  {
    MTLWindowInterface *window = new MTLWindowInterface(this);
    MTLFrameInterface *frame = static_cast<MTLFrameInterface*>(getFrame(name));
    
    frame->retain();
    window->mMTLFrame = frame;
    [window->mMTLWindow setMetalFrame:frame->mMTLFrame];
    mWindows[name] = window;
    
    if (!mMainWindow)
      mMainWindow = window;
  }
  return mWindows[name];
}

Frame* MTLGraphicSystem::getFrame(const std::string &name)
{
  if (!mFrames.count(name))
    mFrames[name] = new MTLFrameInterface(this);
  return mFrames[name];
}

Shader* MTLGraphicSystem::getShader(const std::string &name)
{
  if (!mShaders.count(name))
    mShaders[name] = new MTLShaderInterface(this);
  return mShaders[name];
}

Mesh* MTLGraphicSystem::getMesh(const std::string &name)
{
  if (!mMeshes.count(name))
    mMeshes[name] = new MTLMeshInterface(this);
  return mMeshes[name];
}

Texture* MTLGraphicSystem::getTexture(const std::string &name)
{
  if (!mTextures.count(name))
    mTextures[name] = new MTLTextureInterface(this);
  return mTextures[name];
}

bool MTLGraphicSystem::setToXml(const XMLTree::Node *node)
{
  if (node)
    return addWindows(node->subNode("Windows"));
  cerr << "Error: GLGraphicSystem passed a NULL node." << endl;
  return false;
}

bool MTLGraphicSystem::init()
{
  bool success = true;
  
  mContextInfo->mInflightSemaphore = dispatch_semaphore_create(1);
  
  // Get the Metal Device
  mContextInfo->mDevice = MTLCreateSystemDefaultDevice();
  
  // Initalize the windows
  for (map<std::string, MTLWindowInterface*>::iterator itr = mWindows.begin(); itr != mWindows.end(); ++itr)
  {
    [itr->second->mMTLWindow setMetalDevice: mContextInfo->mDevice];
    success &= itr->second->init();
  }
  
  // Load the Default Metal Shader Library
  mContextInfo->mDefaultLibrary = [mContextInfo->mDevice newDefaultLibrary];
  
  // Create a new command queue
  mContextInfo->mCommandQueue = [mContextInfo->mDevice newCommandQueue];
  
  mContextInfo->mPipelineKey = [[MTLPipelineKey alloc] init];
  mContextInfo->mDescriptorKey = [[MTLDescriptorKey alloc] init];
  return success;
}

void MTLGraphicSystem::update()
{
  updateResources();
  updateUniforms();
  processTasks();
}

InternalUniformMap* MTLGraphicSystem::getInternalUniformMap(UniformMap *map)
{
  MTLUniformMapInterface *internalUniformMap = new MTLUniformMapInterface(map);
  internalUniformMap->mMTLUniformMap = [[MTLUniformMap alloc] initWithDevice:mContextInfo->mDevice];
  mMTLUniforms.push_back(internalUniformMap);
  return internalUniformMap;
}

void MTLGraphicSystem::setNextWindowDrawables() const
{
  for (map<string, MTLWindowInterface*>::const_iterator itr = mWindows.begin(); itr != mWindows.end(); ++itr)
    [itr->second->mMTLWindow setNextDrawable];
}

void MTLGraphicSystem::presentWindowDrawables() const
{
  for (map<string, MTLWindowInterface*>::const_iterator itr = mWindows.begin(); itr != mWindows.end(); ++itr)
    [itr->second->mMTLWindow presentDrawable:mContextInfo->mCommandBuffer];
}

void MTLGraphicSystem::processTasks()
{
  clearTaskSlots();
  loadTaskSlots();

  @autoreleasepool
  {
    // Wait on the Inflight Semaphore before creating a new Command Buffer
    dispatch_semaphore_wait(mContextInfo->mInflightSemaphore, DISPATCH_TIME_FOREVER);
    mContextInfo->mCommandBuffer = [mContextInfo->mCommandQueue commandBuffer];
    
    // Add a Completed Handler for the Command Buffer that signals the Inflight Semaphore
    __block dispatch_semaphore_t dispatchSemaphore = mContextInfo->mInflightSemaphore;
    [mContextInfo->mCommandBuffer addCompletedHandler:^(id <MTLCommandBuffer> cmdb) {
      dispatch_semaphore_signal(dispatchSemaphore);
    }];
    
    setNextWindowDrawables();
    
    if (mTaskSlots.size())
      processTaskSlot(mTaskSlots.at(0));
    
    presentWindowDrawables();
    
    [mContextInfo->mCommandBuffer commit];
    mContextInfo->mCommandBuffer = nil;
  }
}

void MTLGraphicSystem::processTaskSlot(const TaskSlot &slot)
{
  for (TaskSlot::const_iterator itr = slot.begin(); itr != slot.end(); ++itr)
    processTask(*itr);
}

void MTLGraphicSystem::processTask(const GraphicTask &task)
{
  if (task.isViewTask() && task.viewIndex < (int)mTaskSlots.size())
  {
    TaskSlot &slot = mTaskSlots.at(task.viewIndex);
    if (slot.size())
    {
      // TODO: Fix this when Compute tasks are introduced
      if (task.isClearTask() && !slot.at(0).isClearTask())
        slot.at(0).clearState = task.clearState;
      
      processTaskSlot(slot);
    }
  }
  else if (task.isDrawTask())
  {
    const MTLFrameInterface  *frame  = static_cast<const MTLFrameInterface*>(task.frame);
    const MTLShaderInterface *shader = static_cast<const MTLShaderInterface*>(task.shader);
    const MTLMeshInterface   *mesh   = static_cast<const MTLMeshInterface*>(task.mesh);
    
    
    // Get the pipeline state
    [mContextInfo->mPipelineKey setShader:shader->mMTLShader];
    [mContextInfo->mPipelineKey setBlendingEnabled:task.blendState.isBlendingEnabled()];
    id <MTLRenderPipelineState> pipelineState = [frame->mMTLFrame getPipelineForKey:mContextInfo->mPipelineKey];
    
    // Get the Render Descriptor.
    MTLClearColor color = MTLClearColorMake(task.clearState.colors[0].red,
                                            task.clearState.colors[0].green,
                                            task.clearState.colors[0].blue,
                                            task.clearState.colors[0].alpha);
    [mContextInfo->mDescriptorKey setClearColor:color];
    [mContextInfo->mDescriptorKey setClearColorBuffers:task.clearState.flags & CLEAR_COLOR];
    
    [mContextInfo->mDescriptorKey setClearDepth:task.clearState.depth];
    [mContextInfo->mDescriptorKey setClearDepthBuffer:task.clearState.flags & CLEAR_DEPTH];
    
    MTLRenderPassDescriptor *renderPassDesc = [frame->mMTLFrame getDescriptorForKey:mContextInfo->mDescriptorKey];
    
    NSNumber *depthKey = [NSNumber numberWithInt:task.depthState.flags];
    id <MTLDepthStencilState> depthState = [mContextInfo->mDepthStencilStates objectForKey:depthKey];
    if (depthState == nil)
    {
      MTLDepthStencilDescriptor *depthDesc = [MTLDepthStencilDescriptor new];
      depthDesc.depthWriteEnabled = task.depthState.isWritingEnabled();
      depthDesc.depthCompareFunction = MTLCompareFunctionAlways;
      
      if (task.depthState.getTestFunction() == DEPTH_TEST_LESS)
        depthDesc.depthCompareFunction = MTLCompareFunctionLess;
      else if (task.depthState.getTestFunction() == DEPTH_TEST_GREATER)
        depthDesc.depthCompareFunction = MTLCompareFunctionGreater;
      else if (task.depthState.getTestFunction() == DEPTH_TEST_EQUAL)
        depthDesc.depthCompareFunction = MTLCompareFunctionEqual;
      else if (task.depthState.getTestFunction() == DEPTH_TEST_LESS_EQ)
        depthDesc.depthCompareFunction = MTLCompareFunctionLessEqual;
      else if (task.depthState.getTestFunction() == DEPTH_TEST_GREATER_EQ)
        depthDesc.depthCompareFunction = MTLCompareFunctionGreaterEqual;
      else if (task.depthState.getTestFunction() == DEPTH_TEST_NEVER)
        depthDesc.depthCompareFunction = MTLCompareFunctionNever;
      
      depthState = [mContextInfo->mDevice newDepthStencilStateWithDescriptor:depthDesc];
      depthDesc = nil;
      
      [mContextInfo->mDepthStencilStates setObject:depthState forKey:depthKey];
    }
    
    MTLViewport viewport = {0.0, 0.0, 0.0, 0.0, 0.0, 1.0};
    viewport.width  = [frame->mMTLFrame width];
    viewport.height = [frame->mMTLFrame height];
    
    if (pipelineState && renderPassDesc && depthState && mContextInfo->mCommandBuffer != nil)
    {
      id <MTLRenderCommandEncoder> renderEncoder = [mContextInfo->mCommandBuffer renderCommandEncoderWithDescriptor:renderPassDesc];
      renderPassDesc = nil;
      
      // Set the context state with the render encoder
      [renderEncoder setViewport:viewport];
      [renderEncoder setFrontFacingWinding:MTLWindingCounterClockwise];
      [renderEncoder setDepthStencilState:depthState];
      [renderEncoder setRenderPipelineState:pipelineState];
      
      if (task.localUniforms)
      {
        const MTLUniformMapInterface *uniformMap = static_cast<const MTLUniformMapInterface*>(task.localUniforms);
        [uniformMap->mMTLUniformMap applyToShader:shader->mMTLShader withEncoder:renderEncoder];
      }
      
      if (task.viewUniforms)
      {
        const MTLUniformMapInterface *uniformMap = static_cast<const MTLUniformMapInterface*>(task.viewUniforms);
        [uniformMap->mMTLUniformMap applyToShader:shader->mMTLShader withEncoder:renderEncoder];
      }
      
      if (task.materialUniforms)
      {
        const MTLUniformMapInterface *uniformMap = static_cast<const MTLUniformMapInterface*>(task.materialUniforms);
        [uniformMap->mMTLUniformMap applyToShader:shader->mMTLShader withEncoder:renderEncoder];
      }

      if (task.textureMap)
      {
        NSUInteger index = 0;
        for (TextureMap::const_iterator itr = task.textureMap->begin(); itr != task.textureMap->end(); ++itr, ++index)
        {
          const MTLTextureInterface *texture = static_cast<const MTLTextureInterface*>(itr->texture());
          MTLSamplerInterface *sampler = getSampler(itr->sampler());
          if (texture != nullptr && sampler != nullptr && sampler->mSampler != nil)
          {
            [texture->mMTLTexture applyToEncoder:renderEncoder atIndex:index];
            [renderEncoder setFragmentSamplerState:sampler->mSampler atIndex:index];
          }
        }
      }
      
      [mesh->mMTLMesh drawToEncoder:renderEncoder withShader:shader->mMTLShader instanceCount:task.instances];
      [renderEncoder endEncoding];
      renderEncoder = nil;
    }
  }
}

MTLSamplerInterface* MTLGraphicSystem::getSampler(const Sampler &sampler) const
{
  if (mSamplers.count(sampler))
    return mSamplers.at(sampler);
  
  MTLSamplerDescriptor *samplerDesc = [MTLSamplerDescriptor new];
  
  // Set the Minimize and Magnify Filters
  samplerDesc.minFilter = sampler.minFilter() == SAMPLER_FILTER_LINEAR ? MTLSamplerMinMagFilterLinear : MTLSamplerMinMagFilterNearest;
  samplerDesc.magFilter = sampler.magFilter() == SAMPLER_FILTER_LINEAR ? MTLSamplerMinMagFilterLinear : MTLSamplerMinMagFilterNearest;
  
  // Set the Mip Mapping Filter.
  if (sampler.mipMappingEnabled())
    samplerDesc.mipFilter = sampler.mipFilter() == SAMPLER_FILTER_LINEAR ? MTLSamplerMipFilterLinear : MTLSamplerMipFilterNearest;
  else
    samplerDesc.mipFilter = MTLSamplerMipFilterNotMipmapped;
  
  samplerDesc.maxAnisotropy = sampler.samples();
  
  samplerDesc.sAddressMode = (MTLSamplerAddressMode)getSamplerAddressMode(sampler.sCoord());
  samplerDesc.tAddressMode = (MTLSamplerAddressMode)getSamplerAddressMode(sampler.tCoord());
  samplerDesc.rAddressMode = (MTLSamplerAddressMode)getSamplerAddressMode(sampler.rCoord());
  
  samplerDesc.normalizedCoordinates = sampler.isNormalized();
  samplerDesc.lodMinClamp = sampler.lodMin();
  samplerDesc.lodMaxClamp = sampler.lodMax();
  
  MTLSamplerInterface *mtlSampler = new MTLSamplerInterface();
  mtlSampler->mSampler = [mContextInfo->mDevice newSamplerStateWithDescriptor:samplerDesc];
  
  if (mtlSampler->mSampler != nil)
  {
    mSamplers[sampler] = mtlSampler;
    return mtlSampler;
  }
  delete mtlSampler;
  return nullptr;
}

int MTLGraphicSystem::getSamplerAddressMode(SAMPLER_COORD coord) const
{
  if (coord == SAMPLER_COORD_REPEAT)
    return (int)MTLSamplerAddressModeRepeat;
  if (coord == SAMPLER_COORD_MIRROR)
    return (int)MTLSamplerAddressModeMirrorRepeat;
  if (coord == SAMPLER_COORD_CLAMP_ZERO)
    return (int)MTLSamplerAddressModeClampToZero;
  return (int)MTLSamplerAddressModeClampToEdge;
}

void MTLGraphicSystem::updateResources()
{
  for (std::map<std::string, MTLWindowInterface*>::iterator itr = mWindows.begin(); itr != mWindows.end(); ++itr)
    itr->second->update();
  for (std::map<std::string, MTLFrameInterface*>::iterator itr = mFrames.begin(); itr != mFrames.end(); ++itr)
    itr->second->update();
  for (std::map<std::string, MTLShaderInterface*>::iterator itr = mShaders.begin(); itr != mShaders.end(); ++itr)
    itr->second->update();
  for (std::map<std::string, MTLMeshInterface*>::iterator itr = mMeshes.begin(); itr != mMeshes.end(); ++itr)
    itr->second->update();
  for (std::map<std::string, MTLTextureInterface*>::iterator itr = mTextures.begin(); itr != mTextures.end(); ++itr)
    itr->second->update();
}

void MTLGraphicSystem::updateUniforms()
{
  for (list<MTLUniformMapInterface*>::iterator itr = mMTLUniforms.begin(); itr != mMTLUniforms.end();)
  {
    if ((*itr)->inUse())
    {
      (*itr)->update();
      ++itr;
    }
    else
    {
      delete *itr;
      itr = mMTLUniforms.erase(itr);
    }
  }
}
