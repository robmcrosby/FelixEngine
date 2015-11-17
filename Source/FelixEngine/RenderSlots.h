//
//  RenderSlots.h
//  FelixEngine
//
//  Created by Robert Crosby on 10/10/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef RenderSlots_h
#define RenderSlots_h

#include "EventHandler.h"
#include "Component.h"
#include "UniformMap.h"
#include "Material.h"

namespace fx
{
  class GraphicSystem;
  
  class RenderSlot: public EventHandler
  {
  public:
    RenderSlot(Scene *scene);
    virtual ~RenderSlot();
    
    virtual void handle(const Event &event);
    
    bool setToXml(const XMLTree::Node *node);
    
    void update() const;
    void render() const;
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
    
    ClearState& clearState() {return mClearState;}
    const ClearState& clearState() const {return mClearState;}
    
    bool setMesh(const XMLTree::Node &node);
    bool setView(const XMLTree::Node &node);
    bool setFrame(const XMLTree::Node &node);
    bool setMaterial(const XMLTree::Node &node);
    
    void setMesh(const std::string &name);
    void setMesh(Mesh *mesh) {mMesh = mesh;}
    Mesh* mesh() const {return mMesh;}
    
    void setFrame(const std::string &name);
    void setFrame(Frame *frame) {mFrame = frame;}
    Frame* frame() const {return mFrame;}
    
    void setPass(const std::string &pass);
    std::string pass() const {return mPass;}
    int passIndex() const {return mPassIndex;}
    
    void setMaterial(const std::string &name);
    void setMaterial(Material *material) {mMaterial = material;}
    Material* material() const {return mMaterial;}
    
    void setLocalUniforms(UniformMap *map) {mLocalUniforms = map;}
    UniformMap* localUniforms() const {return mLocalUniforms;}
    
    static int GetPassIndex(const std::string &pass);
    
  private:
    bool mVisible;
    int mLayer, mSubMesh, mViewIndex, mInstances;
    DepthState mDepthState;
    BlendState mBlendState;
    ClearState mClearState;
    
    Mesh  *mMesh;
    Frame *mFrame;
    
    Material *mMaterial;
    
    UniformMap *mLocalUniforms;
    
    std::string mPass;
    int mPassIndex;
    
    Scene *mScene;
    GraphicSystem *mGraphicSystem;
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
    
  public:
    typedef std::vector<RenderSlot*>::const_iterator iterator;
    iterator begin() const {return mSlots.begin();}
    iterator end() const {return mSlots.end();}
    
    RenderSlot* operator[](int i) const {return i < (int)mSlots.size() ? mSlots[i] : nullptr;}
    RenderSlot* back() const {return mSlots.size() ? mSlots.back() : nullptr;}
    RenderSlot* front() const {return mSlots.size() ? mSlots.front() : nullptr;}
    
    void addSlot();
    void clear();
    
  public:
    UniformMap& localUniforms() {return mLocalUniforms;}
    const UniformMap& localUniforms() const {return mLocalUniforms;}
    
  private:
    GraphicSystem *mGraphicSystem;
    UniformMap mLocalUniforms;
    std::vector<RenderSlot*> mSlots;
  };
}

#endif /* RenderSlots_h */
