//
//  MetalShaders.metal
//  FelixEngine
//
//  Created by Robert Crosby on 11/29/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#define NUM_LIGHTS 2
#define FOUR_LIGHTS 4

#include "MetalFunctions.h"




struct OutputShadeless {
  float4 position [[ position ]];
};

vertex OutputShadeless v_shadeless(const device packed_float3 *position [[ buffer(0) ]],
                                   constant     STR_Camera    *camera   [[ buffer(1) ]],
                                   constant     STR_Model     *model    [[ buffer(2) ]],
                                   unsigned     int            vid      [[ vertex_id ]],
                                   unsigned     int            iid      [[ instance_id ]]) {
  OutputShadeless output;
  float4 location = model[iid].model * float4(position[vid], 1.0);
  output.position = camera->projection * camera->view * location;
  return output;
}

fragment half4 f_shadeless(constant STR_Material *material [[ buffer(0) ]]) {
  float3 color = material->ambiant.xyz * material->ambiant.a;
  return half4(color.x, color.y, color.z, 1.0);
}




struct OutputColor {
  float4 position [[ position    ]];
  float3 color    [[ user(color) ]];
};

struct InputColor {
  float3 color [[ user(color) ]];
};

vertex OutputColor v_color(const device packed_float3 *position [[ buffer(0) ]],
                           const device packed_float3 *color0   [[ buffer(1) ]],
                           constant     STR_Camera    *camera   [[ buffer(2) ]],
                           constant     STR_Model     *model    [[ buffer(3) ]],
                           unsigned     int            vid      [[ vertex_id ]],
                           unsigned     int            iid      [[ instance_id ]]) {
  OutputColor output;
  float4 location = model[iid].model * float4(position[vid], 1.0);
  output.position = camera->projection * camera->view * location;
  output.color = float3(color0[vid]);
  return output;
}

fragment half4 f_color_shadeless(InputColor             input     [[ stage_in ]],
                                 constant STR_Material *material  [[ buffer(0) ]]) {
  float3 color = input.color * material->ambiant.xyz * material->ambiant.a;
  return half4(color.x, color.y, color.z, 1.0);
}




struct OutputTexture {
  float4 position   [[ position ]];
  float2 coordinate [[ user(uv) ]];
};

struct InputTexture {
  float2 coordinate [[ user(uv) ]];
};

vertex OutputTexture v_texture(const device packed_float3 *position   [[ buffer(0) ]],
                               const device packed_float2 *uv0        [[ buffer(1) ]],
                               constant     STR_Camera    *camera     [[ buffer(2) ]],
                               constant     STR_Model     *model      [[ buffer(3) ]],
                               unsigned     int            vid        [[ vertex_id ]],
                               unsigned     int            iid        [[ instance_id ]]) {
  OutputTexture output;
  float4 location = model[iid].model * float4(position[vid], 1.0);
  output.position = camera->projection * camera->view * location;
  output.coordinate = transform_coordinate(model->texture, float2(uv0[vid]));
  return output;
}

fragment half4 f_texture_shadeless(InputTexture           input     [[ stage_in ]],
                                   constant STR_Material *material  [[ buffer(0) ]],
                                   texture2d<float>       texture2D [[ texture(0) ]],
                                   sampler                sampler2D [[ sampler(0) ]]) {
  float4 texture = texture2D.sample(sampler2D, input.coordinate);
  float3 color = texture.xyz * material->ambiant.xyz * material->ambiant.a;
  return half4(color.x, color.y, color.z, 1.0);
}



vertex OutputTexture video_vertex(const device packed_float3 *position  [[ buffer(0) ]],
                                  const device packed_float2 *uv0       [[ buffer(1) ]],
                                  constant     float4x4      *transform [[ buffer(2) ]],
                                  unsigned int   vid       [[ vertex_id ]]) {
  OutputTexture output;
  output.position = float4(position[vid], 1.0);
  output.coordinate = (*transform * float4(uv0[vid], 0.0, 1.0)).xy;
  return output;
}

fragment float4 video_fragment_Y_CbCr(InputTexture input [[ stage_in ]],
                                      texture2d<float, access::sample> capturedImageTextureY    [[ texture(0) ]],
                                      texture2d<float, access::sample> capturedImageTextureCbCr [[ texture(1) ]]) {
  constexpr sampler colorSampler(mip_filter::linear, mag_filter::linear, min_filter::linear);
  const float4x4 ycbcrToRGBTransform = float4x4(
                                                float4(+1.0000f, +1.0000f, +1.0000f, +0.0000f),
                                                float4(+0.0000f, -0.3441f, +1.7720f, +0.0000f),
                                                float4(+1.4020f, -0.7141f, +0.0000f, +0.0000f),
                                                float4(-0.7010f, +0.5291f, -0.8860f, +1.0000f)
                                                );
  
  // Sample Y and CbCr textures to get the YCbCr color at the given texture coordinate
  float4 ycbcr = float4(capturedImageTextureY.sample(colorSampler, input.coordinate).r,
                        capturedImageTextureCbCr.sample(colorSampler, input.coordinate).rg, 1.0);
  
  // Return converted RGB color
  return ycbcrToRGBTransform * ycbcr;
}




