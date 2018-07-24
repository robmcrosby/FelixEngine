//
//  Scene.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 7/6/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//


#include "Scene.h"
#include "FileSystem.h"

using namespace fx;
using namespace std;

Scene::Scene() {
  
}

Scene::~Scene() {
  
}

bool Scene::loadXMLFile(const std::string &file) {
  XMLTree tree;
  if (FileSystem::loadXMLTree(tree, file)) {
    for (auto node : tree) {
      if (*node == "Scene")
        return loadXML(*node);
    }
  }
  return false;
}

bool Scene::loadXML(const XMLTree::Node &node) {
  bool success = true;
  for (auto &subNode : node)
    success &= loadObject(*subNode);
  return success;
  
//  for (auto subNode : node) {
//    if (*subNode == "FrameBuffer") {
//      if (subNode->hasAttribute("window"))
//        success &= addWindow(*subNode);
//      else
//        success &= addFrame(*subNode);
//    }
//    else if (*subNode == "ShaderProgram")
//      success &= addShader(*subNode);
//    else if (*subNode == "VertexMesh")
//      success &= addMesh(*subNode);
//    else if (*subNode == "LightRig")
//      success &= addLightRig(*subNode);
//    else if (*subNode == "Material")
//      success &= addMaterial(*subNode);
//    else
//      success &= addModel(*subNode) || addCamera(*subNode);
//  }
}

bool Scene::loadObject(const XMLTree::Node &node) {
  if (node == "RenderPass") {
    RenderPassPtr renderPass = getRenderingPass(node);
    return renderPass ? true : false;
  }
  SharedObject obj = build(node);
  return obj ? true : false;
}

SharedObject Scene::build(const std::string &type, const std::string &name) {
  BuilderMap &builders = getBuilderMap();
  if (builders.count(type))
    return builders[type]->build(this, name);
  return SharedObject();
}

SharedObject Scene::build(const XMLTree::Node &node) {
  SharedObject obj = build(node.element(), node.attribute("name"));
  if (obj)
    obj->loadXML(node);
  return obj;
}

void Scene::addObject(SharedObject &obj, const string &name) {
  obj->setScene(this);
  _objects.insert(obj);
  if (name != "")
    _objectMap[name] = obj;
}

void Scene::removeObject(SharedObject &obj) {
  _objects.erase(obj);
}

void Scene::removeObject(const string &name) {
  if (name != "") {
    SharedObject obj = _objectMap[name].lock();
    if (obj)
      _objects.erase(obj);
    _objectMap.erase(name);
  }
}

void Scene::update(float td) {
  // Update each of the objects in the Scene
  Objects::iterator itr = _objects.begin();
  while (itr != _objects.end()) {
    (*itr)->update(td);
    ++itr;
  }
  
  // Render each of the Render Passes
  for (auto &pass : _renderingPasses)
    pass->render();
  
  // Clear each of the Render Passes
  for (auto &pass : _renderingPasses)
    pass->clearRenderItems();
}

void Scene::addRenderingPass(RenderPassPtr renderPass) {
  _renderingPasses.push_back(renderPass);
}

void Scene::clearRenderingPasses() {
  _renderingPasses.clear();
  _renderingPassMap.clear();
}

RenderPassPtr Scene::createRenderingPass() {
  RenderPassPtr renderPass = Graphics::getInstance().createRenderPass();
  addRenderingPass(renderPass);
  return renderPass;
}

RenderPassPtr Scene::getRenderingPass(const string &name) {
  if (!_renderingPassMap.count(name))
    _renderingPassMap[name] = createRenderingPass();
  return _renderingPassMap.at(name);
}

RenderPassPtr Scene::getRenderingPass(const XMLTree::Node &node) {
  RenderPassPtr renderPass;
  if (node.hasAttribute("name"))
    renderPass = getRenderingPass(node.attribute("name"));
  else
    renderPass = createRenderingPass();
  renderPass->loadXML(node);
  return renderPass;
}

ScenePtr Scene::make() {
  return make_shared<Scene>();
}

BuilderMap& Scene::getBuilderMap() {
  static BuilderMap instance;
  return instance;
}
