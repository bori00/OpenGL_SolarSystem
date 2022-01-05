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

// for shadows
uniform samplerCube depthMap;
uniform float far_plane;
float shadow = 0.0;

const int material_shininess = 32;

// array of offset direction for sampling
vec3 gridSamplingDisk[20] = vec3[]
(
   vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1), 
   vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
   vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
   vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);

void computePointShadow() {
    // all coordinates are in world space
    vec3 fragPos = (model * vec4(fPosition, 1.0)).xyz;
	vec3 fragToLight = fragPos - sunLight.position;
    
    float currentDepth = length(fragToLight);
    // test for shadows

    float bias = 10;
    int samples = 20;
    vec3 viewPos = (inverse(view) * vec4(0, 0, 0, 1)).xyz;
    float viewDistance = length(viewPos - fragPos); // (0, 0, 0) stands for the viewpos (in eye space)
    float diskRadius = (1.0 + (viewDistance / far_plane)) / 25.0;

    for(int i = 0; i < samples; ++i)
    {
        float closestDepth = texture(depthMap, fragToLight + gridSamplingDisk[i] * diskRadius).r;
        closestDepth *= far_plane;   // undo mapping [0;1]
        if(currentDepth - bias > closestDepth) {
             shadow += 1.0;
        }
    }
    shadow /= float(samples);

    /*float closestDepth = texture(depthMap, fragToLight).r * far_plane;

    if (closestDepth < currentDepth - bias) {
        shadow = 1.0;
    } else {
        shadow = 0.0;
    }*/
}

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

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    // important: transform world coordinates of the light to eye space coordinates
    vec3 lightPos = (view * vec4(light.position, 1.0)).xyz; 

    vec3 lightDir = normalize(lightPos - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material_shininess);
    // attenuation
    float distance    = length(lightPos - fragPos);
    float attenuation =  1 / (light.constant + light.linear * distance + 
  			     light.quadratic * (distance * distance));    
    // combine results
    vec3 ambient  = light.ambient  * vec3(texture(diffuseTexture, fTexCoords));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(diffuseTexture, fTexCoords));
    vec3 specular = light.specular * spec * vec3(texture(specularTexture, fTexCoords));
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    return (ambient + (1.0 - shadow) * diffuse + (1.0 - shadow) * specular);
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
    // phase 2: Point lights
    computePointShadow();
    result += CalcPointLight(sunLight, normalEye, fPosEye.xyz, viewDir);    

    fColor = vec4(result, 1.0f);

    // debug
    /* vec3 worldPosition = (model * vec4(fPosition, 1.0)).xyz;
    vec3 fragToLight = worldPosition; // - sunLight.position;
    
    float currentDepth = length(fragToLight) / far_plane;
    // test for shadows

    float shadow = 0.0;
    float bias = 0; //.15;
    float closestDepth = texture(depthMap, fragToLight).r;
    fColor = vec4(vec3(closestDepth), 1.0); */
}
