//
//  MetalDepthStencil.h
//  FelixEngine
//
//  Created by Robert Crosby on 6/28/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#ifndef MetalDepthStencil_h
#define MetalDepthStencil_h

#import <Foundation/Foundation.h>
#import <Metal/Metal.h>


@interface MetalDepthStencil : NSObject

-(id)initWithDevice:(id <MTLDevice>)device;
-(id <MTLDepthStencilState>)depthStencilStateForFlags:(int)flags;

@end

#endif /* MetalDepthStencil_h */
