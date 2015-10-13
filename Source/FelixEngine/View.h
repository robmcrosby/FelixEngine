//
//  View.h
//  FelixEngine
//
//  Created by Robert Crosby on 10/12/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef View_h
#define View_h

#include "FelixEngine.h"
#include "UniformMap.h"
#include "GraphicTask.h"


namespace fx
{
  class View
  {
  public:
    View(): mIndex(1), mLayer(0), mFrame(0), mSystem(FelixEngine::GetGraphicSystem()) {}
    ~View() {}
    
    bool setFrame(const XMLTree::Node &node)
    {
      bool success = false;
      if (node.hasAttribute("name"))
      {
        setFrame(node.attribute("name"));
        success = mFrame->setToXml(node);
      }
      return success;
    }
    
    void setFrame(Frame *frame) {mFrame = frame;}
    void setFrame(const std::string &name)
    {
      if (mSystem)
        setFrame(mSystem->getFrame(name));
    }
    
    Frame* frame() {return mFrame;}
    const Frame* frame() const {return mFrame;}
    
    void setIndex(int index) {mIndex = index;}
    int index() const {return mIndex;}
    
    void setLayer(int layer) {mLayer = layer;}
    int layer() const {return mLayer;}
    
    ClearState& clearState() {return mClearState;}
    const ClearState& clearState() const {return mClearState;}
    
    UniformMap& uniformMap() {return mUniformMap;}
    const UniformMap& uniformMap() const {return mUniformMap;}
    
    Uniform& operator[](const std::string &key) {return mUniformMap[key];}
    
    void render() const
    {
      if (mSystem)
      {
        GraphicTask task;
        task.frame = mFrame;
        task.layer = mLayer;
        task.clearState = mClearState;
        task.viewIndex = mIndex;
        mSystem->addGraphicTask(task);
      }
    }
    
    bool applyToTask(GraphicTask &task) const
    {
      if (!mFrame || mIndex < 0)
        return false;
      task.frame = mFrame;
      task.viewIndex = mIndex;
      task.viewUniforms = mUniformMap.getInternalMap();
      return true;
    }
    
    bool setToXml(const XMLTree::Node *node) {return node && setToXml(*node);}
    bool setToXml(const XMLTree::Node &node)
    {
      bool success = true;
      if (node.hasAttribute("index"))
        setIndex(node.attributeAsInt("index"));
      if (node.hasAttribute("layer"))
        setLayer(node.attributeAsInt("layer"));
      
      if (node.hasAttribute("frame"))
        setFrame(node.attribute("frame"));
      else if (node.hasSubNode("Frame"))
        success &= setFrame(*node.subNode("Frame"));
      
      if (node.hasSubNode("ClearState"))
        success &= mClearState.setToXml(*node.subNode("ClearState"));
      if (node.hasSubNode("UniformMap"))
        success &= mUniformMap.setToXml(*node.subNode("UniformMap"));
      
      return success;
    }
    
  private:
    int mIndex, mLayer;
    Frame *mFrame;
    ClearState mClearState;
    UniformMap mUniformMap;
    GraphicSystem *mSystem;
  };
}

#endif /* ViewMap_h */
