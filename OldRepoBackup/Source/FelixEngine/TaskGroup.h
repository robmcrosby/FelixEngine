//
//  TaskGroup.h
//  FelixEngine
//
//  Created by Robert Crosby on 10/27/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef TaskGroup_h
#define TaskGroup_h

#include "Tasking.h"


namespace fx
{
  class GCDTaskingSystem;
  class Task;
  
  class TaskGroup
  {
  public:
    TaskGroup();
    virtual ~TaskGroup();
    
    virtual void execute(void*) {}
    
    bool dispatch(Task *task, void *ptr = nullptr) {return task ? dispatch(*task, ptr) : false;}
    bool dispatch(Task &task, void *ptr = nullptr);
    bool dispatch(TaskDelegate delegate, void *ptr = nullptr);
    bool dispatch(TaskFunction *function, void *ptr = nullptr);
    
    bool waitOnTasks();
    
    bool runAfterTasks(void *ptr = nullptr)
    {
      if (mFunction)
        return runAfterTasks(mFunction, ptr);
      return runAfterTasks(mDelegate, ptr);
    }
    bool runAfterTasks(Task *task, void *ptr = nullptr) {return task ? runAfterTasks(*task, ptr) : false;}
    bool runAfterTasks(Task &task, void *ptr = nullptr);
    bool runAfterTasks(TaskDelegate delegate, void *ptr = nullptr);
    bool runAfterTasks(TaskFunction *function, void *ptr = nullptr);
    
    void setDelegate(TaskDelegate delegate) {mDelegate = delegate;}
    void setFunction(TaskFunction *function) {mFunction = function;}
    
  private:
    void exec(void *ptr) {execute(ptr);}
    
    TaskDelegate mDelegate;
    TaskFunction *mFunction;
    
  private:
    friend GCDTaskingSystem;
    void *mPtr;
  };
}


#endif /* TaskGroup_h */
