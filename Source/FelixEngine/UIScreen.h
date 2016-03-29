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
    UIScreen(Scene *scene): UIWidget(scene),
      mWindow(0),
      mRenderSlots(0),
      mUIRenderPass(0),
      mProjection(0),
      mView(0),
      mWindowSize(0, 0),
      mWindowScale(1.0f, 1.0f)
    {
      setEventFlags(EVENT_WINDOW);
    }
    virtual ~UIScreen() {}
    
    virtual void setToXml(const XMLTree::Node &node)
    {
      UIWidget::setToXml(node);
      
      if (node.hasAttribute("window"))
        setWindow(FelixEngine::GetGraphicSystem()->getWindow(node.attribute("window")));
    }
    
    virtual bool init()
    {
      FelixEngine::GetEventSystem()->addHandler(this);
      
      if (!mWindow)
        setWindow(FelixEngine::GetGraphicSystem()->getMainWindow());
      else
        resize(mWindowSize, mWindowScale);
      
      mRenderSlots = getChild<RenderSlots>();
      mProjection = getChild<Projection>();
      mView = getChild<View>();
      
      mUIRenderPass = mRenderSlots->addSlot();
      mUIRenderPass->setTaskType(GRAPHIC_TASK_PASS);
      mUIRenderPass->setPass(UI_PASS);
      mUIRenderPass->setLayer(1);
      
      return UIWidget::init();
    }
    
    virtual void handle(const Event &event)
    {
      if (event == EVENT_WINDOW)
        handleWindowEvent(event);
    }
    
    void resize(const ivec2 &size, const vec2 &scale)
    {
      mWindowSize = size;
      mWindowScale = scale;
      mSize = vec2(size)/scale;
      
      if (mProjection)
      {
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
        resize(mWindow->size(), mWindow->scale());
    }
    
  protected:
    void handleWindowEvent(const Event &event)
    {
      const Event::WindowData& windowData = event.windowData();
      if (mWindow && mWindow->windowId() == windowData.windowId)
      {
        if (windowData.event == EVENT_WINDOW_RESIZED)
          resize(event.windowData().data, mWindowScale);
      }
    }
    
    void updateWidgets()
    {
      for (UIWidget *widget : mWidgets)
        widget->updateLayout(size(), vec2(0.0f, 0.0f));
    }
    
    RenderSlots *mRenderSlots;
    RenderSlot *mUIRenderPass;
    
    Projection *mProjection;
    View *mView;
    
    ivec2 mWindowSize;
    vec2 mWindowScale;
    
    Window *mWindow;
  };
}

#endif /* UIScreen_h */
