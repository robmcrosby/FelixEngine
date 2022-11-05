#version 450

layout(binding = 0) buffer In {
  mat4 models[];
};

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 fragColor;

void main() {
  vec4 pos = models[gl_InstanceIndex] * vec4(inPosition, 0.0, 1.0);
  gl_Position = vec4(pos.xy, 0.0, 1.0);
  fragColor = inColor;
}
