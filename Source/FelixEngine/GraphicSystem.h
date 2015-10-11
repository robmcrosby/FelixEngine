//
//  GraphicSystem.h
//  FelixEngine
//
//  Created by Robert Crosby on 10/5/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef GraphicSystem_h
#define GraphicSystem_h

#include "System.h"
#include "GraphicResources.h"


namespace fx
{
  /**
   *
   */
  class GraphicSystem: public System
  {
  public:
    GraphicSystem(): System(SYSTEM_GRAPHICS) {}
    virtual ~GraphicSystem() {}
    
    virtual Window* getWindow(const std::string &name) = 0;
    virtual Frame* getFrame(const std::string &name) = 0;
    virtual Shader* getShader(const std::string &name) = 0;
    virtual Mesh* getMesh(const std::string &name) = 0;
    virtual Texture* getTexture(const std::string &name) = 0;
    
    Material* getMaterial(const std::string &name);
    View* getView(const std::string &name);
    
    GraphicResource* getResource(const std::string &type, const std::string &name);
    
  protected:
    bool addWindows(const XMLTree::Node *node);
    bool addWindow(const XMLTree::Node *node);
    
  private:
    std::map<std::string, Material*> mMaterials;
    std::map<std::string, View*> mViews;
  };
}


#endif /* GPU_System_h */
