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
uniform PointLight sunLight;

// textures
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
uniform sampler2D nightDiffuseTexture;

const int material_shininess = 32;

vec3 final_texture_color;

float sun_diff_smoothed_factor;

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

    ambient  = light.color  * final_texture_color * light.ambientStrength;
    diffuse  = light.color  * diff * final_texture_color * light.diffuseStrength;
    specular = light.color * spec * final_texture_color * light.specularStrength;
    
    return (ambient + diffuse + specular);
}  

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    // important: transform world coordinates of the light to eye space coordinates
    vec3 lightPos = (view * vec4(light.position, 1.0)).xyz; 

    vec3 lightDir = normalize(lightPos - fragPos);
    // diffuse shading
    float sun_diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material_shininess);
    // attenuation
    float distance    = length(lightPos - fragPos);
    float attenuation =  1 / (light.constant + light.linear * distance + 
  			     light.quadratic * (distance * distance));    
    // combine results
    vec3 ambient, diffuse, specular;

    ambient = light.ambient  * final_texture_color;
    diffuse = light.diffuse  * sun_diff * final_texture_color;
    specular = light.specular * spec * final_texture_color;
   
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
} 

void CalcTexture(PointLight sun, vec3 normal, vec3 fragPos) {
    vec3 lightPos = (view * vec4(sun.position, 1.0)).xyz; 

    vec3 lightDir = normalize(lightPos - fragPos);

    float sun_diff_value = dot(normal, lightDir);

    sun_diff_smoothed_factor = smoothstep(-0.4, 0.4, sun_diff_value);

    final_texture_color = vec3(texture(diffuseTexture, fTexCoords)) * sun_diff_smoothed_factor + vec3(texture(nightDiffuseTexture, fTexCoords)) * (1 - sun_diff_smoothed_factor);
}

void main() 
{
    // properties
    //compute eye space coordinates
    vec4 fPosEye = view * model * vec4(fPosition, 1.0f);
    vec3 normalEye = normalize(normalMatrix * fNormal);
    //compute view direction (in eye coordinates, the viewer is situated at the origin
    vec3 viewDir = normalize(- fPosEye.xyz);

    CalcTexture(sunLight, normalEye, fPosEye.xyz);

    // phase 1: Directional lighting
    vec3 result = CalcDirLight(dirLight, normalEye, viewDir);
    // phase 2: Point lights
    result += CalcPointLight(sunLight, normalEye, fPosEye.xyz, viewDir); 

    fColor = vec4(result, 1.0f);
}