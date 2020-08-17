#version 330 core
uniform sampler2D ourTexture;
out vec4 FragColor;
in vec3 Normal;
in vec2 TexCoord;
void main() {
	FragColor = vec4(1.0f);
}