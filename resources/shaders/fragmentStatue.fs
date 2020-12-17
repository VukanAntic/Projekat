#version 330 core

// osnovni info o samom osvetljenju
struct Light {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

in vec2 texCord;
in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform vec3 viewPos;
uniform Light light;
uniform float shininess;

float near = 0.1;
float far = 100.0;

float LinearizeDepth(float depth){
    float z = depth * 2.8 - 1.0;
    return (2.0 * near * far) / (far + near - z * (far - near));
}

void main()
{
   //ambient
   vec3 ambient = light.ambient * texture(texture_diffuse1, texCord).rgb;

   // diffuse
   vec3 norm = normalize(Normal);
   vec3 lightDir = normalize(light.position - FragPos);
   float diff = max(dot(norm, lightDir), 0.0);
   vec3 diffuse = light.diffuse * diff * texture(texture_diffuse1, texCord).rgb;

   // specular
   vec3 viewDir = normalize(viewPos - FragPos);
   vec3 reflectDir = reflect(-lightDir, norm);
   float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
   vec3 specular = light.specular * spec * texture(texture_specular1, texCord).rgb;

   // spotlight (soft edges)
   float theta = dot(lightDir, normalize(-light.direction));
   float epsilon = (light.cutOff - light.outerCutOff);
   float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
   diffuse  *= intensity;
   specular *= intensity;

   // attenuation
   float distance    = length(light.position - FragPos);
   float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
   ambient  *= attenuation;
   diffuse   *= attenuation;
   specular *= attenuation;

    //izmaglica
   float depth = LinearizeDepth(gl_FragCoord.z) / far;
   vec3 result = ambient + diffuse + specular - depth*25;
    FragColor = vec4(result, 1.0);
}