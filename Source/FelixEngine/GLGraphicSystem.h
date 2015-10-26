//
//  GLGraphicSystem.h
//  FelixEngine
//
//  Created by Robert Crosby on 10/5/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef GLGraphicSystem_h
#define GLGraphicSystem_h

#include "GraphicSystem.h"
#include <map>

#define GL_GLEXT_PROTOTYPES 1
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

namespace fx
{
  class GLWindow;
  class GLFrame;
  class GLShader;
  class GLMesh;
  class GLTexture;
  class GLUniformMap;
  
  /**
   *
   */
  class GLGraphicSystem: public GraphicSystem
  {
  public:
    GLGraphicSystem();
    virtual ~GLGraphicSystem();
    
    virtual void update();
    
    virtual bool setToXml(const XMLTree::Node *node);
    virtual bool init();
    
    virtual void setVersion(int major, int minor);
    
    virtual Window*  getWindow(const std::string &name);
    virtual Frame*   getFrame(const std::string &name);
    virtual Shader*  getShader(const std::string &name);
    virtual Mesh*    getMesh(const std::string &name);
    virtual Texture* getTexture(const std::string &name);
    
    virtual InternalUniformMap* getInternalUniformMap(UniformMap *map);
    
    void setContext(SDL_GLContext context) {mContext = context;}
    SDL_GLContext getContext() {return mContext;}
    
    std::string getShaderFunction(std::string &name)
    {
      return mShaderFunctions.count(name) ? mShaderFunctions.at(name) : "";
    }
    
  private:
    void processTasks();
    void processTaskSlot(const TaskSlot &slot);
    void processTask(const GraphicTask &task);
    
    void updateResources();
    void updateUniforms();
    bool setVersion(const XMLTree::Node *node);
    bool setShaderFunctions(const XMLTree::Node *node);
    
    std::map<std::string, GLWindow*>  mWindows;
    std::map<std::string, GLFrame*>   mFrames;
    std::map<std::string, GLShader*>  mShaders;
    std::map<std::string, GLMesh*>    mMeshes;
    std::map<std::string, GLTexture*> mTextures;
    
    std::list<GLUniformMap*> mGLUniforms;
    
    std::map<std::string, std::string> mShaderFunctions;
    
    SDL_GLContext mContext;
    int mMajorVersion;
    int mMinorVersion;
  };
}

#endif /* OpenGLSystem_hpp */
