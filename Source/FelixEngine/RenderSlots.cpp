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
#include "View.h"


using namespace std;
using namespace fx;


DEFINE_COMPONENT_ID(RenderSlots);

RenderSlots::RenderSlots(Object *obj): Component("RenderSlots", obj)
{
  mGraphicSystem = dynamic_cast<GraphicSystem*>(FelixEngine::GetSystem(SYSTEM_GRAPHICS));
}

RenderSlots::~RenderSlots()
{
}

bool RenderSlots::setToXml(const XMLTree::Node *node)
{
  return Component::setToXml(node);
}

bool RenderSlots::init()
{
  return Component::init();
}





RenderSlot::RenderSlot(Scene *scene): mVisible(true), mLayer(0), mSubMesh(0), mViewIndex(-1),
  mInstances(1), mMesh(0), mView(0), mScene(scene), mSystem(FelixEngine::GetGraphicSystem())
{
  setToInternalMaterial();
}

RenderSlot::~RenderSlot()
{
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
  return true;
}

bool RenderSlot::applyToTask(GraphicTask &task) const
{
  // Return false if not visible.
  if (!mVisible)
    return false;
  
  // Validate the needed items for the task.
  if (mSubMesh < 0 || mInstances <= 0 || !mMesh || !mView || !mMaterialPtr)
    return false;
  
  // Assign the items to the task.
  task.layer      = mLayer;
  task.subMesh    = mSubMesh;
  task.instances  = mInstances;
  task.mesh       = mMesh;
  task.depthState = mDepthState;
  task.blendState = mBlendState;
  
  // Assign an optional local UniformMap.
  if (mUniformMapPtr)
    task.localUniforms = mUniformMapPtr->getInternalMap();
  else
    task.localUniforms = nullptr;
  
  // Attempt to apply the View and Matieral to the task.
  return mView->applyToTask(task) && mMaterialPtr->applyToTask(task);
}
