//
//  Drawable.h
//  FelixEngine
//
//  Created by Robert Crosby on 7/21/18.
//  Copyright © 2018 Robert Crosby. All rights reserved.
//

#ifndef Drawable_h
#define Drawable_h

#include "Scene.h"
#include "RenderPass.h"


namespace fx {
  
  /**
   *
   */
  class Drawable: public iObject {
  protected:
    Scene *_scene;
    std::set<RenderPassPtr> _renderPasses;
    
  public:
    Drawable() {}
    virtual ~Drawable() {}
    
    virtual void setScene(Scene *scene) {_scene = scene;}
    
    /**
     *
     */
    void setToRenderPass(RenderPassPtr pass) {_renderPasses.insert(pass);}
    
    /**
     *
     */
    void removeFromRenderPass(RenderPassPtr pass) {_renderPasses.erase(pass);}
    
    /**
     *
     */
    bool setToRenderPass(const XMLTree::Node &node) {
      // TODO
      return false;
    }
    
    /**
     *
     */
    void setToRenderPass(const std::string &name) {
      setToRenderPass(Graphics::getInstance().getRenderPass(name));
    }
    
    /**
     *
     */
    void removeFromRenderPass(const std::string &name) {
      removeFromRenderPass(Graphics::getInstance().getRenderPass(name));
    }
  };
}

#endif /* Drawable_h */
