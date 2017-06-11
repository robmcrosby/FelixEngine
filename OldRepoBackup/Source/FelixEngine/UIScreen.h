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
      mWindowSize(0, 0),
      mWindowScale(1.0f, 1.0f)
    {
      setEventFlags(EVENT_WINDOW);
    }
    virtual ~UIScreen() {}
    
    virtual void setToXml(const XMLTree::Node &node)
    {
      Component::setToXml(node);
      addChildren(node);
      
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
      
      mUIRenderPass = mRenderSlots->addSlot();
      mUIRenderPass->setTaskType(GRAPHIC_TASK_PASS);
      mUIRenderPass->setPass(UI_PASS);
      mUIRenderPass->setLayer(10);
      
      return UIWidget::init();
    }
    
    virtual void handle(const Event &event)
    {
      if (event == EVENT_WINDOW)
        handleWindowEvent(event);
    }
    
    virtual void update()
    {
      Component::update();
      if (mRenderSlots)
      {
        Volume v;
        v.right = mSize.x/2.0f;
        v.left = -v.right;
        v.top = mSize.y/2.0f;
        v.bottom = -v.top;
        
        mRenderSlots->setStruct("camera", "projection", mat4::Ortho(v.left, v.right, v.bottom, v.top, v.near, v.far));
        mRenderSlots->setStruct("camera", "view", mat4());
        mRenderSlots->setStruct("camera", "position", vec4());
      }
    }
    
    void resize(ivec2 size, vec2 scale)
    {
      mWindowSize = size;
      mWindowScale = scale;
      mSize = vec2(size)/scale;
        
      updateWidgets();
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
          resize(event.windowData().data*mWindowScale, mWindowScale);
      }
    }
    
    void updateWidgets()
    {
      for (UIWidget *widget : mWidgets)
        widget->updateLayout(size(), vec2(0.0f, 0.0f));
    }
    
    RenderSlots *mRenderSlots;
    RenderSlot *mUIRenderPass;
    
    ivec2 mWindowSize;
    vec2 mWindowScale;
    
    Window *mWindow;
  };
}

#endif /* UIScreen_h */
