//
//  Model.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 7/6/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#include "Model.h"
#include "RenderPass.h"


using namespace fx;
using namespace std;

Model::Model(): _layer(0) {
  _scene = nullptr;
  _scale = vec3(1.0f, 1.0f, 1.0f);
  _uniforms = make_shared<UniformMap>();
  (*_uniforms)["model"] = _transform;
}

Model::~Model() {
  
}

bool Model::loadXML(const XMLTree::Node &node) {
  return true;
}

void Model::update(float dt) {
  _transform.model = mat4::Trans3d(_position) * _orientation.toMat4() * mat4::Scale(_scale);
  _transform.rotation = _orientation;
  (*_uniforms)["model"] = _transform;
  for (auto pass = _renderPasses.begin(); pass != _renderPasses.end(); ++pass)
    (*pass)->addModel(this);
}

void Model::applyToTask(fx::GraphicTask &task) {
  task.uniforms.push_back(_uniforms);
  task.mesh = _mesh;
  
  if (_material)
    _material->setToTask(task);
}

void Model::setMaterial(const std::string &name) {
  if (_scene)
    _material = _scene->get<Material>(name);
}

void Model::setMesh(const std::string &name) {
  _mesh = Graphics::getInstance().getVertexMesh(name);
}


//#include "Scene.h"
//#include "Material.h"
//
//DEFINE_MODEL_BUILDER(Model)
//
//
//using namespace fx;
//using namespace std;
//
//
//Model::Model(Scene *scene): _scene(scene), _data(1), _material(nullptr), _mesh(nullptr), _hidden(false) {
//}
//
//Model::~Model() {
//
//}
//
//void Model::update() {
//  _data[0].model = mat4::Trans3d(_position) * _orientation.toMat4() * mat4::Scale(_scale);
//  _data[0].rotation = _orientation;
//}
//
//bool Model::loadXML(const XMLTree::Node &node) {
//  bool success = true;
//
//  if (node.hasAttribute("mesh"))
//    setMesh(node.attribute("mesh"));
//  if (node.hasAttribute("material"))
//    setMaterial(node.attribute("material"));
//  if (node.hasAttribute("pass"))
//    _scene->renderPasses()[node.attribute("pass")].addModel(this);
//  if (node.hasAttribute("hidden"))
//    setHidden(node.attributeAsBoolean("hidden"));
//
//  for (auto subNode : node) {
//    if (*subNode == "Transform")
//      success &= setTransform(*subNode);
//    //TODO: add sub models, Meshes, Shaders, etc.
//  }
//  return success;
//}
//
//void Model::setMesh(const string &name) {
//  VertexPtr mesh = _scene->getMesh(name);
//  setMesh(mesh);
//}
//
//void Model::setMaterial(const string &name) {
//  setMaterial(_scene->getMaterial(name));
//}
//
//void Model::setShader(ShaderPtr &shader) {
//  if (_material == nullptr)
//    _material = _scene->getMaterial();
//  _material->setShader(shader);
//}
//
//bool Model::setTransform(const XMLTree::Node &node) {
//  if (node.hasAttribute("position"))
//    setPosition(node.attribute("position"));
//  if (node.hasAttribute("orientation"))
//    setOrientation(node.attribute("orientation"));
//  if (node.hasAttribute("scale"))
//    setScale(node.attributeAsFloat("scale"));
//  update();
//  return true;
//}
//
//void Model::enableDepthTesting() {
//  if (_material == nullptr)
//    _material = _scene->getMaterial();
//  _material->enableDepthTesting();
//}
//
//Model* Model::build(const string &type, Scene *scene) {
//  if (builders().count(type))
//    return builders().at(type)->build(scene);
//  return nullptr;
//}
//
//ModelBuilderMap& Model::builders() {
//  static ModelBuilderMap builderMap;
//  return builderMap;
//}

