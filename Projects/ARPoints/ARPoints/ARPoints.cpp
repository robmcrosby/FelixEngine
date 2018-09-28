//
//  ARPoints.cpp
//  ARPoints
//
//  Created by Robert Crosby on 2/18/18.
//  Copyright © 2018 Robert Crosby. All rights reserved.
//

#include "ARPoints.h"
#include <FelixEngine/Camera.h>
#include <FelixEngine/Model.h>
#include <FelixEngine/Material.h>


ARPoints::ARPoints() {
  _tracker = nullptr;
}

ARPoints::~ARPoints() {
  
}

void ARPoints::initalize() {
  _scene.loadXMLFile("Scene.xml");
  _renderScheme.push_back("MainPass");
  
  // Load the Point Mesh
  fx::VertexMeshData meshData;
  fx::FileSystem::loadMesh(meshData, "cube.mesh");
  fx::VertexMeshPtr mesh = _graphics->createVertexMesh();
  mesh->load(meshData);
  
  // Setup the Point Model
  _model = _scene.get<fx::Model>("PointModel");
  _model->setMesh(mesh);
  _model->setMaterial("Material");
  _model->setScale(0.01f);
  _model->addToRenderPass("MainPass");
  _model->setInstances(0);
}

void ARPoints::update(float td) {
  _scene.update(td);
  
  if (_tracker != nullptr) {
    const fx::ARPoints &points = _tracker->getPointCloud();
//    _model->setInstances(points.size() > 0 ? (int)points.size() : 1);
    for (int i = 0; i < points.size(); ++i) {
      _pointCloud[points[i].identifier] = points[i].position;
//      _model->setLocation(points[i].position, i);
//      _model->setScale(0.01f, i);
    }
    
    int i = 0;
    _model->setInstances((int)_pointCloud.size());
    //_model->setInstances(points.size() > 0 ? (int)_pointCloud.size() : 1);
    for (const auto &point : _pointCloud) {
      _model->setLocation(point.second, i);
      _model->setScale(0.01f, i);
      ++i;
    }
  }
  
  fx::RenderPass::renderPasses(_renderScheme);
  fx::RenderPass::resetPasses();
}
