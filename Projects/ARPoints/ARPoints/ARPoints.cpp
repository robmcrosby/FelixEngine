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
}

void ARPoints::update(float td) {
  _scene.update(td);
  
  fx::RenderPass::renderPasses(_renderScheme);
  fx::RenderPass::resetPasses();
}
