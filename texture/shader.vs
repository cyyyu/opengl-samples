#version 410

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 aTexCoords;

out vec2 texCoords;

void main() {
  texCoords = aTexCoords;
  gl_Position = vec4(pos, 1.0);
}
