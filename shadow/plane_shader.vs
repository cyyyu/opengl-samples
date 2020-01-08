#version 410 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 normal;

out VS_OUT {
  vec3 FragPos;
  vec3 Normal;
  vec4 FragPosLightSpace;
} vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;

void main()
{
  vs_out.FragPos = vec3(model * vec4(aPos, 1.0));
  vs_out.Normal = transpose(inverse(mat3(model))) * normal;
  vs_out.FragPosLightSpace = lightSpaceMatrix * vec4(vs_out.FragPos, 1.0);

  mat4 mvp = projection * view * model;
  gl_Position = mvp * vec4(aPos, 1.0);
}

