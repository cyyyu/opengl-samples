#version 410 core
out vec4 FragColor;

in VS_OUT {
  vec3 FragPos;
  vec3 Normal;
  vec4 FragPosLightSpace;
} fs_in;

uniform vec3 viewPos;

const vec3 lightPos = vec3(3.0);
const vec3 lightColor = vec3(1.0); // white
const vec3 objColor = vec3(1.0, 1.0, 1.0); // white

uniform sampler2D shadowMap;

float shadowCalculation(vec4 fragPosLightSpace)
{
  // perform perspective divide
  vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
  // transform to [0,1] range
  projCoords = projCoords * 0.5 + 0.5;
  // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
  float closestDepth = texture(shadowMap, projCoords.xy).r;
  // get depth of current fragment from light's perspective
  float currentDepth = projCoords.z;
  // check whether current frag pos is in shadow
  float shadow = currentDepth > closestDepth  ? 1.0 : 0.0;
  return shadow;
}

void main() {
  // ambient
  float ambientStrength = 0.1;
  vec3 ambient = ambientStrength * lightColor;

  // diffuse
  vec3 norm = normalize(fs_in.Normal);
  vec3 lightDir = normalize(lightPos - fs_in.FragPos);
  float diff = max(dot(norm, lightDir), 0.0);
  vec3 diffuse = diff * lightColor;

  // specular
  float specularStrength = 0.5;
  vec3 viewDir = normalize(viewPos - fs_in.FragPos);
  vec3 reflectDir = reflect(-lightDir, norm);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
  vec3 specular = specularStrength * spec * lightColor;

  // shadow
  float shadow = shadowCalculation(fs_in.FragPosLightSpace);
  vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * objColor;

  vec3 result = (ambient + (1.0 - shadow) * (diffuse + specular)) * objColor;

  FragColor = vec4(result, 1.0);
}


