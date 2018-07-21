//
//  Graphics.h
//  FelixEngine
//
//  Created by Robert Crosby on 6/14/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#ifndef Graphics_h
#define Graphics_h

#include <memory>
#include <map>


namespace fx {
  class Application;
  class GraphicTask;
  
  class FrameBuffer;
  class ShaderProgram;
  class VertexMesh;
  class UniformBuffer;
  class TextureBuffer;
  class BufferPool;
  class RenderPass;
  
  typedef std::shared_ptr<FrameBuffer>   FrameBufferPtr;
  typedef std::shared_ptr<ShaderProgram> ShaderProgramPtr;
  typedef std::shared_ptr<VertexMesh>    VertexMeshPtr;
  typedef std::shared_ptr<UniformBuffer> UniformBufferPtr;
  typedef std::shared_ptr<TextureBuffer> TextureBufferPtr;
  typedef std::shared_ptr<BufferPool>    BufferPoolPtr;
  typedef std::shared_ptr<RenderPass>    RenderPassPtr;
  
  class Graphics {
  protected:
    std::map<std::string, std::weak_ptr<FrameBuffer> >   frameMap;
    std::map<std::string, std::weak_ptr<RenderPass> >    passMap;
    std::map<std::string, std::weak_ptr<ShaderProgram> > shaderMap;
    std::map<std::string, std::weak_ptr<VertexMesh> >    vertexMap;
    std::map<std::string, std::weak_ptr<UniformBuffer> > uniformMap;
    std::map<std::string, std::weak_ptr<TextureBuffer> > textureMap;
    
  protected:
    static Graphics *instance;
    
  public:
    static Graphics& getInstance() {return *instance;}
    
  public:
    virtual ~Graphics() {}
    
    virtual FrameBufferPtr   createFrameBuffer()   = 0;
    virtual ShaderProgramPtr createShaderProgram() = 0;
    virtual VertexMeshPtr    createVertexMesh()    = 0;
    virtual UniformBufferPtr createUniformBuffer() = 0;
    virtual TextureBufferPtr createTextureBuffer() = 0;
    virtual BufferPoolPtr    createBufferPool()    = 0;
    virtual RenderPassPtr    createRenderPass()    = 0;
    
    virtual void nextFrame() = 0;
    virtual void addTask(const GraphicTask &task) = 0;
    virtual void presentFrame() = 0;
    
    FrameBufferPtr getFrameBuffer(const std::string &name) {
      FrameBufferPtr frame = frameMap[name].lock();
      if (!frame) {
        frame = createFrameBuffer();
        frameMap[name] = frame;
      }
      return frame;
    }
    
    RenderPassPtr getRenderPass(const std::string &name) {
      RenderPassPtr renderPass = passMap[name].lock();
      if (!renderPass) {
        renderPass = createRenderPass();
        passMap[name] = renderPass;
      }
      return renderPass;
    }
    
    ShaderProgramPtr  getShaderProgram(const std::string &name) {
      ShaderProgramPtr shader = shaderMap[name].lock();
      if (!shader) {
        shader = createShaderProgram();
        shaderMap[name] = shader;
      }
      return shader;
    }
    
    VertexMeshPtr  getVertexMesh(const std::string &name) {
      VertexMeshPtr mesh = vertexMap[name].lock();
      if (!mesh) {
        mesh = createVertexMesh();
        vertexMap[name] = mesh;
      }
      return mesh;
    }
    
    UniformBufferPtr getUniformBuffer(const std::string &name) {
      UniformBufferPtr uniform = uniformMap[name].lock();
      if (!uniform) {
        uniform = createUniformBuffer();
        uniformMap[name] = uniform;
      }
      return uniform;
    }
    
    TextureBufferPtr getTextureBuffer(const std::string &name) {
      TextureBufferPtr texture = textureMap[name].lock();
      if (!texture) {
        texture = createTextureBuffer();
        textureMap[name] = texture;
      }
      return texture;
    }
  };
  
}

#endif /* Graphics_h */
