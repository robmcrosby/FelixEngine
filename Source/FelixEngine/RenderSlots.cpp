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


DEFINE_COMPONENT_ID(RenderSlots);

RenderSlots::RenderSlots(Object *obj): Component("RenderSlots", obj)
{
  mGraphicSystem = dynamic_cast<GraphicSystem*>(FelixEngine::GetSystem(SYSTEM_GRAPHICS));
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

void RenderSlots::update()
{
  for (iterator itr = begin(); itr != end(); ++itr)
    (*itr)->update();
}

void RenderSlots::addSlot()
{
  mSlots.push_back(new RenderSlot(mObject->getScene()));
}

void RenderSlots::clear()
{
  for (iterator itr = begin(); itr != end(); ++itr)
    delete *itr;
  mSlots.clear();
}




RenderSlot::RenderSlot(Scene *scene): mVisible(true), mLayer(0), mSubMesh(0), mViewIndex(-1),
  mInstances(1), mMesh(0), mScene(scene), mSystem(FelixEngine::GetGraphicSystem()), mUniformMapPtr(0)
{
  setToInternalView();
  setToInternalMaterial();
}

RenderSlot::~RenderSlot()
{
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

bool RenderSlot::setView(const XMLTree::Node &node)
{
  if (node.hasAttribute("name"))
    setView(node.attribute("name"));
  else
    setToInternalView();
  return mViewPtr->setToXml(node);
}

bool RenderSlot::setMaterial(const XMLTree::Node &node)
{
  if (node.hasAttribute("name"))
    setMaterial(node.attribute("name"));
  else
    setToInternalView();
  return mMaterialPtr->setToXml(node);
}

void RenderSlot::setMesh(const string &name)
{
  if (mSystem)
    setMesh(mSystem->getMesh(name));
}

void RenderSlot::setView(const string &name)
{
  if (mScene)
    setView(mScene->getView(name));
}

void RenderSlot::setMaterial(const string &name)
{
  if (mScene)
    setMaterial(mScene->getMaterial(name));
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
    
    // Set the View
    if (node->hasAttribute("view"))
      setView(node->attribute("view"));
    else if (node->hasSubNode("View"))
      setView(*node->subNode("View"));
    
    // Set the Material
    if (node->hasAttribute("material"))
      setMaterial(node->attribute("material"));
    else if (node->hasSubNode("Material"))
      setMaterial(*node->subNode("Material"));
  }
  return success;
}

void RenderSlot::update() const
{
  GraphicTask task;
  if (mSystem && applyToTask(task))
    mSystem->addGraphicTask(task);
  
  mView.update();
}

bool RenderSlot::applyToTask(GraphicTask &task) const
{
  // Return false if not visible.
  if (!mVisible)
    return false;
  
  // Validate the needed items for the task.
  if (mSubMesh < 0 || mInstances <= 0 || !mMesh || !mViewPtr || !mMaterialPtr)
    return false;
  
  // Assign the items to the task.
  task.layer      = mLayer;
  task.subMesh    = mSubMesh;
  task.instances  = mInstances;
  
  task.depthState = mDepthState;
  task.blendState = mBlendState;
  task.clearState = mClearState;
  
  task.mesh = mMesh;
  
  // Assign an optional local UniformMap.
  if (mUniformMapPtr)
    task.localUniforms = mUniformMapPtr->getInternalMap();
  else
    task.localUniforms = nullptr;
  
  // Attempt to apply the View and Matieral to the task.
  return mViewPtr->applyToTask(task) && mMaterialPtr->applyToTask(task);
}
