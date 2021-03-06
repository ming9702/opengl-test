#version 330 core
struct Material {
    sampler2D ambient;
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoord;
in mat4 Model;

out vec4 FragColor;

uniform Material material;
uniform Light light;
uniform vec3 viewPos;

void main()
{   

    // 从法线贴图范围[0,1]获取法线
    // vec3 normal = texture(material.diffuse, TexCoord).rgb;
    // 将法线向量转换为范围[-1,1]
    // normal = normalize(normal * 2.0 - 1.0);   
    // 环境光
    vec3 ambient = light.ambient * vec3(texture(material.ambient, TexCoord));
    // 漫反射
    vec3 norm = normalize(Normal);
    // 光源位置与片段位置的向量差并转换为单位向量
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.ambient, TexCoord));
    // 镜面光
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0f);
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoord));
    //
    FragColor = vec4(ambient + diffuse + specular, 1.0);
}