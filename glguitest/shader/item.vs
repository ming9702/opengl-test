#version 330 core
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec2 TexCoord;
out vec3 FragPos;
out vec3 Normal;
out mat4 Model;

void main() {
	TexCoord = aTexCoord;
	Normal = mat3(transpose(inverse(model))) * aNormal;	
	gl_Position = projection * view * model * vec4(aPos,1.0);
	FragPos = vec3(model * vec4(aPos, 1.0));
	Model = model;
}