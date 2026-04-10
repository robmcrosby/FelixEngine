#version 450

layout(set = 0, binding = 0) uniform TransformUniforms {
  mat4 model;
} transform;

layout(set = 1, binding = 0) uniform CameraUniforms {
  mat4 projection;
  mat4 view;
  vec3 position;
} camera;

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec2 inUV;

layout(location = 0) out vec2 fragUV;

void main() {
  vec4 pos = camera.projection * camera.view * transform.model * vec4(inPosition, 0.0, 1.0);
  gl_Position = vec4(pos.xyz, 1.0);
  fragUV = inUV;
}
