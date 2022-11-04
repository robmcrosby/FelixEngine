#version 450

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 fragColor;

mat3 models[4] = mat3[](
  mat3(
    0.5, 0.0, 0.0,
    0.0, 0.5, 0.0,
    0.5, 0.5, 1.0
  ),
  mat3(
    0.5, 0.0, 0.0,
    0.0, 0.5, 0.0,
   -0.5, 0.5, 1.0
  ),
  mat3(
    0.5, 0.0, 0.0,
    0.0, 0.5, 0.0,
    0.5,-0.5, 1.0
  ),
  mat3(
    0.5, 0.0, 0.0,
    0.0, 0.5, 0.0,
   -0.5,-0.5, 1.0
  )
);

void main() {
  vec3 pos = models[gl_InstanceIndex] * vec3(inPosition, 1.0);
  gl_Position = vec4(pos.xy, 0.0, 1.0);
  fragColor = inColor;
}
