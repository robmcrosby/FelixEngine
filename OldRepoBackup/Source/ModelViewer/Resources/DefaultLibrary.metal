//
//  DefaultLibrary.metal
//  FelixEngine
//
//  Created by Robert Crosby on 10/20/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#include <metal_stdlib>

using namespace metal;

float4 adjustPos(float4 pos, float4 loc);
float2 adjustUV(float2 uv, float4 adj);

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

vertex VertexUVOutput SimpleVertexUV(device float4   *Position [[ buffer(0) ]],
                                     device float2   *UV_0     [[ buffer(1) ]],
                                     device float4x4 *ViewRot  [[ buffer(2) ]],
                                            uint      vid      [[ vertex_id ]])
{
  VertexUVOutput outVertices;
  
  outVertices.m_Position   = *ViewRot * Position[vid];
  outVertices.m_Position.z = (outVertices.m_Position.z/2.0)+0.5;
  outVertices.m_Coordinate = UV_0[vid];
  outVertices.m_Coordinate.y = 1.0 - outVertices.m_Coordinate.y;
  return outVertices;
}

vertex VertexUVOutput VertexUV(device   float4   *Position   [[ buffer(0) ]],
                               device   float2   *UV_0       [[ buffer(1) ]],
                               constant float4x4 *Model      [[ buffer(2) ]],
                               constant float4x4 *View       [[ buffer(3) ]],
                               constant float4x4 *Projection [[ buffer(4) ]],
                                        uint      vid        [[ vertex_id ]])
{
  VertexUVOutput outVertices;
  
  outVertices.m_Position   = *Projection * *View * *Model * Position[vid];
  outVertices.m_Position.z = (outVertices.m_Position.z/2.0)+0.5;
  outVertices.m_Coordinate = UV_0[vid];
  outVertices.m_Coordinate.y = 1.0 - outVertices.m_Coordinate.y;
  return outVertices;
}



float4 adjustPos(float4 pos, float4 loc)
{
  pos.x = (pos.x * loc.z) + loc.x;
  pos.y = (pos.y * loc.w) + loc.y;
  return pos;
}

float2 adjustUV(float2 uv, float4 adj)
{
  uv.x = (1.0-uv.x) * adj.x + uv.x * adj.z;
  uv.y = uv.y * adj.y + (1.0-uv.y) * adj.w;
  return uv;
}

vertex VertexUVOutput TextUV(device   float4   *Position   [[ buffer(0) ]],
                             device   float2   *UV_0       [[ buffer(1) ]],
                             constant float4x4 *Model      [[ buffer(2) ]],
                             constant float4x4 *View       [[ buffer(3) ]],
                             constant float4x4 *Projection [[ buffer(4) ]],
                             constant float4   *Locs       [[ buffer(5) ]],
                             constant float4   *UVs        [[ buffer(6) ]],
                                      uint      iid        [[ instance_id]],
                                      uint      vid        [[ vertex_id ]])
{
  VertexUVOutput outVertices;
  
  outVertices.m_Position   = *Projection * *View * *Model * adjustPos(Position[vid], Locs[iid]);
  outVertices.m_Position.z = (outVertices.m_Position.z/2.0)+0.5;
  outVertices.m_Coordinate = adjustUV(UV_0[vid], UVs[iid]);
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
