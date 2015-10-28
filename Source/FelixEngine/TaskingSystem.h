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
  struct TaskGroup
  {
    TaskGroup(): ptr(0) {}
    virtual ~TaskGroup() {}
    void *ptr;
  };
  struct TaskData
  {
    virtual ~TaskData() {}
    virtual TaskData* copy() {return new TaskData(*this);}
    virtual void release() {delete this;}
  };
  typedef void(TaskFunction)(TaskData*);
  typedef Delegate<void, TaskData*> TaskDelegate;
  
  class TaskingSystem: public System
  {
  public:
    TaskingSystem(): System(SYSTEM_TASKING)
    {
      mDispatchTaskDelegate = Delegate<bool, TaskInfo&>::Create<TaskingSystem, &TaskingSystem::dispatchTask>(this);
      mWaitOnGroupDelegate = Delegate<bool, TaskInfo&>::Create<TaskingSystem, &TaskingSystem::waitOnGroup>(this);
    }
    virtual ~TaskingSystem() {}
    
    virtual bool setToXml(const XMLTree::Node *node) {return true;}
    virtual bool init() {return true;}
    
    bool dispatchTask(TaskFunction *function) {return dispatchTask(function, nullptr, nullptr);}
    bool dispatchTask(TaskFunction *function, TaskData &data) {return dispatchTask(function, &data, nullptr);}
    bool dispatchTask(TaskFunction *function, TaskData *data) {return dispatchTask(function, data, nullptr);}
    bool dispatchTask(TaskFunction *function, TaskGroup &group) {return dispatchTask(function, nullptr, &group);}
    bool dispatchTask(TaskFunction *function, TaskGroup *group) {return dispatchTask(function, nullptr, group);}
    bool dispatchTask(TaskFunction *function, TaskData *data, TaskGroup &group) {return dispatchTask(function, data, &group);}
    bool dispatchTask(TaskFunction *function, TaskData &data, TaskGroup *group) {return dispatchTask(function, &data, group);}
    bool dispatchTask(TaskFunction *function, TaskData *data, TaskGroup *group)
    {
      if (!function)
        return false;
      TaskInfo info;
      info.function = function;
      info.group = group;
      info.data = data ? data->copy() : nullptr;
      return mDispatchTaskDelegate(info);
    }
    
    bool dispatchTask(TaskDelegate &delegate) {return dispatchTask(delegate, nullptr, nullptr);}
    bool dispatchTask(TaskDelegate &delegate, TaskData &data) {return dispatchTask(delegate, &data, nullptr);}
    bool dispatchTask(TaskDelegate &delegate, TaskData *data) {return dispatchTask(delegate, data, nullptr);}
    bool dispatchTask(TaskDelegate &delegate, TaskGroup &group) {return dispatchTask(delegate, nullptr, &group);}
    bool dispatchTask(TaskDelegate &delegate, TaskGroup *group) {return dispatchTask(delegate, nullptr, group);}
    bool dispatchTask(TaskDelegate &delegate, TaskData *data, TaskGroup &group) {return dispatchTask(delegate, data, &group);}
    bool dispatchTask(TaskDelegate &delegate, TaskData &data, TaskGroup *group) {return dispatchTask(delegate, &data, group);}
    bool dispatchTask(TaskDelegate &delegate, TaskData *data, TaskGroup *group)
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
  };
}

#endif /* TaskingSystem_h */
