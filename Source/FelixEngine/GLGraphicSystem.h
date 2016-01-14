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
#include <SDL2/SDL.h>

#if __IPHONEOS__
  #define GLES2_ENABLED 1
  #define GLES3_ENABLED 1
  #include <OpenGLES/ES3/gl.h>
  #include <OpenGLES/ES3/glext.h>
#else
  #define GL_GLEXT_PROTOTYPES 1
  #include <SDL2/SDL_opengl.h>
#endif


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
    
    virtual bool setToXml(const XMLTree::Node *node);
    virtual bool init();
    
    virtual void render();
    
    virtual void setVersion(int major, int minor);
    int majorVersion() const {return mMajorVersion;}
    int minorVersion() const {return mMinorVersion;}
    
    virtual Window*  getWindow(const std::string &name);
    virtual Frame*   getFrame(const std::string &name);
    virtual Shader*  getShader(const std::string &name);
    virtual Mesh*    getMesh(const std::string &name);
    virtual Texture* getTexture(const std::string &name);
    
    virtual SDL_Window* getMainSDLWindow();
    
    void setContext(SDL_GLContext context) {mContext = context;}
    SDL_GLContext getContext() {return mContext;}
    
    std::string getShaderFunction(std::string &name)
    {
      return mShaderFunctions.count(name) ? mShaderFunctions.at(name) : "";
    }
    
  private:
    void processTasks();
    void processPass(const Pass &pass, const GraphicTask *view, int stereo);
    void processTask(const GraphicTask *task, const GraphicTask *view, int stereo);
    
    void setTriangleCullMode(CULL_MODE mode);
    
    void updateResources();
    bool setVersion(const XMLTree::Node *node);
    bool setShaderFunctions(const XMLTree::Node *node);
    
    int getStereoFlags() const;
    
    std::map<std::string, GLWindow*>  mWindows;
    std::map<std::string, GLFrame*>   mFrames;
    std::map<std::string, GLShader*>  mShaders;
    std::map<std::string, GLMesh*>    mMeshes;
    std::map<std::string, GLTexture*> mTextures;
    
    std::map<std::string, std::string> mShaderFunctions;
    
    SDL_GLContext mContext;
    GLWindow *mMainWindow;
    int mMajorVersion;
    int mMinorVersion;
  };
}

#endif /* OpenGLSystem_hpp */
