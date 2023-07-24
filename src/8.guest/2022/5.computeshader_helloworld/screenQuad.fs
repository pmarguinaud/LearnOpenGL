#version 430 core

out vec4 color;
in vec2 texcoords;

uniform sampler2D tex;

void main()
{             
  color = texture (tex, texcoords);
}
