//
//  MTLGraphicResources.h
//  FelixEngine
//
//  Created by Robert Crosby on 10/19/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import <Foundation/Foundation.h>
#import <QuartzCore/CAMetalLayer.h>
#import <Metal/Metal.h>



@interface MTLShader : NSObject

@property (nonatomic, readonly) id <MTLFunction> vertexFunction;
@property (nonatomic, readonly) id <MTLFunction> fragmentFunction;

@property (nonatomic, readonly) NSUInteger shaderId;

-(id)initWithDevice:(id <MTLDevice>)device andLibrary:(id <MTLLibrary>)library;

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

-(id)initWithDevice:(id <MTLDevice>)device;

-(BOOL)loadImage2d:(const void*)image withWidth:(NSUInteger)width andHeight:(NSUInteger)height;
-(BOOL)resizeToWidth:(NSUInteger)width andHeight:(NSUInteger)height;
-(BOOL)resizeToWidth:(NSUInteger)width andHeight:(NSUInteger)height withFormat:(MTLPixelFormat)format;
-(BOOL)setFormat:(MTLPixelFormat)format;

-(void)setTexture:(id<MTLTexture>)texture withFormat:(MTLPixelFormat)format;
-(void)setToEncoder:(id <MTLRenderCommandEncoder>)encoder atIndex:(NSUInteger)index;

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

-(void)setMetalDevice:(id <MTLDevice>)device;

-(void)resizeToWidth:(NSUInteger)width andHeight:(NSUInteger)height;
-(void)setWidth:(NSUInteger)width andHeight:(NSUInteger)height;
-(void)updateWindowBuffer;

-(void)addColorBuffer:(MTLTexture*)texture;
-(void)setDepthBuffer:(MTLTexture*)texture;

-(id <MTLRenderPipelineState>)getPipelineForKey:(MTLPipelineKey*)key;
-(MTLRenderPassDescriptor *)getDescriptorForKey:(MTLDescriptorKey*)key;

@end




@interface MTLWindow : NSObject

-(void)setMetalDevice:(id <MTLDevice>)device;
-(void)setMetalFrame:(MTLFrame*)frame;

-(BOOL)setNSWindow:(NSWindow*)window;

-(void)setNextDrawable;
-(void)presentDrawable:(id <MTLCommandBuffer>)cmdBuffer;

@end
