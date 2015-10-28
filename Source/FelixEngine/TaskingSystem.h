//
//  TaskingSystem.h
//  FelixEngine
//
//  Created by Robert Crosby on 10/26/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef TaskingSystem_h
#define TaskingSystem_h

#include "System.h"
#include "Delegate.h"

namespace fx
{
  class TaskGroup;
  
  typedef void(TaskFunction)(void*);
  typedef Delegate<void, void*> TaskDelegate;
  
  class TaskingSystem: public System
  {
  public:
    static TaskingSystem* Instance() {return sInstance;}
    
    TaskingSystem(): System(SYSTEM_TASKING)
    {
      mDispatchTaskDelegate = Delegate<bool, TaskInfo&>::Create<TaskingSystem, &TaskingSystem::dispatchTask>(this);
      mWaitOnGroupDelegate = Delegate<bool, TaskInfo&>::Create<TaskingSystem, &TaskingSystem::waitOnGroup>(this);
      sInstance = this;
    }
    virtual ~TaskingSystem() {}
    
    virtual bool setToXml(const XMLTree::Node *node) {return true;}
    virtual bool init() {return true;}
    
    bool dispatch(TaskFunction *function, TaskGroup &group) {return dispatch(function, nullptr, &group);}
    bool dispatch(TaskFunction *function, TaskGroup *group) {return dispatch(function, nullptr, group);}
    bool dispatch(TaskFunction *function, void *ptr, TaskGroup &group) {return dispatch(function, ptr, &group);}
    bool dispatch(TaskFunction *function, void *ptr = nullptr, TaskGroup *group = nullptr)
    {
      if (!function)
        return false;
      TaskInfo info;
      info.function = function;
      info.group = group;
      info.ptr = ptr;
      return mDispatchTaskDelegate(info);
    }
    
    bool dispatch(TaskDelegate delegate, TaskGroup &group) {return dispatch(delegate, nullptr, &group);}
    bool dispatch(TaskDelegate delegate, TaskGroup *group) {return dispatch(delegate, nullptr, group);}
    bool dispatch(TaskDelegate delegate, void *ptr, TaskGroup &group) {return dispatch(delegate, ptr, &group);}
    bool dispatch(TaskDelegate delegate, void *ptr = nullptr, TaskGroup *group = nullptr)
    {
      TaskInfo info;
      info.delegate = delegate;
      info.group = group;
      info.ptr = ptr;
      return mDispatchTaskDelegate(info);
    }
    
    
    bool waitOnGroup(TaskGroup *group) {return group && waitOnGroup(*group);}
    bool waitOnGroup(TaskGroup &group)
    {
      TaskInfo info;
      info.group = &group;
      return mWaitOnGroupDelegate(info);
    }
    
  protected:
    struct TaskInfo
    {
      TaskInfo(): function(0), group(0), ptr(0) {}
      TaskDelegate delegate;
      TaskFunction *function;
      TaskGroup *group;
      void *ptr;
    };
    
    Delegate<bool, TaskInfo&> mDispatchTaskDelegate;
    Delegate<bool, TaskInfo&> mWaitOnGroupDelegate;
    
  private:
    bool dispatchTask(TaskInfo &info)
    {
      info.function ? (*info.function)(info.ptr) : info.delegate(info.ptr);
      return true;
    }
    bool waitOnGroup(TaskInfo &info) {return true;}
    
  private:
    static TaskingSystem *sInstance;
  };
}

#endif /* TaskingSystem_h */
