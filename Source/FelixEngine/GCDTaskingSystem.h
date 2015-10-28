//
//  GCDTaskingSystem.h
//  FelixEngine
//
//  Created by Robert Crosby on 10/26/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef GCDTaskingSystem_h
#define GCDTaskingSystem_h

#include "TaskingSystem.h"
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
    }
    virtual ~GCDTaskingSystem() {}
    
    bool dispatchTask(TaskInfo &info)
    {
      dispatch_queue_t queue = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0);
      __block TaskInfo i = info;
      if (info.group)
      {
        if (!info.group->ptr)
          info.group->ptr = (void*)dispatch_group_create();
        dispatch_group_t group = (dispatch_group_t)info.group->ptr;
        dispatch_group_async(group, queue, ^{
          if (i.function)
            (*i.function)(i.data);
          else
            i.delegate(i.data);
        });
      }
      else
      {
        dispatch_async(queue, ^{
          if (i.function)
            (*i.function)(i.data);
          else
            i.delegate(i.data);
        });
        return true;
      }
      return true;
    }
    
    bool waitOnGroup(TaskInfo &info)
    {
      if (info.group->ptr)
      {
        dispatch_group_t group = (dispatch_group_t)info.group->ptr;
        return !dispatch_group_wait(group, DISPATCH_TIME_FOREVER);
      }
      return true;
    }
  };
}

#endif /* GCDTaskingSystem_h */
