//
//  MetalView.m
//  FelixEngine
//
//  Created by Robert Crosby on 5/3/18.
//  Copyright © 2018 Robert Crosby. All rights reserved.
//

#import "MetalView.h"

@implementation MetalView

/** Returns a Metal-compatible layer. */
+(Class) layerClass {
  return [CAMetalLayer class];
}

@end
