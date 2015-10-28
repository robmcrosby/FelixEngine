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
  
  struct TaskData
  {
    virtual ~TaskData() {}
    virtual TaskData* copy() const {return new TaskData(*this);}
    virtual void release() {delete this;}
  };
  typedef void(TaskFunction)(TaskData*);
  typedef Delegate<void, TaskData*> TaskDelegate;
  
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
    
    bool dispatch(TaskFunction *function) {return dispatch(function, nullptr, nullptr);}
    bool dispatch(TaskFunction *function, TaskData &data) {return dispatch(function, &data, nullptr);}
    bool dispatch(TaskFunction *function, TaskData *data) {return dispatch(function, data, nullptr);}
    bool dispatch(TaskFunction *function, TaskGroup &group) {return dispatch(function, nullptr, &group);}
    bool dispatch(TaskFunction *function, TaskGroup *group) {return dispatch(function, nullptr, group);}
    bool dispatch(TaskFunction *function, TaskData *data, TaskGroup &group) {return dispatch(function, data, &group);}
    bool dispatch(TaskFunction *function, TaskData &data, TaskGroup *group) {return dispatch(function, &data, group);}
    bool dispatch(TaskFunction *function, TaskData *data, TaskGroup *group)
    {
      if (!function)
        return false;
      TaskInfo info;
      info.function = function;
      info.group = group;
      info.data = data ? data->copy() : nullptr;
      return mDispatchTaskDelegate(info);
    }
    
    bool dispatch(TaskDelegate delegate) {return dispatch(delegate, nullptr, nullptr);}
    bool dispatch(TaskDelegate delegate, TaskData &data) {return dispatch(delegate, &data, nullptr);}
    bool dispatch(TaskDelegate delegate, TaskData *data) {return dispatch(delegate, data, nullptr);}
    bool dispatch(TaskDelegate delegate, TaskGroup &group) {return dispatch(delegate, nullptr, &group);}
    bool dispatch(TaskDelegate delegate, TaskGroup *group) {return dispatch(delegate, nullptr, group);}
    bool dispatch(TaskDelegate delegate, TaskData *data, TaskGroup &group) {return dispatch(delegate, data, &group);}
    bool dispatch(TaskDelegate delegate, TaskData &data, TaskGroup *group) {return dispatch(delegate, &data, group);}
    bool dispatch(TaskDelegate delegate, TaskData *data, TaskGroup *group)
    {
      TaskInfo info;
      info.delegate = delegate;
      info.group = group;
      info.data = data ? data->copy() : nullptr;
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
      TaskInfo(): function(0), data(0), group(0) {}
      TaskDelegate delegate;
      TaskFunction *function;
      TaskGroup *group;
      TaskData  *data;
    };
    
    Delegate<bool, TaskInfo&> mDispatchTaskDelegate;
    Delegate<bool, TaskInfo&> mWaitOnGroupDelegate;
    
  private:
    bool dispatchTask(TaskInfo &info)
    {
      info.function ? (*info.function)(info.data) : info.delegate(info.data);
      return true;
    }
    bool waitOnGroup(TaskInfo &info) {return true;}
    
  private:
    static TaskingSystem *sInstance;
  };
}

#endif /* TaskingSystem_h */
