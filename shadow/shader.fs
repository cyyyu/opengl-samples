#version 410 core
out vec4 FragColor;

in VS_OUT {
  vec3 FragPos;
  vec3 Normal;
  vec4 FragPosLightSpace;
} fs_in;

uniform vec3 cameraPos;
uniform vec3 objColor;
uniform sampler2D shadowMap;
uniform vec3 lightPos;

const vec3 lightColor = vec3(1.0); // white

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
  // calculate bias (based on depth map resolution and slope)
  vec3 normal = normalize(fs_in.Normal);
  vec3 lightDir = normalize(lightPos - fs_in.FragPos);
  float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
  // check whether current frag pos is in shadow
  // float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
  // PCF
  float shadow = 0.0;
  vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
  for(int x = -1; x <= 1; ++x)
  {
    for(int y = -1; y <= 1; ++y)
    {
      float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
      shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;
    }
  }
  shadow /= 9.0;

  // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
  if(projCoords.z > 1.0)
    shadow = 0.0;
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
  vec3 viewDir = normalize(cameraPos - fs_in.FragPos);
  vec3 reflectDir = reflect(-lightDir, norm);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
  vec3 specular = specularStrength * spec * lightColor;

  // shadow
  float shadow = shadowCalculation(fs_in.FragPosLightSpace);
  vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * objColor;

  vec3 result = (ambient + (1.0 - shadow) * (diffuse + specular)) * objColor;

  FragColor = vec4(result, 1.0);
}

