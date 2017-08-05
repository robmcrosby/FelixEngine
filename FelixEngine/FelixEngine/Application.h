//
//  Application.hpp
//  FelixEngine
//
//  Created by Robert Crosby on 6/13/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#ifndef Application_h
#define Application_h

#include "FileSystem.h"
#include "Graphics.h"
#include "TrackerSystem.h"

namespace fx {
  class Application {
  protected:
    FileSystem *_fileSystem;
    Graphics *_graphics;
    TrackerSystem *_tracker;
    
  public:
    Application();
    virtual ~Application();
    
    virtual void initalize();
    virtual void update();
    virtual void render();
    
    virtual void willEnterBackground();
    virtual void willEnterForeground();
    virtual void willTerminate();
    
    void processFrame();
    
    FileSystem& fileSystem() const {return *_fileSystem;}
    void setFileSystem(FileSystem *fileSystem) {_fileSystem = fileSystem;}
    
    Graphics& graphics() const {return *_graphics;}
    void setGraphics(Graphics *graphics) {_graphics = graphics;}
    
    TrackerSystem& trackerSystem() const {return *_tracker;}
    void setTrackerSystem(TrackerSystem *tracker) {_tracker = tracker;}
  };
}

#endif /* Application_hpp */
