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
  
  return success;
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
  Objects::iterator itr = _objects.begin();
  while (itr != _objects.end()) {
    (*itr)->update(td);
    ++itr;
  }
}
