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
    
    void setFrame(Frame *frame) {mFrame = frame;}
    void setFrame(const std::string &name)
    {
      if (mSystem)
        setFrame(mSystem->getFrame(name));
    }
    
    Frame* frame() {return mFrame;}
    const Frame* frame() const {return mFrame;}
    
    int layer() const {return mLayer;}
    void setLayer(int layer) {mLayer = layer;}
    
    void setClearState(const ClearState &state) {mClearState = state;}
    ClearState clearState() const {return mClearState;}
    
    UniformMap& uniforms() {return mUniforms;}
    const UniformMap& uniforms() const {return mUniforms;}
    
    Variant& operator[](const std::string &key) {return mUniforms[key];}
    
    int index() const {return mIndex;}
    
    void render() const
    {
      if (mSystem)
      {
        GraphicTask task;
        task.mFrame = mFrame;
        task.mLayer = mLayer;
        task.mClearState = mClearState;
        task.mViewIndex = mIndex;
        mSystem->addGraphicTask(task);
      }
    }
    
    bool setToXml(const XMLTree::Node &node)
    {
      bool success = mIndex = node.attributeAsInt("index");
      
      std::string frame = node.attribute("frame");
      if (frame == "")
      {
        std::cerr << "ClearState Node missing frame attribute" << std::endl;
        success = false;
      }
      else
        setFrame(frame);
      
      if (node.hasSubNode("ClearState"))
        success &= mClearState.setToXml(*node.subNode("ClearState"));
      
      if (node.hasAttribute("layer"))
        setLayer(node.attributeAsInt("layer"));
      
      return success;
    }
    
  private:
    int mIndex, mLayer;
    Frame *mFrame;
    ClearState mClearState;
    UniformMap mUniforms;
    GraphicSystem *mSystem;
  };
}

#endif /* ViewMap_h */
