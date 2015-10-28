//
//  GCDTaskingSystem.h
//  FelixEngine
//
//  Created by Robert Crosby on 10/26/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef GCDTaskingSystem_h
#define GCDTaskingSystem_h

#include "TaskGroup.h"
#include <dispatch/dispatch.h>


namespace fx
{
  class GCDTaskingSystem: public TaskingSystem
  {
  public:
    GCDTaskingSystem()
    {
      mDispatchTaskDelegate = Delegate<bool, TaskInfo&>::Create<GCDTaskingSystem, &GCDTaskingSystem::dispatchTask>(this);
      mWaitOnGroupDelegate = Delegate<bool, TaskInfo&>::Create<GCDTaskingSystem, &GCDTaskingSystem::waitOnGroup>(this);
      mRunAfterGroupDelegate = Delegate<bool, TaskInfo&>::Create<GCDTaskingSystem, &GCDTaskingSystem::runAfterGroup>(this);
    }
    virtual ~GCDTaskingSystem() {}
    
    bool dispatchTask(TaskInfo &info)
    {
      dispatch_queue_t queue = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0);
      __block TaskInfo i = info;
      if (info.group)
      {
        if (!info.group->mPtr)
          info.group->mPtr = (void*)dispatch_group_create();
        dispatch_group_t group = (dispatch_group_t)info.group->mPtr;
        dispatch_group_async(group, queue, ^{
          if (i.function)
            (*i.function)(i.ptr);
          else
            i.delegate(i.ptr);
        });
      }
      else
      {
        dispatch_async(queue, ^{
          if (i.function)
            (*i.function)(i.ptr);
          else
            i.delegate(i.ptr);
        });
      }
      return true;
    }
    
    bool waitOnGroup(TaskInfo &info)
    {
      bool success = true;
      if (info.group->mPtr)
      {
        dispatch_group_t group = (dispatch_group_t)info.group->mPtr;
        success = !dispatch_group_wait(group, DISPATCH_TIME_FOREVER);
        info.group->mPtr = nullptr;
      }
      return success;
    }
    
    bool runAfterGroup(TaskInfo &info)
    {
      dispatch_queue_t queue = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0);
      __block TaskInfo i = info;
      if (info.group->mPtr)
      {
        dispatch_group_t group = (dispatch_group_t)info.group->mPtr;
        dispatch_group_notify(group, queue, ^{
          if (i.function)
            (*i.function)(i.ptr);
          else
            i.delegate(i.ptr);
        });
      }
      else
      {
        dispatch_async(queue, ^{
          if (i.function)
            (*i.function)(i.ptr);
          else
            i.delegate(i.ptr);
        });
      }
      return true;
    }
  };
}

#endif /* GCDTaskingSystem_h */
