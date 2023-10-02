#version 450
out vec4 FragColor;
in vec2 UV;

uniform sampler2D _Texture;
uniform sampler2D _WaveTexture;
uniform sampler2D _NoiseTexture;
uniform float _Time;

void main(){
	
	float noise = texture(_NoiseTexture,UV).r;
	vec2 uv = UV + noise * (sin(_Time)*0.05);
	vec4 texture1 = texture(_Texture,uv);
	vec4 texture2 = texture(_WaveTexture,uv);

	vec3 color = mix(texture1.rgb, texture2.rgb, texture2.a*0.25);

	FragColor = vec4(color, 1.0);

}