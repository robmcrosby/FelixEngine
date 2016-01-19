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
#include "Scene.h"
#include "OwnPtr.h"
#include "MeshLoader.h"

namespace fx
{
  class GraphicSystem;
  
//  class RenderSlot: public EventHandler
//  {
//  public:
//    RenderSlot(Scene *scene): mVisible(true), mLayer(0), mSubMesh(0), mViewIndex(-1), mFrame(0), mPassIndex(0), mCullMode(CULL_NONE),
//    mInstances(1), mMesh(0), mScene(scene), mGraphicSystem(FelixEngine::GetGraphicSystem()), mMaterial(0), mStereoFlags(STEREO_ALL)
//    {
//      setEventFlags(EVENT_APP_RENDER);
//      mGraphicSystem->addHandler(this);
//    }
//    virtual ~RenderSlot() {}
//    
//    virtual void handle(const Event &event)
//    {
//      if (event == EVENT_APP_RENDER && event.sender() == mGraphicSystem)
//        render();
//    }
//    
//    void setToXml(const XMLTree::Node &node)
//    {
//      if (node.hasAttribute("layer"))
//        setLayer(node.attributeAsInt("layer"));
//      if (node.hasAttribute("subMesh"))
//        setSubMesh(node.attributeAsInt("subMesh"));
//      if (node.hasAttribute("instances"))
//        setInstances(node.attributeAsInt("instances"));
//      
//      // Set the States
//      if (node.hasSubNode("DepthState"))
//        mDepthState.setToXml(*node.subNode("DepthState"));
//      if (node.hasSubNode("BlendState"))
//        mBlendState.setToXml(*node.subNode("BlendState"));
//      if (node.hasSubNode("ClearState"))
//        mClearState.setToXml(*node.subNode("ClearState"));
//      
//      // Set the Triangle Cull Mode
//      if (node.hasAttribute("cull"))
//        setCullMode(node.attribute("cull"));
//      else
//        setCullMode(CULL_NONE);
//      
//      // Set the Uniforms
//      if (node.hasSubNode("Uniforms"))
//        mUniforms.setToXml(node.subNode("Uniforms"));
//      
//      // Set the Mesh
//      if (node.hasAttribute("mesh"))
//        setMesh(node.attribute("mesh"));
//      else if (node.hasSubNode("Mesh"))
//        setMesh(*node.subNode("Mesh"));
//      
//      // Set the Pass
//      if (node.hasAttribute("pass"))
//        setPass(node.attribute("pass"));
//      
//      // Set the Frame
//      if (node.hasAttribute("frame"))
//        setFrame(node.attribute("frame"));
//      else if (node.hasSubNode("Frame"))
//        setFrame(*node.subNode("Frame"));
//      else
//        setFrame(MAIN_WINDOW);
//      
//      // Set the Material
//      if (node.hasAttribute("material"))
//        setMaterial(node.attribute("material"));
//      else if (node.hasSubNode("Material"))
//        setMaterial(*node.subNode("Material"));
//      
//      // Set the Stereo Type
//      setStereoFlags(GraphicSystem::GetStereoFlags(node.attribute("stereo")));
//    }
//
//    void update() const;
//    void render() const
//    {
//      GraphicTask task;
//      if (mGraphicSystem && applyToTask(task))
//        mGraphicSystem->addGraphicTask(task);
//    }
//    bool applyToTask(GraphicTask &task) const
//    {
//      // Return false if not visible.
//      if (!mVisible)
//        return false;
//      
//      // Assign the items to the task.
//      task.layer      = mLayer;
//      task.subMesh    = mSubMesh;
//      task.instances  = mInstances;
//      
//      task.depthState = mDepthState;
//      task.blendState = mBlendState;
//      task.clearState = mClearState;
//      
//      task.cullMode = mCullMode;
//      
//      task.mesh   = mMesh;
//      task.frame  = mFrame;
//      task.pass   = mPassIndex;
//      task.stereo = mStereoFlags;
//      
//      task.localUniforms = mUniforms.getInternalMap();
//      
//      if (mMaterial)
//        mMaterial->applyToTask(task);
//      else
//      {
//        task.shader = nullptr;
//        task.textureMap = nullptr;
//        task.materialUniforms = nullptr;
//      }
//      
//      return true;
//    }
//    
//    void setVisible() {mVisible = true;}
//    void setHidden() {mVisible = false;}
//    bool visible() const {return mVisible;}
//    
//    void setLayer(int layer) {mLayer = layer;}
//    int layer() const {return mLayer;}
//
//    void setSubMesh(int index) {mSubMesh = index;}
//    int subMesh() const {return mSubMesh;}
//    
//    void setViewIndex(int index) {mViewIndex = index;}
//    int viewIndex() const {return mViewIndex;}
//    
//    void setInstances(int instances) {mInstances = instances;}
//    int instances() const {return mInstances;}
//    
//    DepthState& depthState() {return mDepthState;}
//    const DepthState& depthState() const {return mDepthState;}
//    
//    BlendState& blendState() {return mBlendState;}
//    const BlendState& blendState() const {return mBlendState;}
//    
//    ClearState& clearState() {return mClearState;}
//    const ClearState& clearState() const {return mClearState;}
//    
//    void setCullMode(CULL_MODE mode) {mCullMode = mode;}
//    void setCullMode(const std::string &str) {setCullMode(GetCullMode(str));}
//    CULL_MODE cullMode() const {return mCullMode;}
//    
//    void setMaterial(const XMLTree::Node &node)
//    {
//      if (node.hasAttribute("name"))
//        setMaterial(node.attribute("name"));
//      else
//        mMaterial = new Material();
//      mMaterial->setToXml(node);
//    }
//    
//    void setMesh(Mesh *mesh) {mMesh = mesh;}
//    void setMesh(const std::string &name)
//    {
//      if (mGraphicSystem)
//        setMesh(mGraphicSystem->getMesh(name));
//    }
//    void setMesh(const XMLTree::Node &node)
//    {
//      if (node.hasAttribute("name"))
//      {
//        setMesh(node.attribute("name"));
//        mMesh->setToXml(node);
//      }
//    }
//    Mesh* mesh() const {return mMesh;}
//    
//    void setFrame(Frame *frame) {mFrame = frame;}
//    void setFrame(const std::string &name)
//    {
//      if (mGraphicSystem)
//        setFrame(mGraphicSystem->getFrame(name));
//    }
//    void setFrame(const XMLTree::Node &node)
//    {
//      if (node.hasAttribute("name"))
//      {
//        setFrame(node.attribute("name"));
//        mFrame->setToXml(node);
//      }
//    }
//    Frame* frame() const {return mFrame;}
//    
//    void setPass(const std::string &pass)
//    {
//      mPass = pass;
//      mPassIndex = mPass == "" ? 0 : GetPassIndex(mPass);
//    }
//    std::string pass() const {return mPass;}
//    int passIndex() const {return mPassIndex;}
//    
//    void setMaterial(const std::string &name)
//    {
//      if (mScene)
//        setMaterial(mScene->getMaterial(name));
//    }
//    void setMaterial(Material *material) {mMaterial = material;}
//    Material* material() const {return mMaterial;}
//    
//    void setStereoFlags(int flags) {mStereoFlags = flags;}
//    int stereoFlags() const {return mStereoFlags;}
//    
//    UniformMap& uniforms() {return mUniforms;}
//    const UniformMap& uniforms() const {return mUniforms;}
//    
//    void lock() {mUniforms.lock();}
//    void unlock() {mUniforms.unlock();}
//    
//    static int GetPassIndex(const std::string &pass)
//    {
//      static int counter = 0;
//      static std::map<std::string, int> passes;
//      
//      if (!passes.count(pass))
//        passes[pass] = ++counter;
//      return passes[pass];
//    }
//    
//    static CULL_MODE GetCullMode(const std::string &str)
//    {
//      if (str == "back")
//        return CULL_BACK;
//      if (str == "front")
//        return CULL_FRONT;
//      return CULL_NONE;
//    }
//    
//  private:
//    bool mVisible;
//    int mLayer, mSubMesh, mViewIndex, mInstances;
//    DepthState mDepthState;
//    BlendState mBlendState;
//    ClearState mClearState;
//    
//    CULL_MODE mCullMode;
//    
//    Mesh  *mMesh;
//    Frame *mFrame;
//    
//    Material *mMaterial;
//    
//    //UniformMap mUniforms;
//    
//    std::string mPass;
//    int mPassIndex;
//    int mStereoFlags;
//    
//    Scene *mScene;
//    GraphicSystem *mGraphicSystem;
//  };
  
  
  
  
  
