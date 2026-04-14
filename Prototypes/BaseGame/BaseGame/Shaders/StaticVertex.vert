#version 450

layout(set = 0, binding = 0) uniform TransformUniforms {
  mat4 model;
} transform;

layout(set = 1, binding = 0) uniform CameraUniforms {
  mat4 projection;
  mat4 view;
  vec3 position;
} camera;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;

layout(location = 0) out vec3 outNormal;
layout(location = 1) out vec2 outUV;

void main() {
  gl_Position = camera.projection * camera.view * transform.model * vec4(inPosition, 1.0);
  outNormal = normalize(mat3(transform.model) * inNormal);
  outUV = inUV;
}
