//
//  MTLWindowObj.m
//  FelixEngine
//
//  Created by Robert Crosby on 9/27/14.
//  Copyright (c) 2014 Robert Crosby. All rights reserved.
//

#import "MTLGraphicResources.h"


@implementation MTLWindow
{
  id <MTLDevice> _mtlDevice;
  
  MTLTexture *_colorTexture;
  MTLTexture *_depthTexture;
  MTLFrame   *_mtlFrame;
  
  NSView       *_nsView;
  CAMetalLayer *_layer;
  
  id <CAMetalDrawable>  _drawable;
  
  dispatch_semaphore_t  _inflightSemaphore;
}

-(id)init
{
  self = [super init];
  
  if (self != nil)
  {
    _mtlDevice = nil;
    _colorTexture = nil;
    _depthTexture = nil;
    _mtlFrame = nil;
    _commandBuffer = nil;
    
    _nsView = nil;
    _layer = nil;
    
    _drawable = nil;
    
    _inflightSemaphore = dispatch_semaphore_create(1);
  }
  
  return self;
}

-(void)setMetalDevice:(id <MTLDevice>)device
{
  _mtlDevice = device;
  [_mtlFrame setMetalDevice:device];
  
  _colorTexture = [[MTLTexture alloc] initWithDevice:_mtlDevice];
  [_colorTexture setFormat:MTLPixelFormatBGRA8Unorm];
  [_mtlFrame addColorBuffer:_colorTexture];
  
  _depthTexture = [[MTLTexture alloc] initWithDevice:_mtlDevice];
  [_depthTexture setFormat:MTLPixelFormatDepth32Float];
  [_mtlFrame setDepthBuffer:_depthTexture];
}

-(void)setMetalFrame:(MTLFrame*)frame
{
  _mtlFrame = frame;
}

-(void)dealloc
{
  _mtlDevice = nil;
  _colorTexture = nil;
  _depthTexture = nil;
  _mtlFrame = nil;
  
  _commandBuffer = nil;
  
  _nsView = nil;
  _layer = nil;
  
  _drawable = nil;
  _commandBuffer = nil;
}

-(BOOL)setNSView:(NSView*)nsView
{
  _nsView = nsView;
  
  if (_mtlDevice != nil)
  {
    // Setup metal layer and add as sub layer to view
    _layer = [CAMetalLayer layer];
    _layer.device = _mtlDevice;
    _layer.pixelFormat = MTLPixelFormatBGRA8Unorm;
    
    _layer.presentsWithTransaction = NO;
    _layer.drawsAsynchronously     = YES;
    _layer.needsDisplayOnBoundsChange = YES;
    _layer.contentsScale = 1.0f;
    //_layer.contentsScale = [UIScreen mainScreen].scale;
    
    // Change this to NO if the compute encoder is used as the last pass on the drawable texture
    _layer.framebufferOnly = YES;
    
    // Add metal layer to the views layer hierarchy
    [_layer setFrame:_nsView.layer.frame];
    [_nsView.layer addSublayer:_layer];
    
    //_nsView.opaque = YES;
    //_nsView.backgroundColor = nil;
    //_nsView.contentScaleFactor = [UIScreen mainScreen].scale;
    
    if (_mtlFrame != nil)
      [_mtlFrame setWidth:_nsView.layer.frame.size.width andHeight:_nsView.layer.frame.size.height];
    
    return YES;
  }
  
  return NO;
}

-(BOOL)setupCommandBufferWithQueue:(id <MTLCommandQueue>)queue
{
  if (queue == nil)
  {
    NSLog(@"CommandQueue is nil");
    return NO;
  }
  
  dispatch_semaphore_wait(_inflightSemaphore, DISPATCH_TIME_FOREVER);
  
  // Resize the Metal Layer when needed.
  CGRect viewBounds = _nsView.layer.bounds;
  CGRect layerFrame = _layer.frame;
  if (viewBounds.size.width != layerFrame.size.width || viewBounds.size.height != layerFrame.size.height)
  {
    //[_layer setDrawableSize:viewBounds.size];
    [_layer setDrawableSize:CGSizeMake(viewBounds.size.width*2.0, viewBounds.size.height*2.0)];
    [_layer setFrame:viewBounds];
  }
  
  _drawable = [_layer nextDrawable];
  if (_drawable == nil)
  {
    NSLog(@"NextDrawable Error");
    return NO;
  }
  
  [_colorTexture setTexture:_drawable.texture withFormat:MTLPixelFormatBGRA8Unorm];
  [_mtlFrame updateWindowBuffer];
  
  _commandBuffer = [queue commandBuffer];
  
  return YES;
}

-(void)queueCommandBuffer
{
  // Create a Completed Handler to dispatch the Semaphore when the Queue is done.
  __block dispatch_semaphore_t dispatchSemaphore = _inflightSemaphore;
  [self.commandBuffer addCompletedHandler:^(id <MTLCommandBuffer> cmdb) {
    dispatch_semaphore_signal(dispatchSemaphore);
  }];
  
  // Commit the Command Buffer to draw.
  [self.commandBuffer presentDrawable:_drawable];
  [self.commandBuffer commit];
  
  _drawable = nil;
  
  //[_mtlFrame setRenderBuffer:nil];
  
  
  _commandBuffer = nil;
}

@end