  class RenderSlot: public EventHandler
  {
  public:
    RenderSlot(Scene *scene): mShader(BUFFER_MAP_SHADER), mMesh(BUFFER_MAP_MESH), mScene(scene), mGraphicSystem(FelixEngine::GetGraphicSystem())
    {
      setEventFlags(EVENT_APP_RENDER);
      mGraphicSystem->addHandler(this);
    }
    virtual ~RenderSlot() {}
    
    virtual void handle(const Event &event)
    {
      if (event == EVENT_APP_RENDER && event.sender() == mGraphicSystem)
        render();
    }
    
    void setToXml(const XMLTree::Node &node)
    {
      if (node.hasAttribute("layer"))
        setLayer(node.attributeAsInt("layer"));
      
      // Set the Mesh
      if (node.hasAttribute("mesh"))
        setMesh(node.attribute("mesh"));
      if (node.hasSubNode("Mesh"))
        setMesh(*node.subNode("Mesh"));
      
      // Set the Shader
      if (node.hasAttribute("shader"))
        setShader(node.attribute("shader"));
      if (node.hasSubNode("Shader"))
        setShader(*node.subNode("Shader"));
      
      // Set the Uniforms
      if (node.hasSubNode("Uniforms"))
        setUniforms(*node.subNode("Uniforms"));
      
      // Set the Draw State
      mDrawState.setToXml(node);
    }
    
