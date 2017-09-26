//
//  RenderItem.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 9/26/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#include "RenderItem.h"
#include "RenderPass.h"


using namespace fx;
using namespace std;


void RenderItem::addToRenderPass(const std::string &name) {
  _renderPasses.insert(RenderPass::getRenderPass(name));
}

void RenderItem::addToRenderPass(RenderPassPtr &renderPass) {
  _renderPasses.insert(renderPass);
}

void RenderItem::removeFromRenderPass(const std::string &name) {
  _renderPasses.erase(RenderPass::getRenderPass(name));
}

void RenderItem::removeFromRenderPass(RenderPassPtr &renderPass) {
  _renderPasses.erase(renderPass);
}

void RenderItem::removeFromAllRenderPasses() {
  _renderPasses.clear();
}