struct OutputNormal {
  float4 position [[ position   ]];
  float3 location [[ user(loc)  ]];
  float3 normal   [[ user(norm) ]];
  float3 view     [[ user(view) ]];
};

struct InputNormal {
  float3 location [[ user(loc)  ]];
  float3 normal   [[ user(norm) ]];
  float3 view     [[ user(view) ]];
};

vertex OutputNormal v_normal(const device packed_float3 *position [[ buffer(0) ]],
                             const device packed_float3 *normal   [[ buffer(1) ]],
                             constant     STR_Camera    *camera   [[ buffer(2) ]],
                             constant     STR_Model     *model    [[ buffer(3) ]],
                             unsigned     int            vid      [[ vertex_id ]],
                             unsigned     int            iid      [[ instance_id ]]) {
  OutputNormal output;
  float4 location = model[iid].model * float4(position[vid], 1.0);
  output.position = camera->projection * camera->view * location;
  output.location = location.xyz;
  output.normal = rotate_quat(model[iid].rotation, float3(normal[vid]));
  output.view = camera->position.xyz - output.position.xyz;
  return output;
}

#define LIGHT_FUNC(DIFFUSE, SPECULAR, LIGHTS) \
fragment half4 f_##DIFFUSE##_##SPECULAR##_##LIGHTS(InputNormal input [[stage_in]], \
constant STR_Light *lights [[buffer(0)]], constant STR_Material *material [[buffer(1)]]) { \
  LightingParams lightParams; MaterialParams materialParams; \
  setupLightingParams(&lightParams, input.normal, input.view); \
  setupMaterialParams(&materialParams, *material); \
  float3 color = materialParams.ambiantColor; \
  LIGHT_LOOP(DIFFUSE, SPECULAR, 0, LIGHTS) \
  return half4(color.x, color.y, color.z, 1.0); \
}

#define LIGHT_FUNCS(DIFFUSE, SPECULAR) \
LIGHT_FUNC(DIFFUSE, SPECULAR, 1) \
LIGHT_FUNC(DIFFUSE, SPECULAR, 2) \
LIGHT_FUNC(DIFFUSE, SPECULAR, 4) \
LIGHT_FUNC(DIFFUSE, SPECULAR, 8)

LIGHT_FUNCS(lambert, phong)
LIGHT_FUNCS(lambert, cooktorr)
LIGHT_FUNCS(lambert, toon_s)

LIGHT_FUNCS(oren_nayer, phong)
LIGHT_FUNCS(oren_nayer, cooktorr)
LIGHT_FUNCS(oren_nayer, toon_s)

LIGHT_FUNCS(toon_d, phong)
LIGHT_FUNCS(toon_d, cooktorr)
LIGHT_FUNCS(toon_d, toon_s)

LIGHT_FUNCS(minnaert, phong)
LIGHT_FUNCS(minnaert, cooktorr)
LIGHT_FUNCS(minnaert, toon_s)




struct OutputColorNormal {
  float4 position [[ position    ]];
  float3 location [[ user(loc)   ]];
  float3 normal   [[ user(norm)  ]];
  float3 view     [[ user(view)  ]];
  float3 color    [[ user(color) ]];
};

struct InputColorNormal {
  float3 location [[ user(loc)   ]];
  float3 normal   [[ user(norm)  ]];
  float3 view     [[ user(view)  ]];
  float3 color    [[ user(color) ]];
};

vertex OutputColorNormal v_color_normal(const device packed_float3 *position [[ buffer(0) ]],
                                        const device packed_float3 *normal   [[ buffer(1) ]],
                                        const device packed_float3 *color0   [[ buffer(2) ]],
                                        constant     STR_Camera    *camera   [[ buffer(3) ]],
                                        constant     STR_Model     *model    [[ buffer(4) ]],
                                        unsigned     int            vid      [[ vertex_id ]],
                                        unsigned     int            iid      [[ instance_id ]]) {
  OutputColorNormal output;
  float4 location = model[iid].model * float4(position[vid], 1.0);
  output.position = camera->projection * camera->view * location;
  output.location = location.xyz;
  output.normal = rotate_quat(model[iid].rotation, float3(normal[vid]));
  output.view = camera->position.xyz - output.position.xyz;
  output.color = float3(color0[vid]);
  return output;
}

#define LIGHT_COLOR_FUNC(DIFFUSE, SPECULAR, LIGHTS) \
fragment half4 f_color_##DIFFUSE##_##SPECULAR##_##LIGHTS(InputColorNormal input [[stage_in]], \
constant STR_Light *lights [[buffer(0)]], constant STR_Material *material [[buffer(1)]]) { \
  LightingParams lightParams; MaterialParams materialParams; \
  setupLightingParams(&lightParams, input.normal, input.view); \
  setupMaterialParams(&materialParams, *material, input.color); \
  float3 color = materialParams.ambiantColor * input.color; \
  LIGHT_LOOP(DIFFUSE, SPECULAR, 0, LIGHTS) \
  return half4(color.x, color.y, color.z, 1.0); \
}