    void setLayer(int layer) {mLayer = layer;}
    int layer() const {return mLayer;}
    
    void setMesh(BufferMap &mesh) {mMesh = &mesh;}
    void setMesh(const std::string &name) {setMesh(mScene->getBufferMap(name));}
    void setMesh(const XMLTree::Node &node)
    {
      setMesh(node.attribute("name"));
      mMesh->setToXml(node);
      mGraphicSystem->uploadBuffer(*mMesh);
    }
    BufferMap& mesh() {return *mMesh;}
    
    void setShader(BufferMap &shader) {mShader = &shader;}
    void setShader(const std::string &name) {setShader(mScene->getBufferMap(name));}
    void setShader(const XMLTree::Node &node)
    {
      setShader(node.attribute("name"));
      mShader->setToXml(node);
      mGraphicSystem->uploadBuffer(*mShader);
    }
    BufferMap& shader() {return *mShader;}
    
    void setUniforms(const BufferMap &uniforms) {mUniforms = uniforms;}
    void setUniforms(const XMLTree::Node &node)
    {
      mUniforms = node;
      mGraphicSystem->uploadBuffer(*mUniforms);
    }
    BufferMap& uniforms() {return *mUniforms;}
    
  private:
    void render()
    {
      GraphicTask task(GRAPHIC_TASK_DRAW);
      task.drawState = mDrawState;
      task.bufferSlots[BUFFER_SLOT_SHADER]   = mShader.ptr();
      task.bufferSlots[BUFFER_SLOT_MESH]     = mMesh.ptr();
      task.bufferSlots[BUFFER_SLOT_UNIFORMS] = mUniforms.ptr();
      mGraphicSystem->addGraphicTask(task);
    }
    
    int mLayer;
    
    OwnPtr<BufferMap> mShader;
    OwnPtr<BufferMap> mMesh;
    OwnPtr<BufferMap> mUniforms;
    
    DrawState mDrawState;
    
    Scene *mScene;
    GraphicSystem *mGraphicSystem;
  };
  
  
  
  
  
  
  
  /**
   *
   */
  class RenderSlots: public Component
  {
  public:
    RenderSlots(Scene *scene): Component(scene), mGraphicSystem(FelixEngine::GetGraphicSystem()) {}
    virtual ~RenderSlots() {clear();}
    
    virtual void setToXml(const XMLTree::Node &node)
    {
      Component::setToXml(node);
      for (XMLTree::const_iterator itr = node.begin(); itr != node.end(); ++itr)
      {
        addSlot();
        back()->setToXml(**itr);
      }
    }
    
  public:
    typedef std::vector<RenderSlot*>::const_iterator iterator;
    iterator begin() const {return mSlots.begin();}
    iterator end() const {return mSlots.end();}
    
    RenderSlot* operator[](int i) const {return i < (int)mSlots.size() ? mSlots[i] : nullptr;}
    RenderSlot* back() const {return mSlots.size() ? mSlots.back() : nullptr;}
    RenderSlot* front() const {return mSlots.size() ? mSlots.front() : nullptr;}
    
    void addSlot() {mSlots.push_back(new RenderSlot(mScene));}
    
    void clear()
    {
      for (iterator itr = begin(); itr != end(); ++itr)
        delete *itr;
      mSlots.clear();
    }
    
  private:
    GraphicSystem *mGraphicSystem;
    std::vector<RenderSlot*> mSlots;
  };
}

#endif /* RenderSlots_h */
