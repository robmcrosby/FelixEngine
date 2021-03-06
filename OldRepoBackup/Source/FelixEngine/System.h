//
//  System.h
//  FelixEngine
//
//  Created by Robert Crosby on 10/2/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef System_h
#define System_h

#include "EventHandler.h"
#include "XMLTree.h"

#include <iostream>
#include <istream>
#include <map>


namespace fx
{
  enum SYSTEM_TYPE
  {
    SYSTEM_FILE = 0,
    SYSTEM_GRAPHICS,
    SYSTEM_TASKING,
    SYSTEM_EVENTS,
    SYSTEM_MOTION,
    SYSTEM_AUDIO,
    SYSTEM_PHYSICS,
    SYSTEM_OTHER,
    SYSTEM_COUNT,
  };
  
  /**
   * Defines the base class for a System used in the Felix Engine.
   */
  class System: public EventHandler
  {
  public:
    System(SYSTEM_TYPE type = SYSTEM_OTHER);
    virtual ~System();
    
    virtual bool setToXml(const XMLTree::Node *node) = 0;
    virtual bool init() = 0;
    
    SYSTEM_TYPE type() const {return mType;}
    int getSDLInitFlags() const {return mSDLInitFlags;}
    
  protected:
    SYSTEM_TYPE mType;
    int mSDLInitFlags;
    
  public:
    static System* Create(const std::string &name);
    
    /**
     * Internal class used for reflection with xml
     */
    struct SystemId
    {
      virtual ~SystemId() {}
      virtual System* create() = 0;
    };
    static std::map<std::string, SystemId*>& GetSystemIdMap();
  };
  
  /**
   * Macro used to define SystemId classes.
   */
  #define DEFINE_SYSTEM_ID(T) \
  struct T##ID: public System::SystemId {\
    T##ID() {System::GetSystemIdMap()[#T] = this;}\
    virtual ~T##ID() {}\
    virtual System* create() {return new T();}\
    static T##ID ID;\
  };\
  T##ID T##ID::ID;
  
}


#endif /* System_h */
