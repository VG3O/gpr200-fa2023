#version 450
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;

uniform vec3 _Color;

void main() {
	FragColor = vec4 (_Color,1.0);
	FragColor = texture(skybox, TexCoords);
}