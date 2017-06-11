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
#include <set>
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
  class GLUniforms;
  
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
    
    virtual Window* getMainWindow();
    
    virtual Window*  getWindow(const std::string &name);
    virtual Frame*   getFrame(const std::string &name);
    virtual Shader*  getShader(const std::string &name);
    virtual Mesh*    getMesh(const std::string &name);
    virtual Texture* getTexture(const std::string &name);
    
    virtual SDL_Window* getMainSDLWindow();
    
    void setContext(SDL_GLContext context) {mContext = context;}
    SDL_GLContext getContext() {return mContext;}
    
    std::string getShaderFunction(const std::string &name)
    {
      return mShaderFunctions.count(name) ? mShaderFunctions.at(name) : "";
    }
    
  private:
    void processPass(const TaskPass &pass, const GraphicTask *view);
    void processTask(const GraphicTask *task, const GraphicTask *view);
    
    void processUploadTask(const GraphicTask *task);
    void processUnloadTask(const GraphicTask *task);
    void processDownloadTask(const GraphicTask *task);
    void processViewTask(const GraphicTask *task);
    void processDrawTask(const GraphicTask *task, const GraphicTask *view);
    
    bool bindTextures(Buffer *textures);
    
    void setTriangleCullMode(CULL_TRI_MODE mode);
    
    void updateResources();
    bool setVersion(const XMLTree::Node *node);
    bool setShaderFunctions(const XMLTree::Node *node);
    
    bool addWindows(const XMLTree::Node *node);
    bool addWindow(const XMLTree::Node *node);

    
    std::map<std::string, GLWindow*>  mWindows;
    std::map<std::string, GLFrame*>   mFrames;
    std::map<std::string, GLShader*>  mShaders;
    std::map<std::string, GLMesh*>    mMeshes;
    std::map<std::string, GLTexture*> mTextures;
    
    std::set<GLUniforms*> mUniforms;
    
    std::map<std::string, std::string> mShaderFunctions;
    
    SDL_GLContext mContext;
    GLWindow *mMainWindow;
    int mMajorVersion;
    int mMinorVersion;
  };
}

#endif /* OpenGLSystem_hpp */
