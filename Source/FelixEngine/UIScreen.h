//
//  UIScreen.h
//  FelixEngine
//
//  Created by Robert Crosby on 12/15/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef UIScreen_h
#define UIScreen_h

#include "UIWidget.h"
#include "RenderSlots.h"
#include "Projection.h"
#include "View.h"


namespace fx
{
  /**
   *
   */
  class UIScreen: public UIWidget
  {
  public:
    UIScreen(Scene *scene): UIWidget(scene), mWindow(0), mRenderSlots(0), mProjection(0), mView(0)
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
    
    void resize(const vec2 size)
    {
      if (mProjection && mSize != size)
      {
        mSize = size;
        
        Volume vol;
        vol.right = mSize.w/2.0f;
        vol.left = -vol.right;
        vol.top = mSize.h/2.0f;
        vol.bottom = -vol.top;
        mProjection->setVolume(vol);
        
        updateWidgets();
      }
    }
    
    Window* window() const {return mWindow;}
    void setWindow(Window *window)
    {
      mWindow = window;
      if (mWindow)
        resize(mWindow->size());
    }
    
  protected:
    void handleWindowEvent(const Event &event)
    {
      if (mWindow && event.windowData().event == EVENT_WINDOW_RESIZED)
        resize(event.windowData().data);
    }
    void updateWidgets()
    {
      vec2 screenSize = size();
      vec2 screenCenter = vec2(0.0f, 0.0f);
      for (UIWidgets::iterator itr = mWidgets.begin(); itr != mWidgets.end(); ++itr)
        (*itr)->updateLayout(screenSize, screenCenter);
    }
    
    RenderSlots *mRenderSlots;
    Projection *mProjection;
    View *mView;
    
    Window *mWindow;
  };
}

#endif /* UIScreen_h */
