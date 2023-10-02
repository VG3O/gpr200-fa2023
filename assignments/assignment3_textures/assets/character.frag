#version 450
out vec4 FragColor;
in vec2 UV;

uniform sampler2D _Texture;
uniform sampler2D _Texture2;

void main(){
	
	vec4 texture1 = texture(_Texture,UV);
	vec4 texture2 = texture(_Texture2,UV);

	vec3 color = mix(texture1.rgb, texture2.rgb, texture2.a*0.25);

	FragColor = vec4(color, 1.0);

}