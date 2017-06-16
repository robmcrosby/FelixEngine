//
//  MetalGraphics.m
//  FelixEngine
//
//  Created by Robert Crosby on 6/14/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#include <iostream>

#include "MetalGraphics.h"
#include <Metal/Metal.h>
#include <UIKit/UIKit.h>
#include <QuartzCore/CAMetalLayer.h>

namespace fx {
  struct MetalGraphicsData {
    id <MTLDevice>       device;
    id <MTLCommandQueue> queue;
    
    CAMetalLayer *layer;
    
    ~MetalGraphicsData() {
      device = nil;
      queue  = nil;
      layer  = nil;
    }
  };
}


using namespace std;
using namespace fx;


MetalGraphics::MetalGraphics(): _data(new MetalGraphicsData()) {
  Graphics::instance = this;
}

MetalGraphics::~MetalGraphics() {
  delete _data;
}

bool MetalGraphics::initalize(UIView *view) {
  // Get the Metal Device
  _data->device = MTLCreateSystemDefaultDevice();
  if (_data->device == nil) {
    cerr << "Error Creating Metal Device" << endl;
    return false;
  }
  
  // Create a Command Queue
  _data->queue = [_data->device newCommandQueue];
  if (_data->queue == nil) {
    cerr << "Error Creating Metal Queue" << endl;
    _data->device = nil;
    return false;
  }
  
  // Setup the Metal Layer
  _data->layer = [[CAMetalLayer alloc] init];
  if (_data->layer == nil) {
    cerr << "Error Creating Metal Layer" << endl;
    _data->device = nil;
    _data->queue  = nil;
    return false;
  }
  _data->layer.device = _data->device;
  _data->layer.pixelFormat = MTLPixelFormatBGRA8Unorm;
  _data->layer.framebufferOnly = YES;
  _data->layer.frame = view.layer.frame;
  [view.layer addSublayer: _data->layer];
  
  return true;
}
