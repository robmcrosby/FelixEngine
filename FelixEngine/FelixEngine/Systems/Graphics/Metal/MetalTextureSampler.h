//
//  MetalTextureSampler.h
//  FelixEngine
//
//  Created by Robert Crosby on 7/4/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#ifndef MetalTextureSampler_h
#define MetalTextureSampler_h

#import <Foundation/Foundation.h>
#import <Metal/Metal.h>


@interface MetalTextureSampler : NSObject

-(id)initWithDevice:(id <MTLDevice>)device;
-(id <MTLSamplerState>)samplerStateForFlags:(int)flags;

@end

#endif /* MetalTextureSampler_h */
