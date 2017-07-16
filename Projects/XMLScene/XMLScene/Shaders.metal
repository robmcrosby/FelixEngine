//
//  Shaders.metal
//  XMLScene
//
//  Created by Robert Crosby on 7/16/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#include <metal_stdlib>
using namespace metal;

vertex float4 basic_vertex(const device packed_float4* vertex_array [[ buffer(0) ]],
                           unsigned int   vid          [[ vertex_id ]]) {
  return float4(vertex_array[vid]);
}

fragment half4 basic_fragment() {
  return half4(1.0);
}
