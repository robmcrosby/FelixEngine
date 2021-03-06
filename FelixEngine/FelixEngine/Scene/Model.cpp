//
//  Model.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 7/6/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#include "Model.h"
#include "MeshBuilder.h"
#include "USDCrate.h"


using namespace fx;
using namespace std;

ModelBuilder Model::modelBuilder = ModelBuilder();

Model::Model(): _hidden(0) {
  _scene = nullptr;
  _transforms.push_back(Transform::make());
  _textureTransforms.push_back(mat4());
  
  _uniforms = UniformMap::make();
  (*_uniforms)["model"] = STR_Model();
  _renderItem.uniforms.push_back(_uniforms);
}

Model::~Model() {
  
}

bool Model::load(const XMLTree::Node &node) {
  bool success = true;
  if (node.hasAttribute("mesh"))
    setMesh(node.attribute("mesh"));
  if (node.hasAttribute("material"))
    setMaterial(node.attribute("material"));
  if (node.hasAttribute("pass"))
    setToRenderPass(node.attribute("pass"));
  if (node.hasAttribute("hidden"))
    setHidden(node.attributeAsBoolean("hidden"));
  if (node.hasAttribute("layer"))
    setLayer(node.attributeAsInt("layer"));
  for (auto subNode : node)
    success &= loadXMLItem(*subNode);
  return success;
}

bool Model::load(const USDItem &item, const std::string &pass) {
  bool success = true;
  
  //cout << "Load Model Item" << endl;
  //cout << item << endl;
  
  const USDCrate &crate = *item.crate;
  string meshPath = crate.getFirstMeshPath(item.pathString);
  if (meshPath != "") {
    setMesh(crate.getName(meshPath));
    string materialPath = crate.getMaterialPath(meshPath);
    if (materialPath != "")
      setMaterial(crate.getName(materialPath));
  }
  
//  _transforms.front()->setRotation(quat::RotX(-1.5f));
  
  if (item.getAttribute("xformOpOrder"))
    _transforms.front()->load(item);
  
//  const USDItem *transformItem = item.getAttribute("xformOp:transform");
//  if (transformItem) {
//    double4x4 transform = transformItem->double4x4Value();
//    //_transforms.front() = transform;
//  }
  
  if (pass != "")
    setToRenderPass(pass);
  
  setFaceCulling(CULL_FRONT);
  
  return success;
}

bool Model::loadXMLItem(const XMLTree::Node &node) {
  if (node == "Transform" && _transforms.size() > 0)
    return _transforms.front()->load(node);
  else if (node == "Mesh")
    return setMesh(node);
  else if (node == "Material")
    return setMaterial(node);
  else {
    SharedObject obj = _scene->build(node);
    MaterialPtr material(dynamic_pointer_cast<Material>(obj));
    if (material) {
      setMaterial(material);
      return true;
    }
    cerr << "Unkown XML Node in Model:" << endl << node << endl;
  }
  return false;
}

void Model::update(float dt) {
  Uniform &models = (*_uniforms)["model"];
  models.resize(_transforms.size());
  for (int i = 0; i < _transforms.size(); ++i) {
    STR_Model &model = (STR_Model&)models[i];
    model.model = _transforms.at(i)->transform();
    model.rotation = _transforms.at(i)->rotation().toVec4();
    //model.model = globalModelTransform(i);
    //model.texture = textureTransform(i);
    //model.rotation = _transforms.at(i)->globalRotation().toVec4();
  }
  _uniforms->update();
  
  if (renderable()) {
    // Make copy of the Model's Render Item
    RenderItem item = _renderItem;
    
    if (_material) {
      // Set Render Item's properties from the material
      item.shader = _material->shader();
      item.uniforms.push_back(_material->uniforms());
      item.textures = _material->textures();
      item.depthState = _material->depthState();
      item.blendState = _material->blendState();
    }
    else {
      // TODO: Set Render Items's properties with out the material
    }
    
    // Add the Render Item to each of the Render Passes
    for (auto &pass : _renderPasses)
      pass->addRenderItem(item);
  }
}

bool Model::renderable() const {
  if (!_hidden && _renderItem.instances > 0 && _renderItem.mesh && _renderItem.texturesLoaded())
    return _renderItem.shader || (_material && _material->shader());
  return false;
}

void Model::setInstances(unsigned int instances) {
  while (instances > _transforms.size()) {
    _transforms.push_back(Transform::make());
    _textureTransforms.push_back(mat4());
  }
  _renderItem.instances = instances;
  
  if (!(*_uniforms)["model"].usingBuffer() && instances > 4) {
    (*_uniforms)["model"].useBuffer();
  }
}

bool Model::setMaterial(const XMLTree::Node &node) {
  _material = _scene->get<Material>(node.attribute("name"));
  return _material->load(node);
}

void Model::setMaterial(const std::string &name) {
  if (_scene)
    _material = _scene->get<Material>(name);
}

bool Model::setMesh(const XMLTree::Node &node) {
  _renderItem.mesh = Graphics::getInstance().getVertexMesh(node.attribute("name"));
  return _renderItem.mesh->load(node);
}

void Model::setMesh(const std::string &name) {
  _renderItem.mesh = Graphics::getInstance().getVertexMesh(name);
}

void Model::loadMesh(const std::string &file) {
  _renderItem.mesh = MeshBuilder::createFromFile(file);
}

//void Model::setTextureTransform(const mat4 &transform, int index) {
//  if (index < _textureTransforms.size())
//    _textureTransforms[index] = transform;
//}
//
//mat4 Model::textureTransform(int index) {
//  if (index < _textureTransforms.size())
//    return _textureTransforms.at(index);
//  return mat4();
//}
