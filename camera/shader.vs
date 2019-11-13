#version 410

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 aTexCoord;

out vec2 texCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
  mat4 mvp = projection * view * model;
	gl_Position = mvp * vec4(pos, 1.0f);
  texCoords = aTexCoord;
}
