//
//  Application.hpp
//  FelixEngine
//
//  Created by Robert Crosby on 6/13/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#ifndef Application_h
#define Application_h

#include <FelixEngine/EventSubject.h>
#include <FelixEngine/FileSystem.h>
#include <FelixEngine/Graphics.h>
#include <FelixEngine/MotionSystem.h>
#include <FelixEngine/TrackerSystem.h>

namespace fx {
  class Application: public EventSubject {
  protected:
    FileSystem *_fileSystem;
    Graphics *_graphics;
    MotionSystem *_motionSystem;
    TrackerSystem *_tracker;
    
  public:
    Application();
    virtual ~Application();
    
    virtual void handle(const Event &event);
    
    virtual void initalize();
    virtual void update(float td);
    
    virtual void willEnterBackground();
    virtual void didBecomeActive();
    virtual void willTerminate();
    
    void processFrame(float td);
    
    FileSystem& fileSystem() const {return *_fileSystem;}
    void setFileSystem(FileSystem *fileSystem) {_fileSystem = fileSystem;}
    
    Graphics& graphics() const {return *_graphics;}
    void setGraphics(Graphics *graphics) {_graphics = graphics;}
    
    MotionSystem& motionSystem() const {return *_motionSystem;}
    void setMotionSystem(MotionSystem *motionSystem) {_motionSystem = motionSystem;}
    
    TrackerSystem& trackerSystem() const {return *_tracker;}
    void setTrackerSystem(TrackerSystem *tracker) {_tracker = tracker;}
  };
}

#endif /* Application_hpp */
