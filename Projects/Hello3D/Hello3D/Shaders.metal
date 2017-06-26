//
//  Shaders.metal
//  Hello3D
//
//  Created by Robert Crosby on 6/22/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#include <metal_stdlib>
using namespace metal;

vertex float4 basic_vertex(const device packed_float4* Position [[ buffer(0) ]],
                                        unsigned int   vid      [[ vertex_id ]]) {
  return float4(Position[vid]);
}

fragment half4 basic_fragment() {
  return half4(1.0);
}
