//
//  Scene.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 7/6/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//


#include "Scene.h"
#include "FileSystem.h"
#include "USDArchive.h"
#include "MeshLoader.h"
#include "Model.h"
#include "PbrMaterial.h"


using namespace fx;
using namespace std;

Scene::Scene() {
  
}

Scene::~Scene() {
  
}

bool Scene::loadXMLFile(const std::string &file, const string &pass) {
  XMLTree tree;
  if (FileSystem::loadXMLTree(tree, file)) {
    for (auto node : tree) {
      if (*node == "Scene")
        return load(*node);
    }
  }
  return false;
}

bool Scene::load(const XMLTree::Node &node) {
  bool success = true;
  for (auto &subNode : node)
    success &= loadObject(*subNode);
  return success;
}

bool Scene::loadObject(const XMLTree::Node &node) {
  if (node == "RenderPass")
    return getRenderingPass(node.attribute("name"))->load(node);
  if (node == "Shader")
    return getShaderProgram(node.attribute("name"))->load(node);
  if (node == "Frame")
    return getFrameBuffer(node.attribute("name"))->load(node);
  if (node == "Mesh")
    return getVertexMesh(node.attribute("name"))->load(node);
  if (node == "Texture")
    return getTextureBuffer(node.attribute("name"))->load(node);
  if (node == "External")
    return loadExternal(node);
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
    obj->load(node);
  return obj;
}

bool Scene::loadExternal(const XMLTree::Node &node) {
  if (node.hasAttribute("file")) {
    string file = node.attribute("file");
    string fileType = FileSystem::getFilePostfix(file);
    if (fileType == "xml")
      return loadXMLFile(file, node.attribute("pass"));
    if (fileType == "usdz")
      return loadUSDZFile(file, node.attribute("pass"));
  }
  return false;
}

bool Scene::loadUSDZFile(const string &file, const string &pass) {
  bool success = true;
  string filePath = FileSystem::findFile(file);
  USDArchive archive;
  if (archive.read(filePath)) {
    // Load All Image Files From USD Archive
    StringVector images = archive.imageFiles();
    for (auto &name : images) {
      ImageBufferData imageData;
      success &= archive.loadImage(imageData, name) && getTextureBuffer(name)->load(imageData);
    }
    
    // Load Objects from USD Crate Files
    StringVector crates = archive.crateFiles();
    for (auto &name : crates) {
      USDCrate crate = archive.getUSDCrate(name);
      success &= loadUSDCrate(crate, pass);
    }
  }
  else {
    cerr << "Error Opening USDZ File" << endl;
    success = false;
  }
  return success;
}

bool Scene::loadUSDCrate(const USDCrate &crate, const string &pass) {
  bool success = true;
  
  // Load Materials
  StringVector materials = crate.materialPaths();
  for (auto &path : materials)
    success &= get<PbrMaterial>(crate.getName(path))->load(crate, path);
  
  // Load Meshes
  StringVector meshes = crate.meshPaths();
  for (auto &path : meshes)
    success &= getVertexMesh(crate.getName(path))->load(crate, path);
  
  // Load Models
  StringVector models = crate.objectPaths();
  for (auto &path : models)
    success &= get<Model>(crate.getName(path))->load(crate, path, pass);
  
  //crate.printUSD();
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
  if (name == "")
    return createRenderingPass();
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
  renderPass->load(node);
  return renderPass;
}

ShaderProgramPtr Scene::getShaderProgram(const std::string &name) {
  ShaderProgramPtr shader = Graphics::getInstance().getShaderProgram(name);
  SharedObject object = static_pointer_cast<iObject>(shader);
  addObject(object);
  return shader;
}

FrameBufferPtr Scene::getFrameBuffer(const std::string &name) {
  FrameBufferPtr frame = Graphics::getInstance().getFrameBuffer(name);
  SharedObject object = static_pointer_cast<iObject>(frame);
  addObject(object);
  return frame;
}

VertexMeshPtr Scene::getVertexMesh(const string &name) {
  VertexMeshPtr mesh = Graphics::getInstance().getVertexMesh(name);
  SharedObject object = static_pointer_cast<iObject>(mesh);
  addObject(object);
  return mesh;
}

TextureBufferPtr Scene::getTextureBuffer(const string &name) {
  TextureBufferPtr texture = Graphics::getInstance().getTextureBuffer(name);
  SharedObject object = static_pointer_cast<iObject>(texture);
  addObject(object);
  return texture;
}

ScenePtr Scene::make() {
  return make_shared<Scene>();
}

BuilderMap& Scene::getBuilderMap() {
  static BuilderMap instance;
  return instance;
}
