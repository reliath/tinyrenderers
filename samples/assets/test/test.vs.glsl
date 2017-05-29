#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (std140, set=0, binding=0) uniform Buffer0T {
  uniform mat4 mvp;
} Buffer0;

void main()
{
  gl_Position = Buffer0.mvp * vec4(0);
}
