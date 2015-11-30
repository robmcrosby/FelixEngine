//
//  main.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 10/1/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//


#include <FelixEngine/FelixEngine.h>

//#include <FelixEngine/List.h>
//#include <FelixEngine/TaskGroup.h>

#if __IPHONEOS__
//#define SETTINGS_FILE "SettingsGLES2.xml"
#define SETTINGS_FILE "SettingsGLES3.xml"
//#define SETTINGS_FILE "SettingsMetal.xml"
#else
#define SETTINGS_FILE "SettingsGL41.xml"
//#define SETTINGS_FILE "SettingsMetal.xml"
#endif

//#define SCENE_FILE "TriangleScene.xml"
//#define SCENE_FILE "TextureScene.xml"
//#define SCENE_FILE "FrameScene.xml"
#define SCENE_FILE "DepthScene.xml"

using namespace std;

//// Temporary Testing of List
//struct TestPush: fx::Task
//{
//  TestPush(fx::List<int> *l, int v = 0): list(l), value(v) {}
//  virtual void execute(void*)
//  {
//    list->pushBack(value);
//  }
//  fx::List<int> *list;
//  int value;
//};
//
//struct TestPop: fx::Task
//{
//  TestPop(fx::List<int> *l): list(l) {}
//  virtual void execute(void*)
//  {
//    list->popBack();
//  }
//  fx::List<int> *list;
//};


int main(int argc, char* args[])
{
  fx::FelixEngine *engine = fx::FelixEngine::Instance();
  
  if (!engine->init(SETTINGS_FILE))
  {
    cerr << "Error Initalizing the Engine" << endl;
    return 1;
  }
  
//    //Temporary Testing of List
//  fx::List<int> testList;
//  TestPush push1(&testList, 1);
//  TestPush push2(&testList, 2);
//  TestPush push3(&testList, 3);
//  TestPush push4(&testList, 4);
//  TestPop pop(&testList);
//  
//  fx::TaskGroup group;
//  group.dispatch(push1);
//  group.dispatch(push2);
//  group.dispatch(push3);
//  group.dispatch(push4);
//  group.dispatch(pop);
//  group.dispatch(pop);
//  group.waitOnTasks();
//  
//  for (fx::List<int>::Iterator itr = testList.begin(); itr != testList.end(); ++itr)
//    cout << *itr << ", ";
//  cout << endl;
  
  if (!engine->loadScene(SCENE_FILE))
  {
    cerr << "Error Loading the Scene" << endl;
    return 1;
  }
  
  return engine->runLoop();
}