/*
	Written by Brandon Salvietti
	28 December 2023

	model.h
	Description: This model class allows for multiple model objects to be created and loaded into an OpenGL scene. It also houses some important lighting functions as well.
*/

#pragma once
#include <vector>
#include <iostream>

#include "../ew/mesh.h"
#include "../ew/ewMath/ewMath.h"

#include "texture.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace vg3o {
	struct Light {
		ew::Vec3 position;
		ew::Vec3 color;
		float strength = 1.0f;

		ew::Vec3 diffuse = ew::Vec3(1.f, 1.f, 1.f);
		ew::Vec3 specular = ew::Vec3(1.f, 1.f, 1.f);

		float radius = 5.0f;
	};

	struct SpotLight {
		ew::Vec3 position;
		ew::Vec3 rotation;
		ew::Vec3 color;
		float strength = 1.0f;

		ew::Vec3 diffuse = ew::Vec3(1.f, 1.f, 1.f);
		ew::Vec3 specular = ew::Vec3(1.f, 1.f, 1.f);

		float range = 5.0f;

		float cutoff = 20.0f;
		float outerCutoff = 23.5f;
	};

	struct DirectionalLight {
		ew::Vec3 direction = ew::Vec3(0.f, 0.f, 0.f);
		float strength = 1.0f;

		ew::Vec3 ambient;
		ew::Vec3 diffuse;
		ew::Vec3 specular;
	};

	inline ew::Vec3 getForwardVector(ew::Vec3 rotation) {
		ew::Vec3 radianRot = rotation * ew::DEG2RAD;
		return ew::Vec3(
			cos(radianRot.y) * cos(radianRot.x),
			sin(radianRot.x),
			sin(radianRot.y) * cos(radianRot.x)
		);
	};
	/// <summary>
	/// Generates color values based on the temperature passed.
	/// </summary>
	/// <param name="kelvin">The temperature to get the color from.</param>
	/// <returns>Returns a vec3 color.</returns>
	inline ew::Vec3 GetTemperatureColor(int kelvin) {

		kelvin /= 100;

		float r, g, b;

		// red
		if (kelvin <= 66) {
			r = 255.f;
		}
		else {
			r = 329.698727446f * pow((float)kelvin - 60.f, -0.1332047592f);
		}
		r = ew::Clamp((r / 255.f), 0.0f, 1.0f);
		// green
		if (kelvin <= 66) {
			g = 99.4708025861f * log((float)kelvin) - 161.1195681661f;
		}
		else {
			g = 288.1221695283f * pow((float)kelvin - 60.f, -0.0755148492f);
		}
		g = ew::Clamp((g / 255.f), 0.0f, 1.0f);
		// blue
		if (kelvin >= 66) {
			b = 255.f;
		}
		else if (kelvin <= 19) {
			b = 0.f;
		}
		else {
			b = 138.5177312231f * log((float)kelvin - 10.f) - 305.0447927307f;
		}
		b = ew::Clamp((b / 255.f), 0.0f, 1.0f);

		return ew::Vec3(r, g, b);
	}

	class Model {
	public:
		Model() {};
		Model(const std::string& path);
		void draw(ew::Shader& shader) {
			for (unsigned int i = 0; i < meshes.size(); i++)
				meshes[i].drawLoadTex(shader);
		};

		std::vector<ew::Texture> loaded_textures;
	private:
		std::vector<ew::Mesh> meshes;

		std::string directory;

		void loadModel(const std::string& path);
		void processAssimpNode(aiNode* node, const aiScene* scene);
		ew::Mesh processAssimpMesh(aiMesh* mesh, const aiScene* scene);

		std::vector<ew::Texture> loadMaterialTextures(aiMaterial* material, aiTextureType type, std::string typeName);
	};
}