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
    Task(): mFunction(0), mData(0) {setDelegate(TaskDelegate::Create<Task, &Task::exec>(this));}
    virtual ~Task() {}
    
    virtual void execute(TaskData*) {}
    
    bool dispatch(TaskGroup &group) {return dispatch(&group);}
    bool dispatch(TaskGroup *group = nullptr)
    {
      TaskingSystem *taskingSystem = TaskingSystem::Instance();
      if (!taskingSystem)
        return false;
      if (mFunction)
        return taskingSystem->dispatch(mFunction, mData, group);
      return taskingSystem->dispatch(mDelegate, mData, group);
    }
    
    void setDelegate(TaskDelegate delegate) {mDelegate = delegate;}
    void setFunction(TaskFunction *function) {mFunction = function;}
    
    void setData(const TaskData &data) {setData(&data);}
    void setData(const TaskData *data)
    {
      if (mData)
        mData->release();
      mData = data ? data->copy() : nullptr;
    }
    void clearData() {setData(nullptr);}
    
    TaskData* taskData() const {return mData;}
    
  private:
    void exec(TaskData *data) {execute(data);}
    
    TaskDelegate mDelegate;
    TaskFunction *mFunction;
    TaskData *mData;
  };
}

#endif /* Task_h */
