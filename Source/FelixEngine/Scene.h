//
//  Scene.h
//  FelixEngine
//
//  Created by Robert Crosby on 10/2/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef Scene_h
#define Scene_h

#include <istream>
#include <list>
#include "XMLTree.h"
#include "Object.h"

namespace fx
{
  class Scene
  {
  public:
    Scene() {}
    ~Scene() {clearObjects();}
    
    bool setToXml(const XMLTree::Node *node)
    {
      bool success = false;
      if (node && node->hasAttribute("name"))
      {
        success = true;
        setName(node->attribute("name"));
      }
      return success;
    }
    
    void setName(const std::string &name) {mName = name;}
    std::string name() const {return mName;}
    
    void addObject(Object *obj) {mObjects.push_back(obj);}
    void clearObjects()
    {
      for (std::list<Object*>::iterator itr = mObjects.begin(); itr != mObjects.end(); ++itr)
        delete *itr;
      mObjects.clear();
    }
    
  private:
    std::string mName;
    std::list<Object*> mObjects;
  };
}

#endif /* Scene_h */
