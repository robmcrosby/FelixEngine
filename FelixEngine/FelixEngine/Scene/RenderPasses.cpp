//
//  RenderPasses.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 7/8/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#include "RenderPasses.h"


using namespace fx;
using namespace std;


RenderPasses::RenderPasses() {
  
}

RenderPasses::~RenderPasses() {
  
}

void RenderPasses::render() {
  
}

RenderPass& RenderPasses::operator[](const std::string &name) {
  // Attempt to find the RenderPass
  for (int i = 0; i < _renderPasses.size(); ++i) {
    if (name == _renderPasses.at(i).name)
      return _renderPasses.at(i);
  }
  
  // Create a new RenderPass
  _renderPasses.push_back(name);
  return _renderPasses.back();
}
