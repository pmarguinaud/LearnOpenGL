#version 430 core

layout (local_size_x = 32, local_size_y = 32, local_size_z = 1) in;
layout (rgba32f) uniform image2D imgOutput;

void main() 
{
  vec4 value;

  ivec2 texelCoord = ivec2 (gl_GlobalInvocationID.xy);
  
  value = imageLoad (imgOutput, texelCoord);

  value.a = value.a * 0.999;

  imageStore (imgOutput, texelCoord, value);
}
