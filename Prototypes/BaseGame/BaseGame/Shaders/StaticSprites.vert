#version 450

struct Transform {
  mat4 model;
  mat4 atlas;
};

layout(set = 0, binding = 0) buffer TransformsBuffer {
  Transform transforms[];
};

layout(set = 1, binding = 0) uniform CameraUniforms {
  mat4 projection;
  mat4 view;
  vec3 position;
} camera;

const vec4 positions[4] = {
  {-0.5, -0.5, 0.0, 1.0},
  {-0.5,  0.5, 0.0, 1.0},
  { 0.5, -0.5, 0.0, 1.0},
  { 0.5,  0.5, 0.0, 1.0}
};

const vec4 uvCoords[4] = {
  {0.0, 0.0, 0.0, 1.0},
  {0.0, 1.0, 0.0, 1.0},
  {1.0, 0.0, 0.0, 1.0},
  {1.0, 1.0, 0.0, 1.0}
};

layout(location = 0) out vec3 outNormal;
layout(location = 1) out vec2 outUV;

void main() {
  Transform transform = transforms[gl_InstanceIndex];
  gl_Position = camera.projection * camera.view * transform.model * positions[gl_VertexIndex];
  outNormal = normalize(mat3(transform.model) * vec3(0.0, 0.0, 1.0));
  outUV = (transform.atlas * uvCoords[gl_VertexIndex]).xy;
}
