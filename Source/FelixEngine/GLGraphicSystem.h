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
#include <SDL2/SDL_opengl.h>

namespace fx
{
  class GLWindow;
  class GLFrame;
  class GLShader;
  class GLMesh;
  class GLTexture;
  
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
    
    virtual Window* getWindow(const std::string &name);
    virtual Frame* getFrame(const std::string &name);
    virtual Shader* getShader(const std::string &name);
    virtual Mesh* getMesh(const std::string &name);
    virtual Texture* getTexture(const std::string &name);
    
    void setContext(SDL_GLContext context) {mContext = context;}
    SDL_GLContext getContext() {return mContext;}
    
    void loadOnNextUpdate() {mCheckForUnloaded = true;}
  private:
    void checkForUnloaded();
    bool setVersion(const XMLTree::Node *node);
    
    std::map<std::string, GLWindow*>  mWindows;
    std::map<std::string, GLFrame*>   mFrames;
    std::map<std::string, GLShader*>  mShaders;
    std::map<std::string, GLMesh*>    mMeshes;
    std::map<std::string, GLTexture*> mTextures;
    
    SDL_GLContext mContext;
    bool mCheckForUnloaded;
  };
}

#endif /* OpenGLSystem_hpp */
