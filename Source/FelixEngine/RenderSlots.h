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
#include "Material.h"
#include "Scene.h"
#include "OwnPtr.h"

#include "TextureMap.h"

#include "MeshLoader.h"
#include "TextureLoader.h"

namespace fx
{
  /**
   * Projection Flags are used to communicate to the Projection.
   */
  enum PROJ_FLAGS
  {
    PROJ_STEREO = 0x0003, /**< Stereo Projection Mask */
    PROJ_MONO   = 0x0000, /**< Mono Projection */
    PROJ_LEFT   = 0x0001, /**< Left Projection */
    PROJ_RIGHT  = 0x0002, /**< Right Projection */
    
    PROJ_SPLIT  = 0x0004, /**< Split the Frame for Left and Right */
    
    PROJ_ROTATION = 0x0030, /**< Rotation Mode Maks */
    PROJ_ROT_NONE = 0x0000, /**< No Rotation of the Projection */
    PROJ_ROT_CW   = 0x0010, /**< Rotate the Projection Clockwise */
    PROJ_ROT_CCW  = 0x0020, /**< Rotate the Projection Counter Clockwise */
    PROJ_ROT_FULL = 0x0030, /**< Rotate the Projection Upside Down */
  };
  
  class RenderSlot: public EventHandler
  {
  public:
    RenderSlot(Scene *scene):
      mScene(scene),
      mTextureMap(scene),
      mGraphicSystem(FelixEngine::GetGraphicSystem()),
      mShader(0),
      mMesh(0),
      mFrame(0),
      mEnabled(true),
      mLayer(0),
      mPass(0),
      mProjFlags(0),
      mTaskType(GRAPHIC_TASK_DRAW),
      mStereoType(STEREO_MONO)
    {
      setEventFlags(EVENT_APP_RENDER);
      mGraphicSystem->addHandler(this);
    }
    RenderSlot(const RenderSlot &other):
      mScene(other.mScene),
      mTextureMap(other.mScene),
      mGraphicSystem(FelixEngine::GetGraphicSystem()),
      mShader(0),
      mMesh(0),
      mUniforms(0),
      mFrame(0)
    {
      setEventFlags(EVENT_APP_RENDER);
      mGraphicSystem->addHandler(this);
      *this = other;
    }
    virtual ~RenderSlot() {}
    
    RenderSlot& operator=(const RenderSlot &other)
    {
      mEnabled = other.mEnabled;
      mLayer   = other.mLayer;
      mPass    = other.mPass;
      
      mProjFlags = other.mProjFlags;
      
      mShader   = other.mShader;
      mMesh     = other.mMesh;
      mUniforms = other.mUniforms;
      mFrame    = other.mFrame;
      
      mTextureMap = other.mTextureMap;
      mTaskType   = other.mTaskType;
      mStereoType = other.mStereoType;
      mDrawState  = other.mDrawState;
      
      return *this;
    }
    
    virtual void handle(const Event &event)
    {
      if (event == EVENT_APP_RENDER && event.sender() == mGraphicSystem)
      {
        updateBuffers();
        if (enabled())
          render();
      }
    }
    
    void setToXml(const XMLTree::Node &node)
    {
      if (node.hasAttribute("layer"))
        setLayer(node.attributeAsInt("layer"));
      
      // Set the Pass
      if (node.hasAttribute("pass"))
        setPass(node.attribute("pass"));
      
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
      
      // Set the Frame
      if (node.hasAttribute("frame"))
        setFrame(node.attribute("frame"));
      if (node.hasSubNode("Frame"))
        setFrame(*node.subNode("Frame"));
      
      // Set the Textures
      if (node.hasSubNode("TextureMap"))
        mTextureMap.setToXml(*node.subNode("TextureMap"));
      
      // Set the Task Type
      if (node.hasAttribute("type"))
        setTaskType(node.attribute("type"));
      
      // Set the Stereo Type
      setStereoType(ParseStereoType(node.attribute("stereo")));
      
      // Set the Draw State
      mDrawState.setToXml(node);
    }
    
    void enable() {mEnabled = true;}
    void disable() {mEnabled = false;}
    void setEnabled(bool enabled = true) {mEnabled = enabled;}
    bool enabled() const {return mEnabled;}
    
    void setLayer(int layer) {mLayer = layer;}
    int layer() const {return mLayer;}
    
    void setPass(int pass) {mPass = pass;}
    void setPass(const std::string &name) {setPass(mScene->getPassIndex(name));}
    int pass() const {return mPass;}
    
    void setProjectionFlags(int flags) {mProjFlags = flags;}
    int projectionFlags() const {return mProjFlags;}
    
    void setMesh(const Buffer &mesh) {mMesh = mesh;}
    void setMesh(const std::string &name) {mMesh = &mScene->getMeshBuffer(name);}
    void setMesh(const XMLTree::Node &node) {mMesh = &mScene->createMesh(node);}
    Buffer& mesh() {return *mMesh;}
    const Buffer& mesh() const {return *mMesh;}
    
    void setShader(const Buffer &shader) {mShader = shader;}
    void setShader(const std::string &name) {mShader = &mScene->getShaderBuffer(name);}
    void setShader(const XMLTree::Node &node) {mShader = &mScene->createShader(node);}
    Buffer& shader() {return *mShader;}
    const Buffer& shader() const {return *mShader;}
    
