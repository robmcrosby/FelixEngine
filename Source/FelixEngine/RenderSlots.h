//
//  RenderSlots.h
//  FelixEngine
//
//  Created by Robert Crosby on 10/10/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef RenderSlots_h
#define RenderSlots_h

#include "FelixEngine.h"
#include "EventHandler.h"
#include "Component.h"
#include "Scene.h"
#include "OwnPtr.h"

#include "Material.h"

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
      mGraphicSystem(FelixEngine::GetGraphicSystem()),
      mShader(0),
      mTextures(0),
      mMesh(0),
      mUniforms(0),
      mFrame(0),
      mMaterial(0),
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
      mGraphicSystem(FelixEngine::GetGraphicSystem()),
      mShader(0),
      mTextures(0),
      mMesh(0),
      mUniforms(0),
      mFrame(0),
      mMaterial(0)
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
      mTextures = other.mTextures;
      mFrame    = other.mFrame;
      
      mMaterial = other.mMaterial;
      
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
      // Set the Layer
      if (node.hasAttribute("layer"))
        setLayer(node.attributeAsInt("layer"));
      
      // Set the Pass
      if (node.hasAttribute("pass"))
        setPass(node.attribute("pass"));
      
      // Set the Mesh
      if (node.hasAttribute("mesh"))
        setMesh(node.attribute("mesh"));
      
      // Set the Shader
      if (node.hasAttribute("shader"))
        setShader(node.attribute("shader"));
      
      // Set the Material
      if (node.hasAttribute("material"))
        setMaterial(node.attribute("material"));
      
      // Set the Frame
      if (node.hasAttribute("frame"))
        setFrame(node.attribute("frame"));
      
      // Set the Task Type
      if (node.hasAttribute("type"))
        setTaskType(node.attribute("type"));
      
      // Set the Stereo Type
      setStereoType(ParseStereoType(node.attribute("stereo")));
      
      // Set the Draw State
      mDrawState.setToXml(node);
      
      // Process the Sub-Nodes
      for (const auto &subNode : node)
      {
        if (*subNode == "Mesh")
          setMesh(*subNode);
        else if (*subNode == "Shader")
          setShader(*subNode);
        else if (*subNode == "Frame")
          setFrame(*subNode);
        else if (*subNode == "Textures")
          addTextures(*subNode);
        else if (*subNode == "Texture")
          addTexture(*subNode);
        else if (*subNode == "Material")
          setMaterial(*subNode);
        else if (*subNode == "Uniforms")
          setUniforms(*subNode);
        else
          setUniform(*subNode);
      }
    }
    
    bool enabled() const {return mEnabled;}
    void enable() {mEnabled = true;}
    void disable() {mEnabled = false;}
    void setEnabled(bool enabled = true) {mEnabled = enabled;}
    
    int layer() const {return mLayer;}
    void setLayer(int layer) {mLayer = layer;}
    
    int pass() const {return mPass;}
    void setPass(int pass) {mPass = pass;}
    void setPass(const std::string &name) {setPass(mScene->getPassIndex(name));}
    
    int projectionFlags() const {return mProjFlags;}
    void setProjectionFlags(int flags) {mProjFlags = flags;}
    
    Buffer& mesh()
    {
      if (!mMesh.ptr())
        mMesh = BUFFER_MESH;
      return *mMesh;
    }
    void setMesh(Buffer *mesh) {mMesh = mesh;}
    void setMesh(const Buffer &mesh) {mMesh = mesh;}
    void setMesh(const char *name) {setMesh(std::string(name));}
    void setMesh(const std::string &name) {mMesh = &mScene->getMeshBuffer(name);}
    void setMesh(const XMLTree::Node &node) {mMesh = &mScene->createMesh(node);}
    void uploadMesh()
    {
      if (mMesh.valid())
        mMesh->setToUpdate();
    }
    void clearMesh() {mMesh.clear();}
    
    Buffer& shader()
    {
      if (!mShader.ptr())
        mShader = BUFFER_PROGRAM;
      return *mShader;
    }
    Buffer* shaderPtr() const {return (!mShader.ptr() && mMaterial.ptr()) ? mMaterial->shaderPtr() : mShader.ptr();}
    void setShader(const Buffer &shader) {mShader = shader;}
    void setShader(const char *name) {setShader(std::string(name));}
    void setShader(const std::string &name) {mShader = &mScene->getShaderBuffer(name);}
    void setShader(const XMLTree::Node &node) {mShader = &mScene->createShader(node);}
    void uploadShader()
    {
      if (mShader.valid())
        mShader->setToUpdate();
    }
    void clearShader() {mShader.clear();}
    
    Buffer& frame()
    {
      if (!mFrame.ptr())
        mFrame = BUFFER_FRAME;
      return *mFrame;
    }
    void setFrame(const Buffer &frame) {mFrame = frame;}
    void setFrame(const char *name) {setFrame(std::string(name));}
    void setFrame(const std::string &name) {mFrame = &mScene->getFrameBuffer(name);}
    void setFrame(const XMLTree::Node &node) {mFrame = &mScene->createFrame(node);}
    void uploadFram()
    {
      if (mFrame.valid())
        mFrame->setToUpdate();
    }
    void clearFrame() {mFrame.clear();}
    
    Buffer& uniforms()
    {
      if (!mUniforms.ptr())
        mUniforms = BUFFER_UNIFORM;
      return *mUniforms;
    }
    void setUniforms(const Buffer &uniforms) {mUniforms = uniforms;}
    void setUniforms(const XMLTree::Node &node)
    {
      uniforms().setToXml(node);
      uniforms().setToUpdate();
    }
    
    void setUniform(const XMLTree::Node &node) {uniforms().set(node);}
    void setUniform(const std::string &name, const Variant &var) {uniforms().set(name, var);}
    void setStruct(const std::string &name, const std::string &comp, const Variant &var)
    {
      uniforms().getBuffer(name, BUFFER_STRUCT).set(comp, var);
      uniforms().setToUpdate();
    }
    void setInstanced(const std::string &name, const Variant &var)
    {
      uniforms().getBuffer(name, BUFFER_INSTANCED) = var;
      uniforms().setToUpdate();
    }
    void clearUniforms() {mUniforms.clear();}
    
    Buffer& textures()
    {
      if (!mTextures.ptr())
        mTextures = BUFFER_TEXTURES;
      return *mTextures;
    }
    Buffer* texturesPtr() const {return (!mTextures.ptr() && mMaterial.ptr()) ? mMaterial->texturesPtr() : mTextures.ptr();}
    
    void setTexture(const Buffer &texture, Sampler sampler = Sampler())
    {
      if (!mTextures.valid() || !mTextures->mapSize())
        addTexture(texture, sampler);
      else
      {
        mTextures->front() = texture;
        mTextures->front().setFlags(sampler.flags());
      }
    }
    
    void addTexture(const Buffer &texture) {textures().addBuffer(texture);}
    void addTexture(const Buffer &texture, Sampler sampler)
    {
      addTexture(texture);
      mTextures->back().setFlags(sampler.flags());
    }
    void addTexture(const std::string &name) {addTexture(mScene->getTextureBuffer(name));}
    void addTexture(const std::string &name, Sampler sampler)
    {
      addTexture(name);
      mTextures->back().setFlags(sampler.flags());
    }
    void addTexture(const XMLTree::Node &node)
    {
      if (node.hasAttribute("file"))
        addTexture(mScene->createTexture(node), Sampler(node));
      else
        addTexture(node.attribute("name"), Sampler(node));
    }
    void addTexture(const std::string &name, const std::string &file, Sampler sampler = Sampler())
    {
      addTexture(mScene->createTexture(name, file), sampler);
    }
    void addTextures(const XMLTree::Node &node)
    {
      for (const auto subNode : node)
        addTexture(*subNode);
    }
    void uploadTextures()
    {
      if (mTextures.valid())
      {
        for (auto &texture : *mTextures)
          texture.setToUpdate();
      }
    }
    void clearTextures() {mTextures.clear();}
    
    Material& material()
    {
      if (!mMaterial.ptr())
        mMaterial = mScene;
      return *mMaterial;
    }
    Buffer* materialUniforms() const {return mMaterial.ptr() ? mMaterial->uniformsPtr() : nullptr;}
    void setMaterial(const Material &material) {mMaterial = material;}
    void setMaterial(Material *material) {mMaterial = material;}
    void setMaterial(const std::string &name)
    {
      mMaterial = mScene->getChild<Material>(name);
      if (!mMaterial.ptr())
      {
        mMaterial = new Material(mScene);
        mMaterial->setName(name);
        mScene->addChild(mMaterial.ptr());
      }
    }
    void setMaterial(const XMLTree::Node &node)
    {
      if (node.hasAttribute("name"))
        setMaterial(node.attribute("name"));
      else if (!mMaterial.ptr())
        mMaterial = mScene;
      mMaterial->setToXml(node);
    }
    void clearMaterial() {mMaterial.clear();}
    
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
      
      task.bufferSlots[BUFFER_SLOT_SHADER]   = shaderPtr();
      task.bufferSlots[BUFFER_SLOT_TEXTURES] = mTextures.ptr();
      
      task.bufferSlots[BUFFER_SLOT_UNIFORMS_0] = mUniforms.ptr();
      task.bufferSlots[BUFFER_SLOT_UNIFORMS_1] = materialUniforms();
      
      mGraphicSystem->addGraphicTask(task);
    }
    void updateBuffers()
    {
      if (mShader.valid() && !mShader->updated())
        mGraphicSystem->uploadBuffer(*mShader);
      if (mMesh.valid() && !mMesh->updated())
        mGraphicSystem->uploadBuffer(*mMesh);
      if (mUniforms.valid() && !mUniforms->updated())
        mGraphicSystem->uploadBuffer(*mUniforms);
      if (mFrame.valid() && !mFrame->updated())
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
    OwnPtr<Buffer> mTextures;
    
    OwnPtr<Material> mMaterial;
    
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
    RenderSlots(Scene *scene): Component(scene) {}
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
    
    void addSlot(RenderSlot *slot) {mSlots.push_back(slot);}
    RenderSlot* addSlot()
    {
      RenderSlot *slot = new RenderSlot(mScene);
      addSlot(slot);
      return slot;
    }
    
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
    std::vector<RenderSlot*> mSlots;
  };
}

#endif /* RenderSlots_h */
