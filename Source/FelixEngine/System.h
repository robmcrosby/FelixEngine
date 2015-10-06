//
//  System.h
//  FelixEngine
//
//  Created by Robert Crosby on 10/2/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef System_h
#define System_h

#include <iostream>
#include <istream>
#include <map>

#include "XMLTree.h"

namespace fx
{
  enum SYSTEM_TYPE
  {
    SYSTEM_GRAPHICS = 0,
    SYSTEM_AUDIO,
    SYSTEM_PHYSICS,
    SYSTEM_OTHER,
    SYSTEM_COUNT,
  };
  
  /**
   * Defines the base class for a System used in the Felix Engine.
   */
  class System
  {
  public:
    System(SYSTEM_TYPE type = SYSTEM_OTHER);
    virtual ~System();
    
    virtual void update() = 0;
    virtual bool setToXml(const XMLTree::Node &node) = 0;
    
    SYSTEM_TYPE type() const {return mType;}
    
  protected:
    SYSTEM_TYPE mType;
    
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
