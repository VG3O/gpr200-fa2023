#version 450
layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 vUV;
layout(location = 3) in vec3 vTangent;

out Surface{
	vec2 UV;
	vec3 WorldPosition;
	vec3 WorldNormal;
	mat3 TBN;
}vs_out;
/*
struct Material {
	bool hasBump;
};

uniform Material _Material;
*/
uniform mat4 _Model;
uniform mat4 _ViewProjection;

void main(){
	vs_out.UV = vUV;
	vs_out.WorldPosition = vec3(_Model * vec4(vPos,1.0));
	vs_out.WorldNormal = transpose(inverse(mat3(_Model))) * vNormal;

	vec3 T = normalize(vec3(_Model * vec4(vTangent, 0.0)));
	vec3 N = normalize(vec3(_Model * vec4(vNormal, 0.0)));
	vec3 B = cross(N, T);
	mat3 TBN = mat3(T, B, N);

	vs_out.TBN = TBN;
	gl_Position = _ViewProjection * _Model * vec4(vPos,1.0);
}