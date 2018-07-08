//
//  RenderItem.h
//  FelixEngine
//
//  Created by Robert Crosby on 7/7/18.
//  Copyright © 2018 Robert Crosby. All rights reserved.
//

#ifndef RenderItem_h
#define RenderItem_h

#include "Graphics.h"
#include "GraphicResources.h"
#include "GraphicStates.h"
#include "UniformMap.h"
#include "TextureMap.h"
#include "FileSystem.h"


namespace fx {
  /**
   *
   */
  struct RenderItem {
    ShaderProgramPtr shader;
    VertexMeshPtr    mesh;
    
    UniformsList uniforms;
    TexturesPtr textures;
    
    int instances;
    
    CULL_MODE  cullMode;
    DepthState depthState;
    BlendState blendState;
    
    RenderItem(): instances(1), cullMode(CULL_NONE) {}
    
    ShaderProgramPtr getShader() {
      if (!shader)
        shader = Graphics::getInstance().createShaderProgram();
      return shader;
    }
    void setShader(const std::string &name) {shader = Graphics::getInstance().getShaderProgram(name);}
    bool loadShaderFunctions(const std::string &vertex, const std::string &fragment) {return getShader()->loadShaderFunctions(vertex, fragment);}
    
    VertexMeshPtr getMesh() {
      if (!mesh)
        mesh = Graphics::getInstance().createVertexMesh();
      return mesh;
    }
    void setMesh(const std::string &name) {mesh = Graphics::getInstance().getVertexMesh(name);}
    bool loadMeshData(const VertexMeshData &data) {return getMesh()->load(data);}
    bool loadMeshFile(const std::string &file) {
      VertexMeshData data;
      FileSystem::loadMesh(data, file);
      return loadMeshData(data);
    }
    void setMeshPrimativeType(VERTEX_PRIMITIVE type) {getMesh()->setPrimativeType(type);}
    bool setMeshIndexBuffer(size_t count, const int *buffer) {
      return getMesh()->setIndexBuffer(count, buffer);
    }
    bool setMeshVertexBuffer(const std::string &name, size_t size, size_t count, const float *buffer) {
      return getMesh()->setVertexBuffer(name, size, count, buffer);
    }
    bool loadMesh() {return getMesh()->loadBuffers();}
    
    void enableDepthTesting() {depthState.enableDefaultTesting();}
    void enableBlending() {blendState.enableDefaultBlending();}
  };
}

#endif /* RenderItem_h */
