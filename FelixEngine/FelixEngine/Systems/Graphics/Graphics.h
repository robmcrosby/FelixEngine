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
  };
  
}

#endif /* Graphics_h */
