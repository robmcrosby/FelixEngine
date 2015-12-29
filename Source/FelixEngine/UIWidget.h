//
//  UIWidget.h
//  FelixEngine
//
//  Created by Robert Crosby on 12/15/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef UIWidget_h
#define UIWidget_h

#include "Component.h"
#include "Transform.h"

namespace fx
{
  enum LAYOUT_FLAGS
  {
    LAYOUT_CENTER = 0x00,
    LAYOUT_LEFT   = 0x01,
    LAYOUT_RIGHT  = 0x02,
    LAYOUT_TOP    = 0x10,
    LAYOUT_BOTTOM = 0x20,
    LAYOUT_ORIGIN = 0x33,
  };
  
  class UIWidget;
  typedef std::list<UIWidget*> UIWidgets;
  
  /**
   *
   */
  class UIWidget: public Component
  {
  public:
    UIWidget(Scene *scene): Component(scene), mTransform(0), mLayoutFlags(0) {}
    virtual ~UIWidget() {}
    
    virtual void setToXml(const XMLTree::Node &node)
    {
      Component::setToXml(node);
      addChildren(node);
      mTransform = getChild<Transform>();
      
      if (node.hasAttribute("size"))
        resize(node.attribute("size"));
      if (node.hasAttribute("offset"))
        setOffset(node.attribute("offset"));
      if (node.hasAttribute("origin"))
        setOriginLayout(node.attribute("origin"));
    }
    
    virtual bool init()
    {
      updateTransform();
      return Component::init();
    }
    
    virtual void addChild(Component *child)
    {
      Component::addChild(child);
      UIWidget *widget = dynamic_cast<UIWidget*>(child);
      if (widget)
      {
        widget->updateLayout(size(), center());
        mWidgets.push_back(widget);
      }
    }
    
    virtual void removeChild(Component *child)
    {
      Component::removeChild(child);
      UIWidget *widget = dynamic_cast<UIWidget*>(child);
      if (widget)
        mWidgets.remove(widget);
    }
    
    virtual void clearChildren()
    {
      mWidgets.clear();
      Component::clearChildren();
    }
    
    vec2 size() const {return mSize;}
    void resize(const vec2 &size)
    {
      mSize = size;
      updateTransform();
    }
    
    vec2 center() const
    {
      vec2 ret = mParrentCenter + mOffset;
      
      if (mLayoutFlags & LAYOUT_LEFT)
        ret.x += size().x/2.0f - mParrentSize.x/2.0f;
      else if (mLayoutFlags & LAYOUT_RIGHT)
        ret.x += mParrentSize.x/2.0f - size().x/2.0f;
      
      if (mLayoutFlags & LAYOUT_BOTTOM)
        ret.y += size().y/2.0f - mParrentSize.y/2.0f;
      else if (mLayoutFlags & LAYOUT_TOP)
        ret.y += mParrentSize.y/2.0f - size().y/2.0f;
      
      return ret;
    }
    vec2 offset() const {return mOffset;}
    void setOffset(const vec2 &offset)
    {
      mOffset = offset;
      updateTransform();
    }
    
    void setOriginLayout(int layout) {mLayoutFlags = (mLayoutFlags & ~LAYOUT_ORIGIN) | (layout & LAYOUT_ORIGIN);}
    void setOriginLayout(const std::string &str) {setOriginLayout(GetOriginLayout(str));}
    
    void updateLayout(const vec2 &parrentSize, const vec2 &parrentCenter)
    {
      mParrentSize = parrentSize;
      mParrentCenter = parrentCenter;
      updateTransform();
      
      vec2 thisSize = size();
      vec2 thisCenter = center();
      for (UIWidgets::iterator itr = mWidgets.begin(); itr != mWidgets.end(); ++itr)
        (*itr)->updateLayout(thisSize, thisCenter);
    }
    
    static int GetOriginLayout(const std::string &str)
    {
      int layout = 0;
      
      if (str.find('L') != std::string::npos)
        layout |= LAYOUT_LEFT;
      else if (str.find('R') != std::string::npos)
        layout |= LAYOUT_RIGHT;
      
      if (str.find('T') != std::string::npos)
        layout |= LAYOUT_TOP;
      else if (str.find('B') != std::string::npos)
        layout |= LAYOUT_BOTTOM;
      
      return layout;
    }
    
  protected:
    void updateTransform()
    {
      if (mTransform)
      {
        mTransform->clear();
        mTransform->addScale(size());
        mTransform->addTranslate(center());
      }
    }
    
    Transform *mTransform;
    
    int mLayoutFlags;
    vec2 mSize;
    vec2 mOffset;
    
    
    vec2 mParrentSize;
    vec2 mParrentCenter;
    
    UIWidgets mWidgets;
  };
}

#endif /* UIWidget_h */
