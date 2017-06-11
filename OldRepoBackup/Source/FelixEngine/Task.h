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
    Task(): mDelegate(TaskDelegate::Create<Task, &Task::exec>(this)) {}
    Task(const Task &other): mDelegate(TaskDelegate::Create<Task, &Task::exec>(this)) {}
    virtual ~Task() {}
    
    Task& operator=(const Task &other) {return *this;}
    
    virtual void execute(void *ptr) {}
    
    bool dispatch(TaskGroup &group) {return dispatch(nullptr, &group);}
    bool dispatch(TaskGroup *group) {return dispatch(nullptr, group);}
    bool dispatch(void *ptr = nullptr, TaskGroup *group = nullptr)
    {
      TaskingSystem *taskingSystem = TaskingSystem::Instance();
      if (!taskingSystem)
        return false;
      return taskingSystem->dispatch(mDelegate, ptr, group);
    }
    
    bool runAfterGroup(TaskGroup &group, void *ptr = nullptr) {return runAfterGroup(&group, ptr);}
    bool runAfterGroup(TaskGroup *group, void *ptr = nullptr)
    {
      TaskingSystem *taskingSystem = TaskingSystem::Instance();
      if (!taskingSystem)
        return false;
      return taskingSystem->runAfterGroup(mDelegate, group, ptr);
    }
    
  protected:
    TaskDelegate mDelegate;
    
  private:
    void exec(void *ptr) {execute(ptr);}
  };
}

#endif /* Task_h */
