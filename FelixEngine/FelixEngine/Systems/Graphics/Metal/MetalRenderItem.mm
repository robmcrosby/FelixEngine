//
//  MetalRenderItem.m
//  FelixEngine
//
//  Created by Robert Crosby on 7/7/18.
//  Copyright © 2018 Robert Crosby. All rights reserved.
//

#include "MetalRenderItem.h"
#include <Metal/Metal.h>

using namespace fx;
using namespace std;


MetalRenderItem::MetalRenderItem(id <MTLDevice> device): _device(device) {
  
}

MetalRenderItem::~MetalRenderItem() {
  
}
