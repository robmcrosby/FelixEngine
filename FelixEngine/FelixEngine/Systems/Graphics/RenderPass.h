//
//  RenderPass.h
//  FelixEngine
//
//  Created by Robert Crosby on 7/7/18.
//  Copyright © 2018 Robert Crosby. All rights reserved.
//

#ifndef RenderPass_h
#define RenderPass_h

#include <FelixEngine/iObject.h>
#include <FelixEngine/RenderItem.h>
#include <FelixEngine/Graphics.h>
#include <FelixEngine/XMLTree.h>
#include <set>


namespace fx {
  
  /**
   *
   */
  typedef std::vector<RenderItem> RenderItems;
  
  /**
   *
   */
  struct RenderPass: public iObject {
    FrameBufferPtr frame;
    
    ActionState colorActions[MAX_COLOR_ATTACHEMENTS];
    ActionState depthStencilAction;
    
    UniformsList uniforms;
    TexturesPtr  textures;
    
    bool sortItems;
    bool itemsSorted;
    RenderItems renderItems;
    
    RenderPass(): sortItems(0), itemsSorted(0) {}
    virtual ~RenderPass() {}
    
    void addRenderItem(const RenderItem &item) {
      itemsSorted = false;
      renderItems.push_back(item);
    }
    void clearRenderItems() {
      itemsSorted = true;
      renderItems.clear();
    }
    
    virtual void render() = 0;
    
    FrameBufferPtr getFrame() {
      if (!frame)
        frame = Graphics::getInstance().createFrameBuffer();
      return frame;
    }
    ivec2 getFrameSize() {return getFrame()->size();}
    void setFrame(const std::string &name) {frame = Graphics::getInstance().getFrameBuffer(name);}
    bool setFrame(const XMLTree::Node &node) {
      if (node.hasAttribute("name"))
        setFrame(node.attribute("name"));
      return getFrame()->load(node);
    }
    bool setFrameToWindow(int index) {return getFrame()->setToWindow(index);}
    bool resizeFrame(int width, int height) {return getFrame()->resize(width, height);}
    bool addColorTexture() {return getFrame()->addColorTexture();}
    bool addDepthBuffer() {return getFrame()->addDepthBuffer();}
    TextureBufferPtr getColorTexture(int index = 0) {return getFrame()->getColorTexture(0);}
    
    bool setClearSettings(const XMLTree::Node &node) {
      if (node.hasAttribute("color"))
        setClearColor(node.attribute("color"));
      if (node.hasAttribute("depth"))
        setClearDepthStencil(node.attributeAsFloat("depth"));
      return true;
    }
    void setClearColor(const vec4 &color) {
      for (int i = 0; i < MAX_COLOR_ATTACHEMENTS; ++i) {
        colorActions[i].loadAction = LOAD_CLEAR;
        colorActions[i].clearColor = color;
      }
    }
    void setDefaultColorActions() {
      for (int i = 0; i < MAX_COLOR_ATTACHEMENTS; ++i) {
        colorActions[i].loadAction = LOAD_LAST;
        colorActions[i].storeAction = STORE_SAVE;
        colorActions[i].clearColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
      }
    }
    void setClearDepthStencil(float depth = 1.0f, int stencil = 0) {
      depthStencilAction.loadAction = LOAD_CLEAR;
      depthStencilAction.clearColor = vec4(depth, (float)stencil, 0.0f, 1.0f);
    }
    void setDefaultDepthStencilActions() {
      depthStencilAction.loadAction = LOAD_LAST;
      depthStencilAction.storeAction = STORE_SAVE;
      depthStencilAction.clearColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
    }
    
    void addUniformMap(UniformsPtr uniformMap) {uniforms.push_back(uniformMap);}
    UniformMap& getUniformMap(int index = 0) {
      while (uniforms.size() <= index)
        uniforms.push_back(UniformMap::make());
      return *uniforms.at(index).get();
    }
    void clearUniformMaps() {uniforms.clear();}
    
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
    
    virtual void setScene(Scene *scene) {}
    virtual void update(float dt) {}
    virtual bool load(const XMLTree::Node &node) {
      bool success = true;
      if (node.hasAttribute("frame"))
        setFrame(node.attribute("frame"));
      if (node.hasAttribute("sorted"))
        sortItems = node.attributeAsBoolean("sorted");
      for (auto subNode : node)
        success &= loadXMLItem(*subNode);
      return success;
    }
    
    bool loadXMLItem(const XMLTree::Node &node) {
      if (node == "Frame")
        return setFrame(node);
      if (node == "Clear")
        return setClearSettings(node);
      std::cerr << "Unknown XML Node in RenderPass:" << std::endl << node << std::endl;
      return false;
    }
    
    void sortRenderItems() {
      if (sortItems && !itemsSorted) {
        sort(renderItems.begin(), renderItems.end());
        itemsSorted = true;
      }
    }
  };
}

#endif /* RenderPass_h */
