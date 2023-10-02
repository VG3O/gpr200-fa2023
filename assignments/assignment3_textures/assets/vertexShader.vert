#version 450
layout(location = 0) in vec3 vPos;
layout(location = 1) in vec2 vUV;

uniform float _Time;

out vec2 UV;
void main(){
	vec2 uv = vUV * 2.0 - 1.0;
	UV = vec2((vUV.x+(_Time*0.5)),vUV.y-sin(_Time*0.5));
	gl_Position = vec4(vPos,1.0);
}