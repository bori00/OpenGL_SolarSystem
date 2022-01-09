#version 410 core

in vec3 fPosition;
in vec3 fNormal;
in vec2 fTexCoords;

out vec4 fColor;

//matrices
uniform mat4 model;
uniform mat4 view;
uniform mat3 normalMatrix;

//lighting
struct DirLight {
    vec3 direction;
  
    vec3 color;

    float ambientStrength;
    float diffuseStrength;
    float specularStrength;
};
uniform DirLight dirLight;

// textures
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;

const int material_shininess = 32;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = vec3(normalize(view * vec4(light.direction, 0.0f)));
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material_shininess);
    // combine results
    vec3 ambient  = light.color  * vec3(texture(diffuseTexture, fTexCoords)) * light.ambientStrength;
    vec3 diffuse  = light.color  * diff * vec3(texture(diffuseTexture, fTexCoords)) * light.diffuseStrength;
    vec3 specular = light.color * spec * vec3(texture(specularTexture, fTexCoords)) * light.specularStrength;
    return (ambient + diffuse + specular);
}  

void main() 
{
    // properties
    //compute eye space coordinates
    vec4 fPosEye = view * model * vec4(fPosition, 1.0f);
    vec3 normalEye = normalize(normalMatrix * fNormal);
    //compute view direction (in eye coordinates, the viewer is situated at the origin
    vec3 viewDir = normalize(- fPosEye.xyz);

    // phase 1: Directional lighting
    vec3 result = CalcDirLight(dirLight, normalEye, viewDir);

    fColor = vec4(min(result, 1.0), 1.0f);
}
