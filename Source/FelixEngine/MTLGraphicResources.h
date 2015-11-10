//
//  MTLGraphicResources.h
//  FelixEngine
//
//  Created by Robert Crosby on 10/19/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#import <Foundation/Foundation.h>

#if !TARGET_IPHONE_SIMULATOR
#import <QuartzCore/CAMetalLayer.h>
#import <Metal/Metal.h>

#if TARGET_OS_IPHONE
  #import <UIKit/UIKit.h>
#else
  #import <Cocoa/Cocoa.h>
#endif


@interface MTLShader : NSObject

@property (nonatomic, readonly) id <MTLFunction> vertexFunction;
@property (nonatomic, readonly) id <MTLFunction> fragmentFunction;

@property (nonatomic, readonly) NSUInteger shaderId;

-(id)initWithDevice:(id <MTLDevice>)device andLibrary:(id <MTLLibrary>)library;

-(BOOL)loaded;

-(BOOL)setVertexFunction:(NSString*)name;
-(BOOL)setFragmentFunction:(NSString*)name;

-(BOOL)hasReflection;
-(void)setReflection:(MTLRenderPipelineReflection*)reflection;

-(NSInteger)getVertexLocationOf:(NSString*)name;
-(NSInteger)getFragmentLocationOf:(NSString*)name;

-(NSInteger)getAttributeLocationOf:(NSString*)name;

@end





@interface MTLMesh : NSObject

-(id)initWithDevice:(id <MTLDevice>)device;

-(BOOL)loaded;

-(BOOL)addVertexBuffer:(const void*)buffer withElementSize:(NSUInteger)elementSize withNumElements:(NSUInteger)numElements forName:(NSString*)name;
-(BOOL)setIndexBuffer:(const void*)buffer withNumIndices:(NSUInteger)numIndices;
-(void)setPrimitiveType:(MTLPrimitiveType)type;

-(void)drawToEncoder:(id <MTLRenderCommandEncoder>)encoder withShader:(MTLShader*)shader instanceCount:(NSUInteger)instances;

@end




@interface MTLTexture : NSObject

@property (nonatomic, readonly)  id <MTLTexture> texture;
@property (nonatomic, readonly)  MTLTextureType type;
@property (nonatomic, readonly)  MTLPixelFormat format;
@property (nonatomic, readonly)  NSUInteger width;
@property (nonatomic, readonly)  NSUInteger height;

@property (nonatomic, readwrite) MTLTextureUsage usage;

-(id)initWithDevice:(id <MTLDevice>)device;

-(BOOL)loaded;

-(BOOL)loadImage:(const void*)image Width:(NSUInteger)width Height:(NSUInteger)height;
-(BOOL)resizeToWidth:(NSUInteger)width Height:(NSUInteger)height;
-(BOOL)resizeToWidth:(NSUInteger)width Height:(NSUInteger)height Format:(MTLPixelFormat)format;
-(BOOL)setFormat:(MTLPixelFormat)format;

-(void)setTexture:(id<MTLTexture>)texture Format:(MTLPixelFormat)format;
-(void)applyToEncoder:(id <MTLRenderCommandEncoder>)encoder atIndex:(NSUInteger)index;

@end





@interface MTLPipelineKey : NSObject<NSCopying>

@property (nonatomic, readwrite) MTLShader *shader;
@property (nonatomic, readwrite) BOOL blendingEnabled;

@end





@interface MTLDescriptorKey : NSObject<NSCopying>

@property (nonatomic, readwrite) MTLClearColor clearColor;
@property (nonatomic, readwrite) BOOL clearColorBuffers;

@property (nonatomic, readwrite) CGFloat clearDepth;
@property (nonatomic, readwrite) BOOL clearDepthBuffer;

@end




@interface MTLFrame : NSObject

@property (nonatomic, readonly) NSUInteger width;
@property (nonatomic, readonly) NSUInteger height;
@property (nonatomic, readonly) NSUInteger frameId;

-(id)initWithDevice:(id <MTLDevice>)device;

-(BOOL)loaded;

-(void)setMetalDevice:(id <MTLDevice>)device;

-(void)resizeToWidth:(NSUInteger)width Height:(NSUInteger)height;
-(void)setWidth:(NSUInteger)width Height:(NSUInteger)height;
-(void)updateWindowBufferAtWidth:(NSUInteger)width Height:(NSUInteger)height;

-(void)addColorBuffer:(MTLTexture*)texture;
-(void)setDepthBuffer:(MTLTexture*)texture;

-(id <MTLRenderPipelineState>)getPipelineForKey:(MTLPipelineKey*)key;
-(MTLRenderPassDescriptor *)getDescriptorForKey:(MTLDescriptorKey*)key;

@end




@interface MTLWindow : NSObject

-(void)setMetalDevice:(id <MTLDevice>)device;
-(void)setMetalFrame:(MTLFrame*)frame;

-(void)setNextDrawable;
-(void)presentDrawable:(id <MTLCommandBuffer>)cmdBuffer;

#if TARGET_OS_IPHONE
-(BOOL)setUIWindow:(UIWindow*)window;
#else
-(BOOL)setNSWindow:(NSWindow*)window;
#endif

@end



@interface MTLUniform : NSObject<NSCopying>

@property (nonatomic, readwrite) NSString *name;
@property (nonatomic, readwrite) NSUInteger offset;

@end



@interface MTLUniformMap : NSObject

@property (nonatomic, readwrite) NSArray *uniforms;
@property (nonatomic, readwrite) id <MTLBuffer> buffer;

-(id)initWithDevice:(id <MTLDevice>)device;

-(void)setUniformsSize:(NSUInteger)size;
-(void)setBufferSize:(NSUInteger)size;

-(void)applyToShader:(MTLShader*)shader withEncoder:(id <MTLRenderCommandEncoder>)encoder;

@end

#endif
