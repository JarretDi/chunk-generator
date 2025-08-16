#version 330 core

struct Material {
    sampler2D diffuse;
	sampler2D specular;
    float shininess;
}; 

struct Light {
    vec3 direction;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Material material;
uniform Light light;  

in vec3 fragPos;
in vec3 normal;
in vec2 texCoord;

out vec4 colour;

uniform sampler2D texture1;

void main() {
	vec3 ambient = light.ambient * texture(material.diffuse, texCoord).rgb;

	vec3 norm = normalize(normal);
	vec3 lightDir = normalize(-light.direction);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = light.diffuse * diff * texture(material.diffuse, texCoord).rgb;

	vec3 viewDir = normalize(-fragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = light.specular * spec * texture(material.specular, texCoord).rgb;  

	vec3 result = (ambient + diffuse + specular);
	colour = vec4(result, 1.0);
}