#define LIGHT_COLOR_FUNCS(DIFFUSE, SPECULAR) \
LIGHT_COLOR_FUNC(DIFFUSE, SPECULAR, 1) \
LIGHT_COLOR_FUNC(DIFFUSE, SPECULAR, 2) \
LIGHT_COLOR_FUNC(DIFFUSE, SPECULAR, 4) \
LIGHT_COLOR_FUNC(DIFFUSE, SPECULAR, 8)

LIGHT_COLOR_FUNCS(lambert, phong)
LIGHT_COLOR_FUNCS(lambert, cooktorr)
LIGHT_COLOR_FUNCS(lambert, toon_s)

LIGHT_COLOR_FUNCS(oren_nayer, phong)
LIGHT_COLOR_FUNCS(oren_nayer, cooktorr)
LIGHT_COLOR_FUNCS(oren_nayer, toon_s)

LIGHT_COLOR_FUNCS(toon_d, phong)
LIGHT_COLOR_FUNCS(toon_d, cooktorr)
LIGHT_COLOR_FUNCS(toon_d, toon_s)

LIGHT_COLOR_FUNCS(minnaert, phong)
LIGHT_COLOR_FUNCS(minnaert, cooktorr)
LIGHT_COLOR_FUNCS(minnaert, toon_s)



struct OutputTextureNormal {
  float4 position   [[ position   ]];
  float3 location   [[ user(loc)  ]];
  float3 normal     [[ user(norm) ]];
  float3 view       [[ user(view) ]];
  float2 coordinate [[ user(uv)   ]];
};

struct InputTextureNormal {
  float3 location   [[ user(loc)  ]];
  float3 normal     [[ user(norm) ]];
  float3 view       [[ user(view) ]];
  float2 coordinate [[ user(uv)   ]];
};

vertex OutputTextureNormal v_texture_normal(const device packed_float3 *position [[ buffer(0) ]],
                                            const device packed_float3 *normal   [[ buffer(1) ]],
                                            const device packed_float2 *uv0      [[ buffer(2) ]],
                                            constant     STR_Camera    *camera   [[ buffer(3) ]],
                                            constant     STR_Model     *model    [[ buffer(4) ]],
                                            unsigned     int            vid      [[ vertex_id ]],
                                            unsigned     int            iid      [[ instance_id ]]) {
  OutputTextureNormal output;
  float4 location = model[iid].model * float4(position[vid], 1.0);
  output.position = camera->projection * camera->view * location;
  output.location = location.xyz;
  output.normal = rotate_quat(model[iid].rotation, float3(normal[vid]));
  output.view = camera->position.xyz - output.position.xyz;
  output.coordinate = float2(uv0[vid]);
  return output;
}

#define LIGHT_TEXTURE_FUNC(DIFFUSE, SPECULAR, LIGHTS) \
fragment half4 f_texture_##DIFFUSE##_##SPECULAR##_##LIGHTS(InputTextureNormal input [[stage_in]], \
constant STR_Light *lights [[buffer(0)]], constant STR_Material *material [[buffer(1)]], \
texture2d<float> texture2D [[texture(0)]], sampler sampler2D [[sampler(0)]]) { \
  LightingParams lightParams; MaterialParams materialParams; \
  float4 texture = texture2D.sample(sampler2D, input.coordinate); \
  setupLightingParams(&lightParams, input.normal, input.view); \
  setupMaterialParams(&materialParams, *material, texture.xyz); \
  float3 color = materialParams.ambiantColor * texture.xyz; \
  LIGHT_LOOP(DIFFUSE, SPECULAR, 0, LIGHTS) \
  return half4(color.x, color.y, color.z, texture.w); \
}

#define LIGHT_TEXTURE_FUNCS(DIFFUSE, SPECULAR) \
LIGHT_TEXTURE_FUNC(DIFFUSE, SPECULAR, 1) \
LIGHT_TEXTURE_FUNC(DIFFUSE, SPECULAR, 2) \
LIGHT_TEXTURE_FUNC(DIFFUSE, SPECULAR, 4) \
LIGHT_TEXTURE_FUNC(DIFFUSE, SPECULAR, 8)

LIGHT_TEXTURE_FUNCS(lambert, phong)
LIGHT_TEXTURE_FUNCS(lambert, cooktorr)
LIGHT_TEXTURE_FUNCS(lambert, toon_s)

LIGHT_TEXTURE_FUNCS(oren_nayer, phong)
LIGHT_TEXTURE_FUNCS(oren_nayer, cooktorr)
LIGHT_TEXTURE_FUNCS(oren_nayer, toon_s)

LIGHT_TEXTURE_FUNCS(toon_d, phong)
LIGHT_TEXTURE_FUNCS(toon_d, cooktorr)
LIGHT_TEXTURE_FUNCS(toon_d, toon_s)

LIGHT_TEXTURE_FUNCS(minnaert, phong)
LIGHT_TEXTURE_FUNCS(minnaert, cooktorr)
LIGHT_TEXTURE_FUNCS(minnaert, toon_s)
