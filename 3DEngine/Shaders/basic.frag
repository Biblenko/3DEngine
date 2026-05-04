#version 460
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
    sampler2D diffuseMap;
}; 
uniform Material material;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;

void main()
{
    // Vectors directions
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    vec3 viewDir = normalize(viewPos - FragPos);


    vec3 texColor = texture(material.diffuseMap, TexCoords).rgb;
    
    vec3 ambient = material.ambient * texColor * lightColor;

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * (material.diffuse * texColor);
    

    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(norm, halfwayDir), 0.0), material.shininess);
    vec3 specular = spec * material.specular;  
        

    vec3 result = ambient + (diffuse + specular) * lightColor;
    FragColor = vec4(result, 1.0);
}