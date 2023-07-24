#version 430 core

layout (local_size_x = 32, local_size_y = 32, local_size_z = 1) in;
layout (rgba32f) uniform image2D imgOutput;

void main() 
{
  vec4 value;

  ivec2 texelCoord = ivec2 (gl_GlobalInvocationID.xy);
  
  value.r = float (texelCoord.x) / (gl_NumWorkGroups.x * gl_WorkGroupSize.x);
  value.g = float (texelCoord.y) / (gl_NumWorkGroups.y * gl_WorkGroupSize.y);
  value.b = 1.0;
  value.a = 1.0;

  imageStore (imgOutput, texelCoord, value);
}
