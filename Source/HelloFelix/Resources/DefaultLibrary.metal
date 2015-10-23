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

vertex VertexOutput SimpleVertex(device float4 *Position [[ buffer(0) ]],
                                        uint    vid      [[ vertex_id ]])
{
  VertexOutput outVertices;
  outVertices.m_Position = Position[vid];
  return outVertices;
}

//fragment half4 SimpleColor(constant float4 *Color [[ buffer(1) ]])
//{
//  return half4(*Color);
//  //return half4(1.0, 1.0, 1.0, 1.0);
//}

fragment half4 SimpleColor()
{
  return half4(1.0, 1.0, 1.0, 1.0);
}
