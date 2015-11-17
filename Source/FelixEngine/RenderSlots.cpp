//
//  RenderSlots.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 10/10/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#include "RenderSlots.h"
#include "FelixEngine.h"
#include "GraphicSystem.h"
#include "Scene.h"


using namespace std;
using namespace fx;


RenderSlots::RenderSlots(Object *obj): Component("RenderSlots", obj)
{
  mGraphicSystem = FelixEngine::GetGraphicSystem();
}

RenderSlots::~RenderSlots()
{
  clear();
}

bool RenderSlots::setToXml(const XMLTree::Node *node)
{
  bool success = Component::setToXml(node);
  if (success && node)
  {
    for (XMLTree::const_iterator itr = node->begin(); itr != node->end(); ++itr)
    {
      addSlot();
      success &= back()->setToXml(*itr);
    }
  }
  return success;
}

bool RenderSlots::init()
{
  return Component::init();
}

void RenderSlots::addSlot()
{
  RenderSlot *slot = new RenderSlot(mObject->getScene());
  slot->setLocalUniforms(&mLocalUniforms);
  mSlots.push_back(slot);
}

void RenderSlots::clear()
{
  for (iterator itr = begin(); itr != end(); ++itr)
    delete *itr;
  mSlots.clear();
}









RenderSlot::RenderSlot(Scene *scene): mVisible(true), mLayer(0), mSubMesh(0), mViewIndex(-1), mFrame(0), mPassIndex(0),
  mInstances(1), mMesh(0), mScene(scene), mGraphicSystem(FelixEngine::GetGraphicSystem()), mLocalUniforms(0), mMaterial(0)
{
  setEventFlags(EVENT_APP_RENDER);
  mGraphicSystem->addHandler(this);
}

RenderSlot::~RenderSlot()
{
}

void RenderSlot::handle(const fx::Event &event)
{
  if (event == EVENT_APP_RENDER && event.sender() == mGraphicSystem)
    render();
}

bool RenderSlot::setMesh(const XMLTree::Node &node)
{
  bool success = false;
  if (node.hasAttribute("name"))
  {
    setMesh(node.attribute("name"));
    success = mMesh->setToXml(node);
  }
  return success;
}

bool RenderSlot::setFrame(const XMLTree::Node &node)
{
  bool success = false;
  if (node.hasAttribute("name"))
  {
    setFrame(node.attribute("name"));
    success = mFrame->setToXml(node);
  }
  return success;
}

bool RenderSlot::setMaterial(const XMLTree::Node &node)
{
  if (node.hasAttribute("name"))
    setMaterial(node.attribute("name"));
  else
    mMaterial = new Material();
  return mMaterial->setToXml(node);
}

void RenderSlot::setMesh(const string &name)
{
  if (mGraphicSystem)
    setMesh(mGraphicSystem->getMesh(name));
}

void RenderSlot::setFrame(const string &name)
{
  if (mGraphicSystem)
    setFrame(mGraphicSystem->getFrame(name));
}

void RenderSlot::setMaterial(const string &name)
{
  if (mScene)
    setMaterial(mScene->getMaterial(name));
}

void RenderSlot::setPass(const string &pass)
{
  mPass = pass;
  mPassIndex = mPass == "" ? 0 : GetPassIndex(mPass);
}

int RenderSlot::GetPassIndex(const std::string &pass)
{
  static int counter = 0;
  static map<string, int> passes;
  
  if (!passes.count(pass))
    passes[pass] = ++counter;
  return passes[pass];
}


bool RenderSlot::setToXml(const XMLTree::Node *node)
{
  bool success = false;
  if (node)
  {
    success = true;
    if (node->hasAttribute("layer"))
      setLayer(node->attributeAsInt("layer"));
    if (node->hasAttribute("subMesh"))
      setSubMesh(node->attributeAsInt("subMesh"));
    if (node->hasAttribute("instances"))
      setInstances(node->attributeAsInt("instances"));
    
    // Set the States
    if (node->hasSubNode("DepthState"))
      success &= mDepthState.setToXml(*node->subNode("DepthState"));
    if (node->hasSubNode("BlendState"))
      success &= mBlendState.setToXml(*node->subNode("BlendState"));
    if (node->hasSubNode("ClearState"))
      success &= mClearState.setToXml(*node->subNode("ClearState"));
    
    // Set the Mesh
    if (node->hasAttribute("mesh"))
      setMesh(node->attribute("mesh"));
    else if (node->hasSubNode("Mesh"))
      success &= setMesh(*node->subNode("Mesh"));
    
    // Set the Pass
    if (node->hasAttribute("pass"))
      setPass(node->attribute("pass"));
    
    // Set the Frame
    if (node->hasAttribute("frame"))
      setFrame(node->attribute("frame"));
    else if (node->hasSubNode("Frame"))
      setFrame(*node->subNode("Frame"));
    
    // Set the Material
    if (node->hasAttribute("material"))
      setMaterial(node->attribute("material"));
    else if (node->hasSubNode("Material"))
      setMaterial(*node->subNode("Material"));
  }
  return success;
}

void RenderSlot::render() const
{
  GraphicTask task;
  if (mGraphicSystem && applyToTask(task))
    mGraphicSystem->addGraphicTask(task);
}

bool RenderSlot::applyToTask(GraphicTask &task) const
{
  // Return false if not visible.
  if (!mVisible)
    return false;
  
  // Assign the items to the task.
  task.layer      = mLayer;
  task.subMesh    = mSubMesh;
  task.instances  = mInstances;
  
  task.depthState = mDepthState;
  task.blendState = mBlendState;
  task.clearState = mClearState;
  
  task.mesh = mMesh;
  task.frame = mFrame;
  task.pass = mPassIndex;
  
  if (mLocalUniforms)
    task.localUniforms = mLocalUniforms->getInternalMap();
  else
    task.localUniforms = nullptr;

  if (mMaterial)
    mMaterial->applyToTask(task);
  else
  {
    task.shader = nullptr;
    task.textureMap = nullptr;
    task.materialUniforms = nullptr;
  }
  
  return true;
}
