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
    float specularStrength;
};
uniform DirLight dirLight;

// Old code
// uniform vec3 lightDir;
// uniform vec3 lightColor;

// Old code: components
//vec3 ambient;
// float ambientStrength = 0.2f;
// vec3 diffuse;
// vec3 specular;
// float specularStrength = 0.5f;

// textures
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;

const int material_shininess = 32;


// OLd code
// void computeDirLight()
// {
//     //compute eye space coordinates
//     vec4 fPosEye = view * model * vec4(fPosition, 1.0f);
//     vec3 normalEye = normalize(normalMatrix * fNormal);

//     //normalize light direction
//     vec3 lightDirN = vec3(normalize(view * vec4(lightDir, 0.0f)));

//     //compute view direction (in eye coordinates, the viewer is situated at the origin
//     vec3 viewDir = normalize(- fPosEye.xyz);

//     //compute ambient light
//     ambient = ambientStrength * lightColor;

//     //compute diffuse light
//     diffuse = max(dot(normalEye, lightDirN), 0.0f) * lightColor;
   
//     //compute specular light
//     vec3 reflectDir = reflect(-lightDirN, normalEye);
//     float specCoeff = pow(max(dot(viewDir, reflectDir), 0.0f), 32);
//     specular = specularStrength * specCoeff * lightColor;
// }

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material_shininess);
    // combine results
    vec3 ambient  = light.color  * vec3(texture(diffuseTexture, fTexCoords)) * light.ambientStrength;
    vec3 diffuse  = light.color  * diff * vec3(texture(diffuseTexture, fTexCoords));
    vec3 specular = light.color * spec * vec3(texture(diffuseTexture, fTexCoords)) * light.specularStrength;
    return (ambient + diffuse + specular);
}  

void main() 
{
    // initial code
    //computeDirLight();
    //compute final vertex color
    //vec3 color = min((ambient + diffuse) * texture(diffuseTexture, fTexCoords).rgb + specular * texture(specularTexture, fTexCoords).rgb, 1.0f);

    // new code
    // properties
    //compute eye space coordinates
    vec4 fPosEye = view * model * vec4(fPosition, 1.0f);
    vec3 normalEye = normalize(normalMatrix * fNormal);
    //normalize light direction
    vec3 lightDirN = vec3(normalize(view * vec4(dirLight.direction, 0.0f)));
    //compute view direction (in eye coordinates, the viewer is situated at the origin
    vec3 viewDir = normalize(- fPosEye.xyz);

    // phase 1: Directional lighting
    vec3 result = CalcDirLight(dirLight, normalEye, viewDir);
    // phase 2: Point lights
    // for(int i = 0; i < NR_POINT_LIGHTS; i++)
    //     result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);    
    // phase 3: Spot light
    //result += CalcSpotLight(spotLight, norm, FragPos, viewDir);    

    fColor = vec4(result, 1.0f);
}
