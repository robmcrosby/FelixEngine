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
                                 constant float4x4 *View       [[ buffer(2) ]],
                                 constant float4x4 *Projection [[ buffer(3) ]],
                                          uint      vid        [[ vertex_id ]])
{
  VertexOutput outVertices;
  outVertices.m_Position = *Projection * *View * *Model * Position[vid];
  outVertices.m_Position.z = (outVertices.m_Position.z/2.0)+0.5;
  return outVertices;
}

fragment half4 SimpleColor(constant float4 *Color [[ buffer(4) ]])
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

vertex VertexUVOutput SimpleVertexUV(device float4 *Position [[ buffer(0) ]],
                                     device float2 *UV_0     [[ buffer(1) ]],
                                     uint    vid      [[ vertex_id ]])
{
  VertexUVOutput outVertices;
  
  outVertices.m_Position   = Position[vid];
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



struct VertNormalOutput
{
  float4 m_Position [[position]];
  float4 m_Normal   [[user(normal)]];
};

struct FragNormalInput
{
  float4 m_Position [[position]];
  float4 m_Normal   [[user(normal)]];
};


vertex VertNormalOutput SimpleNormal(device   float4   *Position   [[ buffer(0) ]],
                                     device   float4   *Normal     [[ buffer(1) ]],
                                     constant float4x4 *Model      [[ buffer(2) ]],
                                     constant float4x4 *View       [[ buffer(3) ]],
                                     constant float4x4 *Projection [[ buffer(4) ]],
                                     constant float4x4 *Rotation   [[ buffer(5) ]],
                                              uint      vid        [[ vertex_id ]])
{
  VertNormalOutput outVertices;
  outVertices.m_Position = *Projection * *View * *Model * Position[vid];
  outVertices.m_Position.z = (outVertices.m_Position.z/2.0)+0.5;
  outVertices.m_Normal = *Rotation * Normal[vid];
  return outVertices;
}

fragment half4 DirectLight(FragNormalInput  inFrag [[ stage_in  ]],
                           constant float4 *Color  [[ buffer(6) ]])
{
  float d = dot(inFrag.m_Normal.xyz, float3(0.0, 1.0, 0.0));
  return half4(Color->r*d, Color->g*d, Color->b*d, Color->a);
}
