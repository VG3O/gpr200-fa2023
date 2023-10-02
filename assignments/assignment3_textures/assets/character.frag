#version 450
out vec4 FragColor;
in vec2 UV;

uniform sampler2D _CharacterTexture;
uniform float _Time;

void main(){
	vec4 col = texture(_CharacterTexture, UV);
	col.a *= abs(tan(_Time));
	FragColor = col;

}