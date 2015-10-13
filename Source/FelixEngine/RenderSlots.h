//
//  RenderSlots.h
//  FelixEngine
//
//  Created by Robert Crosby on 10/10/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef RenderSlots_h
#define RenderSlots_h

#include "Component.h"
#include "UniformMap.h"
#include "Material.h"

namespace fx
{
  class GraphicSystem;
  class View;
  
  class RenderSlot
  {
  public:
    RenderSlot(Scene *scene);
    ~RenderSlot();
    
    bool setToXml(const XMLTree::Node *node);
    bool applyToTask(GraphicTask &task) const;
    
    void setVisible() {mVisible = true;}
    void setHidden() {mVisible = false;}
    bool visible() const {return mVisible;}
    
    void setLayer(int layer) {mLayer = layer;}
    int layer() const {return mLayer;}
    
    void setSubMesh(int index) {mSubMesh = index;}
    int subMesh() const {return mSubMesh;}
    
    void setViewIndex(int index) {mViewIndex = index;}
    int viewIndex() const {return mViewIndex;}
    
    void setInstances(int instances) {mInstances = instances;}
    int instances() const {return mInstances;}
    
    DepthState& depthState() {return mDepthState;}
    const DepthState& depthState() const {return mDepthState;}
    
    BlendState& blendState() {return mBlendState;}
    const BlendState& blendState() const {return mBlendState;}
    
    void setMesh(const std::string &name);
    void setMesh(Mesh *mesh) {mMesh = mesh;}
    Mesh* mesh() const {return mMesh;}
    
    void setView(const std::string &name);
    void setView(View *view) {mView = view;}
    View* view() const {return mView;}
    
    Material& material() {return mMaterial;}
    const Material& material() const {return mMaterial;}
    
    void setMaterial(const std::string &name);
    void setMaterial(Material *material) {mMaterialPtr = material;}
    void setToInternalMaterial() {mMaterialPtr = &mMaterial;}
    Material* materialPointer() const {return mMaterialPtr;}
    
    void setUniformMap(UniformMap *map) {mUniformMapPtr = map;}
    UniformMap* uniformMap() const {return mUniformMapPtr;}
    
  private:
    bool mVisible;
    int mLayer, mSubMesh, mViewIndex, mInstances;
    DepthState mDepthState;
    BlendState mBlendState;
    
    Mesh *mMesh;
    View *mView;
    Material mMaterial, *mMaterialPtr;
    
    UniformMap *mUniformMapPtr;
    
    Scene *mScene;
    GraphicSystem *mSystem;
  };
  
  /**
   *
   */
  class RenderSlots: public Component
  {
  public:
    RenderSlots(Object *obj);
    virtual ~RenderSlots();
    
    virtual bool setToXml(const XMLTree::Node *node);
    virtual bool init();
    
  private:
    GraphicSystem *mGraphicSystem;
  };
}

#endif /* RenderSlots_h */
