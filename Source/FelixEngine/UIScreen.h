//
//  UIScreen.h
//  FelixEngine
//
//  Created by Robert Crosby on 12/15/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef UIScreen_h
#define UIScreen_h

#include "Component.h"
#include "RenderSlots.h"
#include "Projection.h"
#include "View.h"


namespace fx
{
  /**
   *
   */
  class UIScreen: public Component
  {
  public:
    UIScreen(Scene *scene): Component(scene), mWindow(0), mRenderSlots(0), mProjection(0), mView(0), mScale(1.0f)
    {
      setEventFlags(EVENT_WINDOW);
    }
    virtual ~UIScreen() {}
    
    virtual void setToXml(const XMLTree::Node &node)
    {
      Component::setToXml(node);
      addChildren(node);
      mRenderSlots = getChild<RenderSlots>();
      mProjection = getChild<Projection>();
      mView = getChild<View>();
      
      if (node.hasAttribute("window"))
        setWindow(FelixEngine::GetGraphicSystem()->getWindow(node.attribute("window")));
      else if (node.hasAttribute("size"))
        resize(node.attribute("size"));
    }
    
    virtual bool init()
    {
      FelixEngine::GetEventSystem()->addHandler(this);
      if (mWindow)
        resize(mWindow->size());
      return Component::init();
    }
    
    virtual void handle(const Event &event)
    {
      if (event == EVENT_WINDOW)
        handleWindowEvent(event);
    }
    
    float scale() const {return mScale;}
    void setScale(float scale) {mScale = scale;}
    
    vec2 size() const {return mSize/mScale;}
    ivec2 pixelSize() const {return mSize;}
    void resize(const ivec2 size)
    {
      if (mProjection && mSize != size)
      {
        mSize = size;
        
        Volume vol;
        vol.right = (mSize.w/2.0f)/mScale;
        vol.left = -vol.right;
        vol.top = (mSize.h/2.0f)/mScale;
        vol.bottom = -vol.top;
        mProjection->setVolume(vol);
      }
    }
    
    Window* window() const {return mWindow;}
    void setWindow(Window *window)
    {
      mWindow = window;
      if (mWindow)
      {
        setScale(mWindow->scale());
        resize(mWindow->size());
      }
      else
        setScale(1.0f);
    }
    
  protected:
    void handleWindowEvent(const Event &event)
    {
      if (mWindow && event.windowData().event == EVENT_WINDOW_RESIZED)
        resize(event.windowData().data);
    }
    
    RenderSlots *mRenderSlots;
    Projection *mProjection;
    View *mView;
    
    ivec2 mSize;
    float mScale;
    Window *mWindow;
  };
}

#endif /* UIScreen_h */