    void setFrame(const Buffer &frame) {mFrame = frame;}
    void setFrame(const std::string &name) {mFrame = &mScene->getFrameBuffer(name);}
    void setFrame(const XMLTree::Node &node) {mFrame = &mScene->createFrame(node);}
    Buffer& frame() {return *mFrame;}
    const Buffer& frame() const {return *mFrame;}
    
    void setUniforms(const Buffer &uniforms) {mUniforms = uniforms;}
    void setUniforms(const XMLTree::Node &node)
    {
      mUniforms = BUFFER_UNIFORM;
      mUniforms->setToXml(node);
      mUniforms->setToUpdate();
    }
    Buffer& uniforms()
    {
      if (!mUniforms.ptr())
        mUniforms = BUFFER_UNIFORM;
      return *mUniforms;
    }
    const Buffer& uniforms() const {return *mUniforms;}
    
    void setUniform(const std::string &name, const Variant &var) {uniforms().set(name, var);}
    void setStruct(const std::string &name, const std::string &comp, const Variant &var)
    {
      uniforms().getBuffer(name, BUFFER_STRUCT).set(comp, var);
      uniforms().setToUpdate();
    }
    
    TextureMap& textureMap() {return mTextureMap;}
    const TextureMap& textureMap() const {return mTextureMap;}
    
    DrawState& drawState() {return mDrawState;}
    const DrawState& drawState() const {return mDrawState;}
    
    void setStereoType(STEREO_TYPE type) {mStereoType = type;}
    STEREO_TYPE stereoType() const {return mStereoType;}
    
    void setTaskType(GRAPHIC_TASK_TYPE type) {mTaskType = type;}
    void setTaskType(const std::string &str) {mTaskType = ParseGraphicTaskType(str);}
    GRAPHIC_TASK_TYPE taskType() const {return mTaskType;}
    
  private:
    void render()
    {
      GraphicTask task(mTaskType);
      task.layer = mLayer;
      task.pass = mPass;
      task.drawState = mDrawState;
      task.bufferSlots[BUFFER_SLOT_MESH]     = mMesh.ptr();
      task.bufferSlots[BUFFER_SLOT_FRAME]    = mFrame.ptr();
      
      task.bufferSlots[BUFFER_SLOT_SHADER]   = mShader.ptr();
      task.bufferSlots[BUFFER_SLOT_UNIFORMS] = mUniforms.ptr();
      task.textureMap = &mTextureMap;
      
      mGraphicSystem->addGraphicTask(task);
    }
    void updateBuffers()
    {
      if (mShader.ptr() && !mShader->updated())
        mGraphicSystem->uploadBuffer(*mShader);
      if (mMesh.ptr() && !mMesh->updated())
        mGraphicSystem->uploadBuffer(*mMesh);
      if (mUniforms.ptr() && !mUniforms->updated())
        mGraphicSystem->uploadBuffer(*mUniforms);
      if (mFrame.ptr() && !mFrame->updated())
        mGraphicSystem->uploadBuffer(*mFrame);
    }
    
    bool mEnabled;
    int  mLayer;
    int  mPass;
    
    int mProjFlags;
    
    OwnPtr<Buffer> mMesh;
    OwnPtr<Buffer> mFrame;
    
    OwnPtr<Buffer> mShader;
    OwnPtr<Buffer> mUniforms;
    TextureMap     mTextureMap;
    
    GRAPHIC_TASK_TYPE mTaskType;
    STEREO_TYPE       mStereoType;
    DrawState         mDrawState;
    
    Scene         *mScene;
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
    
    void setGlobal(const std::string &name, const Variant &v)
    {
      for (iterator itr = begin(); itr != end(); ++itr)
        (*itr)->uniforms()[name] = v;
    }
    
  public:
    typedef std::vector<RenderSlot*>::iterator iterator;
    iterator begin() {return mSlots.begin();}
    iterator end() {return mSlots.end();}
    
    typedef std::vector<RenderSlot*>::const_iterator const_iterator;
    const_iterator begin() const {return mSlots.begin();}
    const_iterator end() const {return mSlots.end();}
    
    RenderSlot* operator[](int i) const {return i < (int)mSlots.size() ? mSlots[i] : nullptr;}
    RenderSlot* back() const {return mSlots.size() ? mSlots.back() : nullptr;}
    RenderSlot* front() const {return mSlots.size() ? mSlots.front() : nullptr;}
    
    void addSlot() {mSlots.push_back(new RenderSlot(mScene));}
    void addSlot(RenderSlot *slot) {mSlots.push_back(slot);}
    
    void clear()
    {
      for (iterator itr = begin(); itr != end(); ++itr)
        delete *itr;
      mSlots.clear();
    }
    
    void setUniform(const std::string &name, const Variant &var)
    {
      for (RenderSlot *slot : mSlots)
        slot->setUniform(name, var);
    }
    void setStruct(const std::string &name, const std::string &comp, const Variant &var)
    {
      for (RenderSlot *slot : mSlots)
        slot->setStruct(name, comp, var);
    }
    
  private:
    GraphicSystem *mGraphicSystem;
    std::vector<RenderSlot*> mSlots;
  };
}

#endif /* RenderSlots_h */
