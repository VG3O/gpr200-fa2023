#version 450
layout(location = 0) in vec3 vPos;
layout(location = 1) in vec2 vUV;

uniform float _Time;

uniform float _MaxVerticalSize;
uniform float _MaxHorizontalSize;
uniform float _VerticalScaleSpeed;
uniform float _HoriztonalScaleSpeed;

out vec2 UV;
void main(){
	vec2 uv = (vUV*2.0-1.0)+0.5;
	vec3 position = vec3((vPos.x*1.2)*abs(sin(_Time*1.5)), (vPos.y*1.3)*abs(sin(_Time*1.5)), 0.0);
	uv = vec2(uv.x-(0.5*sin(_Time*1.5)),uv.y-(0.5*sin(_Time)));
	UV = uv;
	gl_Position = vec4(position,1.0);
}