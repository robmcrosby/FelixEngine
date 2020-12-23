//
//  RenderItem.h
//  FelixEngine
//
//  Created by Robert Crosby on 7/7/18.
//  Copyright © 2018 Robert Crosby. All rights reserved.
//

#ifndef RenderItem_h
#define RenderItem_h

#include <FelixEngine/Graphics.h>
#include <FelixEngine/GraphicResources.h>
#include <FelixEngine/GraphicStates.h>
#include <FelixEngine/UniformMap.h>
#include <FelixEngine/TextureMap.h>
#include <FelixEngine/FileSystem.h>


namespace fx {
  /**
   *
   */
  struct RenderItem {
    ShaderProgramPtr shader;
    VertexMeshPtr    mesh;
    
    UniformsList uniforms;
    TexturesPtr  textures;
    
    int instances;
    int layer;
    
    CULL_MODE  cullMode;
    DepthState depthState;
    BlendState blendState;
    
    RenderItem(): instances(1), layer(0), cullMode(CULL_NONE) {}
    
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
    
    void addUniformMap(UniformsPtr uniformMap) {uniforms.push_back(uniformMap);}
    UniformMap& getUniformMap(int index = 0) {
      while (uniforms.size() <= index)
        uniforms.push_back(UniformMap::make());
      return *uniforms.at(index).get();
    }
    
    TextureMap& getTextureMap() {
      if (!textures)
        textures = TextureMap::make();
      return *textures;
    }
    void setTexture(const std::string &name, const RGBA &color) {getTextureMap().setColor(name, color);}
    void setTexture(const std::string &name, TextureBufferPtr texture, SamplerState sampler = SamplerState()) {
      getTextureMap().setTexture(name, texture, sampler);
    }
    void setTexture(const std::string &name, ImageBufferData &imageData, SamplerState sampler = SamplerState()) {
      getTextureMap().setTexture(name, imageData, sampler);
    }
    bool setTexture(const std::string &name, const std::string &file, SamplerState sampler = SamplerState()) {
      return getTextureMap().setTexture(name, file, sampler);
    }
    bool setCubeMap(const std::string &name, const std::vector<std::string> &files, SamplerState sampler = SamplerState()) {
      return getTextureMap().setCubeMap(name, files, sampler);
    }
    bool setCubeMap(const std::string &name, const std::string &file, SamplerState sampler = SamplerState()) {
      return getTextureMap().setCubeMap(name, file, sampler);
    }
    bool texturesLoaded() const {return !textures || textures->loaded();}
    
    bool operator<(const RenderItem &other) const {return layer < other.layer;}
  };
}

#endif /* RenderItem_h */
