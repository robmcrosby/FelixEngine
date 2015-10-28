//
//  TaskGroup.h
//  FelixEngine
//
//  Created by Robert Crosby on 10/27/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef TaskGroup_h
#define TaskGroup_h

#include "TaskingSystem.h"
#include "Task.h"


namespace fx
{
  class GCDTaskingSystem;
  
  class TaskGroup
  {
  public:
    TaskGroup(): mPtr(0) {}
    
    bool dispatch(Task &task, void *ptr = nullptr) {return task.dispatch(ptr, this);}
    bool dispatch(TaskDelegate delegate, void *ptr = nullptr)
    {
      TaskingSystem *taskingSystem = TaskingSystem::Instance();
      if (!taskingSystem)
        return false;
      return taskingSystem->dispatch(delegate, ptr, this);
    }
    bool dispatch(TaskFunction *function, void *ptr = nullptr)
    {
      TaskingSystem *taskingSystem = TaskingSystem::Instance();
      if (!taskingSystem)
        return false;
      return taskingSystem->dispatch(function, ptr, this);
    }
    
    bool waitOnTasks()
    {
      TaskingSystem *taskingSystem = TaskingSystem::Instance();
      if (!taskingSystem)
        return false;
      return taskingSystem->waitOnGroup(this);
    }
    
    bool runAfterTasks(Task &task, void *ptr = nullptr) {return task.runAfterGroup(this, ptr);}
    bool runAfterTasks(TaskDelegate delegate, void *ptr = nullptr)
    {
      TaskingSystem *taskingSystem = TaskingSystem::Instance();
      if (!taskingSystem)
        return false;
      return taskingSystem->runAfterGroup(delegate, this, ptr);
    }
    bool runAfterTasks(TaskFunction *function, void *ptr = nullptr)
    {
      TaskingSystem *taskingSystem = TaskingSystem::Instance();
      if (!taskingSystem)
        return false;
      return taskingSystem->runAfterGroup(function, this, ptr);
    }
    
  private:
    friend GCDTaskingSystem;
    void *mPtr;
  };
}


#endif /* TaskGroup_h */
