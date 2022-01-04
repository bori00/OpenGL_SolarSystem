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

struct PointLight {    
    vec3 position; //provided in world coordinates --> needs to be multiplied by the view
    
    float constant;
    float linear;
    float quadratic;  

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};  
#define NR_POINT_LIGHTS 1  
uniform PointLight pointLights[NR_POINT_LIGHTS];

// textures
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
uniform sampler2D nightDiffuseTexture;

const int material_shininess = 32;

float sun_diff; //--> used for all lighting types

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = vec3(normalize(view * vec4(light.direction, 0.0f)));
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material_shininess);
    // combine results
    vec3 ambient, diffuse, specular;
    if (sun_diff > 0) {
        ambient  = light.color  * vec3(texture(diffuseTexture, fTexCoords)) * light.ambientStrength;
        diffuse  = light.color  * diff * vec3(texture(diffuseTexture, fTexCoords)) * light.diffuseStrength;
        specular = light.color * spec * vec3(texture(specularTexture, fTexCoords)) * light.specularStrength;
    } else {
        ambient  = light.color  * vec3(texture(nightDiffuseTexture, fTexCoords)) * light.ambientStrength;
        diffuse  = light.color  * diff * vec3(texture(nightDiffuseTexture, fTexCoords)) * light.diffuseStrength;
        specular = light.color * spec * vec3(texture(nightDiffuseTexture, fTexCoords)) * light.specularStrength;
    }
    return (ambient + diffuse + specular);
}  

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    // important: transform world coordinates of the light to eye space coordinates
    vec3 lightPos = (view * vec4(light.position, 1.0)).xyz; 

    vec3 lightDir = normalize(lightPos - fragPos);
    // diffuse shading
    sun_diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material_shininess);
    // attenuation
    float distance    = length(lightPos - fragPos);
    float attenuation =  1 / (light.constant + light.linear * distance + 
  			     light.quadratic * (distance * distance));    
    // combine results
    vec3 ambient, diffuse, specular;
    if (sun_diff > 0) {
        ambient = light.ambient  * vec3(texture(diffuseTexture, fTexCoords));
        diffuse = light.diffuse  * sun_diff * vec3(texture(diffuseTexture, fTexCoords));
        specular = light.specular * spec * vec3(texture(specularTexture, fTexCoords));
    } else {
        ambient = light.ambient  * vec3(texture(nightDiffuseTexture, fTexCoords));
        diffuse  = light.diffuse  * sun_diff * vec3(texture(nightDiffuseTexture, fTexCoords));
        specular = light.specular * spec * vec3(texture(nightDiffuseTexture, fTexCoords));
    }
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
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

    // phase 1: Point lights
    vec3 result = vec3(0, 0, 0);

    for(int i = 0; i < NR_POINT_LIGHTS; i++)
        result += CalcPointLight(pointLights[0], normalEye, fPosEye.xyz, viewDir);    

    // phase 2: Directional lighting
    result += CalcDirLight(dirLight, normalEye, viewDir);

    fColor = vec4(result, 1.0f);
}
