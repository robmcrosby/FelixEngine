//
//  Task.h
//  FelixEngine
//
//  Created by Robert Crosby on 10/27/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef Task_h
#define Task_h

#include "TaskingSystem.h"

namespace fx
{
  class Task
  {
  public:
    Task(): mFunction(0) {setDelegate(TaskDelegate::Create<Task, &Task::exec>(this));}
    virtual ~Task() {}
    
    virtual void execute(void*) {}
    
    bool dispatch(TaskGroup &group) {return dispatch(nullptr, &group);}
    bool dispatch(TaskGroup *group) {return dispatch(nullptr, group);}
    bool dispatch(void *ptr = nullptr, TaskGroup *group = nullptr)
    {
      TaskingSystem *taskingSystem = TaskingSystem::Instance();
      if (!taskingSystem)
        return false;
      if (mFunction)
        return taskingSystem->dispatch(mFunction, ptr, group);
      return taskingSystem->dispatch(mDelegate, ptr, group);
    }
    
    bool runAfterGroup(TaskGroup &group, void *ptr = nullptr) {return runAfterGroup(&group, ptr);}
    bool runAfterGroup(TaskGroup *group, void *ptr = nullptr)
    {
      TaskingSystem *taskingSystem = TaskingSystem::Instance();
      if (!taskingSystem)
        return false;
      if (mFunction)
        return taskingSystem->runAfterGroup(mFunction, group, ptr);
      return taskingSystem->runAfterGroup(mDelegate, group, ptr);
    }
    
    void setDelegate(TaskDelegate delegate) {mDelegate = delegate;}
    void setFunction(TaskFunction *function) {mFunction = function;}
    
  private:
    void exec(void *ptr) {execute(ptr);}
    
    TaskDelegate mDelegate;
    TaskFunction *mFunction;
  };
}

#endif /* Task_h */
