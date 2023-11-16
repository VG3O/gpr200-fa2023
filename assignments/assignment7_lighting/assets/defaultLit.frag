#version 450
out vec4 FragColor;

in Surface{
	vec2 UV;
	vec3 WorldPosition;
	vec3 WorldNormal;
}fs_in;

uniform sampler2D _Texture;

struct Light{ 
	vec3 position;
	vec3 color;
	float brightness;
};

#define MAX_LIGHTS 4
uniform Light _Light;

// light material uniforms

// camera uniforms
uniform vec3 _CameraPosition;

void main(){
	// start with ambient
	float ambientStrength = 0.1; // change this to use the material uniform
	vec3 ambient = ambientStrength * _Light.color;

	// diffuse lighting
	// get a vector that points to the light direction
	vec3 normal = normalize(fs_in.WorldNormal);
	vec3 omega = normalize(_Light.position - fs_in.WorldPosition);
	
	// calculate the light intensity (I0 is just _Light.color)
	vec3 diffuse = (max(dot(normal, omega),0)) * _Light.color;

	// specular lighting
	vec3 reflection = reflect(-omega, normal);
	vec3 camView = normalize(_CameraPosition -  fs_in.WorldPosition);
	float tempSpecularCo = 32; // change this to use the material uniform

	vec3 specular = pow(max(dot(reflection, camView), 0), tempSpecularCo) * _Light.color;

	// add all light calcs together
	vec3 OutColor = (ambient + diffuse + specular) * texture(_Texture,fs_in.UV).rgb;

	FragColor = vec4(OutColor, 1.0);
}