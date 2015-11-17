//
//  DefaultLibrary.metal
//  FelixEngine
//
//  Created by Robert Crosby on 10/20/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#include <metal_stdlib>

using namespace metal;

struct VertexOutput
{
  float4 m_Position [[position]];
};

vertex VertexOutput SimpleVertex(device   float4   *Position   [[ buffer(0) ]],
                                 constant float4x4 *Model      [[ buffer(1) ]],
                                 constant float4x4 *Projection [[ buffer(2) ]],
                                          uint      vid        [[ vertex_id ]])
{
  VertexOutput outVertices;
  outVertices.m_Position = *Projection * *Model * Position[vid];
  outVertices.m_Position.z = (outVertices.m_Position.z/2.0)+0.5;
  return outVertices;
}

fragment half4 SimpleColor(constant float4 *Color [[ buffer(3) ]])
{
  return half4(*Color);
}



struct VertexUVOutput
{
  float4 m_Position   [[position]];
  float2 m_Coordinate [[user(texturecoord)]];
};

struct FragmentUVInput
{
  float4 m_Position   [[position]];
  float2 m_Coordinate [[user(texturecoord)]];
};

vertex VertexUVOutput SimpleVertexUV(device   float4   *Position   [[ buffer(0) ]],
                                     device   float2   *UV_0       [[ buffer(1) ]],
                                     constant float4x4 *Model      [[ buffer(2) ]],
                                     constant float4x4 *Projection [[ buffer(3) ]],
                                              uint      vid        [[ vertex_id ]])
{
  VertexUVOutput outVertices;
  
  outVertices.m_Position   = *Projection * *Model * Position[vid];
  outVertices.m_Position.z = (outVertices.m_Position.z/2.0)+0.5;
  outVertices.m_Coordinate = UV_0[vid];
  outVertices.m_Coordinate.y = 1.0 - outVertices.m_Coordinate.y;
  return outVertices;
}

fragment half4 SimpleTexture(FragmentUVInput   inFrag    [[ stage_in ]],
                             texture2d<float>  tex2D     [[ texture(0) ]],
                             sampler           sampler2D [[ sampler(0) ]])
{
  float4 color = tex2D.sample(sampler2D, inFrag.m_Coordinate);
  return half4(color.r, color.g, color.b, color.a);
}
