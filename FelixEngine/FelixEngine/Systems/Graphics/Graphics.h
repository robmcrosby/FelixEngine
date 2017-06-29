//
//  Graphics.h
//  FelixEngine
//
//  Created by Robert Crosby on 6/14/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#ifndef Graphics_h
#define Graphics_h


namespace fx {
  class FrameBuffer;
  class ShaderProgram;
  class VertexMesh;
  class UniformBuffer;
  class GraphicTask;
  
  /** File System */
  class Graphics {
  protected:
    static Graphics *instance;
    
  public:
    static Graphics& getInstance() {return *instance;}
    
  public:
    virtual ~Graphics() {}
    
    virtual FrameBuffer*   getMainWindowBuffer() = 0;
    virtual ShaderProgram* createShaderProgram() = 0;
    virtual VertexMesh*    createVertexMesh()    = 0;
    virtual UniformBuffer* createUniformBuffer() = 0;
    
    virtual void nextFrame() = 0;
    virtual void addTask(const GraphicTask &task) = 0;
    virtual void render() = 0;
  };
  
}

#endif /* Graphics_h */
