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
  
  typedef std::shared_ptr<FrameBuffer>   FramePtr;
  typedef std::shared_ptr<ShaderProgram> ShaderPtr;
  typedef std::shared_ptr<VertexMesh>    VertexPtr;
  typedef std::shared_ptr<UniformBuffer> UniformPtr;
  typedef std::shared_ptr<TextureBuffer> TexturePtr;
  
  /** File System */
  class Graphics {
  private:
    std::map<std::string, std::weak_ptr<FrameBuffer> >   frameMap;
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
    
    virtual FramePtr   getMainWindowBuffer() = 0;
    virtual FramePtr   createFrameBuffer()   = 0;
    virtual ShaderPtr  createShaderProgram() = 0;
    virtual VertexPtr  createVertexMesh()    = 0;
    virtual UniformPtr createUniformBuffer() = 0;
    virtual TexturePtr createTextureBuffer() = 0;
    
    virtual void nextFrame() = 0;
    virtual void addTask(const GraphicTask &task) = 0;
    virtual void presentFrame() = 0;
    
    FramePtr getFrameBuffer(const std::string &name) {
      FramePtr frame = frameMap[name].lock();
      if (!frame) {
        frame = createFrameBuffer();
        frameMap[name] = frame;
      }
      return frame;
    }
    
    ShaderPtr  getShaderProgram(const std::string &name) {
      ShaderPtr shader = shaderMap[name].lock();
      if (!shader) {
        shader = createShaderProgram();
        shaderMap[name] = shader;
      }
      return shader;
    }
    
    VertexPtr  getVertexMesh(const std::string &name) {
      VertexPtr mesh = vertexMap[name].lock();
      if (!mesh) {
        mesh = createVertexMesh();
        vertexMap[name] = mesh;
      }
      return mesh;
    }
    
    UniformPtr getUniformBuffer(const std::string &name) {
      UniformPtr uniform = uniformMap[name].lock();
      if (!uniform) {
        uniform = createUniformBuffer();
        uniformMap[name] = uniform;
      }
      return uniform;
    }
    
    TexturePtr getTextureBuffer(const std::string &name) {
      TexturePtr texture = textureMap[name].lock();
      if (!texture) {
        texture = createTextureBuffer();
        textureMap[name] = texture;
      }
      return texture;
    }
  };
  
}

#endif /* Graphics_h */
