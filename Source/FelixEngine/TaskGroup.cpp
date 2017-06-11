//
//  TaskGroup.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 4/19/16.
//  Copyright © 2016 Robert Crosby. All rights reserved.
//

#include "TaskGroup.h"
#include "Task.h"
//#include "TaskingSystem.h"


using namespace fx;

TaskGroup::TaskGroup(): mFunction(0), mPtr(0)
{
  mDelegate = TaskDelegate::Create<TaskGroup, &TaskGroup::exec>(this);
}

TaskGroup::~TaskGroup()
{
}

bool TaskGroup::dispatch(Task &task, void *ptr)
{
  return task.dispatch(ptr, this);
}

bool TaskGroup::dispatch(TaskDelegate delegate, void *ptr)
{
  TaskingSystem *taskingSystem = TaskingSystem::Instance();
  if (!taskingSystem)
    return false;
  return taskingSystem->dispatch(delegate, ptr, this);
}

bool TaskGroup::dispatch(TaskFunction *function, void *ptr)
{
  TaskingSystem *taskingSystem = TaskingSystem::Instance();
  if (!taskingSystem)
    return false;
  return taskingSystem->dispatch(function, ptr, this);
}

bool TaskGroup::waitOnTasks()
{
  TaskingSystem *taskingSystem = TaskingSystem::Instance();
  if (!taskingSystem)
    return false;
  return taskingSystem->waitOnGroup(this);
}

bool TaskGroup::runAfterTasks(Task &task, void *ptr)
{
  return task.runAfterGroup(this, ptr);
}

bool TaskGroup::runAfterTasks(TaskDelegate delegate, void *ptr)
{
  TaskingSystem *taskingSystem = TaskingSystem::Instance();
  if (!taskingSystem)
    return false;
  return taskingSystem->runAfterGroup(delegate, this, ptr);
}

bool TaskGroup::runAfterTasks(TaskFunction *function, void *ptr)
{
  TaskingSystem *taskingSystem = TaskingSystem::Instance();
  if (!taskingSystem)
    return false;
  return taskingSystem->runAfterGroup(function, this, ptr);
}
