//
//  RenderItem.h
//  FelixEngine
//
//  Created by Robert Crosby on 9/26/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#ifndef RenderItem_h
#define RenderItem_h

#include <set>


namespace fx {
  class RenderPass;
  typedef std::shared_ptr<RenderPass> RenderPassPtr;
  typedef std::set<RenderPassPtr> RenderPasses;
  
  class RenderItem {
  protected:
    RenderPasses _renderPasses;
    
  public:
    RenderItem() {}
    virtual ~RenderItem() {}
    
    void addToRenderPass(const std::string &name);
    void addToRenderPass(RenderPassPtr &renderPass);
    
    void removeFromRenderPass(const std::string &name);
    void removeFromRenderPass(RenderPassPtr &renderPass);
    
    void removeFromAllRenderPasses();
  };
}


#endif /* RenderItem_h */
