#version 450
layout (location=0) out vec4 FragColor;
layout (location=1) out vec4 BrightColor;

in Surface{
	vec2 UV;
	vec3 WorldPosition;
	vec3 WorldNormal;
	mat3 TBN;
}fs_in;

uniform sampler2D _Texture;


struct Light{ 
	vec3 position;
	vec3 color;
	float strength;

	vec3 diffuse;
	vec3 specular;
	
	float radius;
};

struct SpotLight{
	vec3 position;
	vec3 direction;
	vec3 color;
	float strength;

	vec3 diffuse;
	vec3 specular;

	float range;

	float cutoff;
	float outerCutoff;
};

struct DirectionalLight {
	vec3 direction;
	float strength;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

#define MAX_TEXTURES 10

struct Material{
	vec3 ambientColor;
	vec3 diffuseColor;
	vec3 specularColor;
	float shininess;
	float opacity;

	bool hasDiffuse;
	bool hasSpecular;
	bool hasBump;

};

#define MAX_LIGHTS 6
uniform Light _PointLights[MAX_LIGHTS];
uniform SpotLight _SpotLights[MAX_LIGHTS];
uniform SpotLight _Headlights[10];
uniform DirectionalLight _Sun;

// active variables
uniform bool _HeadlightsActive;

// material uniform
uniform Material _Material;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_normal1;
uniform samplerCube _Skybox;

// camera uniforms
uniform vec3 _CameraPosition;

// current light count
uniform int _PointLightAmount;
uniform int _SpotLightAmount;

vec3 MakePointLight(Light light, Material material, vec3 camView, vec3 normal, vec3 pos)
{
	// diffuse lighting
	// get a vector that points to the light direction
	vec3 newNormal = normalize(normal);

	vec3 lightDirection = light.position - pos;
	float dist = length(lightDirection);
	lightDirection = normalize(lightDirection);

	if (dist < 1) {
		dist = 1;
	}

	vec3 diffuseColor = material.diffuseColor;
	vec3 specularColor = material.specularColor;

	if (material.hasDiffuse) {
		diffuseColor *= vec3(texture(texture_diffuse1, fs_in.UV));
	} 
	/*
	if (material.hasSpecular) {
		specularColor *= vec3(texture(material.texture_specular1, fs_in.UV));
	}*/

	float clampedDistance = clamp(dist, 0.0, light.radius);
	float attenuation = pow(1.0 - (clampedDistance / light.radius), 2.0);

	// calculate the light intensity (I0 is just light.color)
	float diffuseFloat = (max(dot(newNormal, lightDirection),0.0)) * light.strength;

	vec3 diffuse = diffuseFloat * diffuseColor;

	// specular lighting
	vec3 halfway = normalize(lightDirection - camView);

	float specularFloat = pow(max(dot(newNormal, halfway), 0.0), material.shininess*2.0) * light.strength;
	
	vec3 specular = specularFloat * specularColor;

	diffuse *= light.color;
	specular *= light.color;

	diffuse *= attenuation;
	specular *= attenuation;

	return diffuse + specular;
}

vec3 MakeSpotLight(SpotLight light, Material material, vec3 camView, vec3 normal, vec3 position) {
	vec3 lightDirection = normalize(light.position - position);

	float dist = length(light.position-position);

	float theta = dot(lightDirection, normalize(-light.direction));
	float epsilon = light.cutoff - light.outerCutoff;
	float intensity = smoothstep(0.0, 1.0, (theta - light.outerCutoff) / epsilon);

	float clampedDistance = clamp(dist, 0.0, light.range);
	float attenuation = pow(1.0 - (clampedDistance / light.range), 2.0);

	vec3 diffuseColor = material.diffuseColor;
	vec3 specularColor = material.specularColor;

	if (material.hasDiffuse) {
		diffuseColor *= vec3(texture(texture_diffuse1, fs_in.UV));
	} 
	vec3 ambient = 0.40 * diffuseColor;
	vec3 specular;
	vec3 diffuse;

	vec3 newNormal = normalize(normal);
	float diffuseFloat = (max(dot(newNormal, lightDirection),0.0)) * light.strength;

	// specular lighting
	vec3 halfway = normalize(lightDirection - camView);

	float specularFloat = pow(max(dot(newNormal, halfway), 0.0), material.shininess*2.0) * light.strength;

	diffuse = diffuseFloat * diffuseColor;
	specular = specularFloat * specularColor;

	diffuse *= light.color;
	specular *= light.color;

	diffuse *= intensity * attenuation;
	specular *= intensity * attenuation;

	return diffuse + specular;
}

vec3 MakeDirectionalLight(DirectionalLight light, Material material, vec3 camView, vec3 normal, vec3 position) {	
	vec3 lightDirection = normalize(-light.direction);
	vec3 newNormal = normalize(normal);

	float diffuseFloat = (max(dot(newNormal, lightDirection),0.0)) * light.strength;

	vec3 diffuse = diffuseFloat * light.diffuse;

	// specular lighting
	vec3 halfway = normalize(lightDirection - camView);

	float specularFloat = pow(max(dot(newNormal, halfway), 0.0), material.shininess*2.0) * light.strength;
	
	vec3 specular = specularFloat * light.specular;

	vec3 ambient = 0.4 * light.ambient;

	return ambient + diffuse + specular;
}

void main(){
	vec3 normal = fs_in.WorldNormal;
	if (_Material.hasBump) {
		normal = texture(texture_normal1, fs_in.UV).rgb;
		normal = normalize(normal * 2.0 - 1.0);
		normal = normalize(fs_in.TBN * normal);
	}
	vec3 result;
	vec3 camera = normalize(fs_in.WorldPosition - _CameraPosition);
	result += MakeDirectionalLight(_Sun, _Material, camera, normal, fs_in.WorldPosition);
	for(int i = 0; i < _PointLightAmount; i++) {
		result += MakePointLight(_PointLights[i], _Material, camera, normal, fs_in.WorldPosition);
	}
	for(int i = 0; i < _SpotLightAmount; i++) {
		result += MakeSpotLight(_SpotLights[i], _Material, camera, normal, fs_in.WorldPosition);
	}
	if (_HeadlightsActive) {
		for(int i = 0; i < 10; i++) {
			result += MakeSpotLight(_Headlights[i], _Material, camera, normal, fs_in.WorldPosition);
		}
	}

	vec4 color;
	vec3 color3;
	float alpha;

	if (_Material.hasDiffuse) {
		color = texture(texture_diffuse1,fs_in.UV);
		color3 = color.rgb;
		alpha = color.a;
	} else {
		color3 = _Material.diffuseColor;
		alpha = _Material.opacity;
	}
	if (alpha < 0.05)
		discard;

	vec3 finalFragColor = result * color3; 
	vec3 R = reflect(camera, normalize(fs_in.WorldNormal));
	
	float reflectivity = _Material.shininess/1500.0;

	FragColor = vec4(finalFragColor+(texture(_Skybox, R).rgb*reflectivity), alpha);

	float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
	
	if (brightness > 1.0)
		BrightColor = vec4(FragColor.rgb, 1.0);
	else
		BrightColor = vec4(0.0,0.0,0.0,1.0);
}