/*
	Written by Brandon Salvietti
	28 December 2023

	model.h
	Description: This model class allows for multiple model objects to be created and loaded into an OpenGL scene.
*/

#pragma once
#include <vector>
#include <iostream>

#include "../ew/mesh.h"

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
		ew::Vec3 specular = ew::Vec3(1.f,1.f,1.f);

		// these values determine attenuation
		// default values are for 13 distance
		float constant = 1.0f;
		float linear = 0.35f;
		float quadratic = 0.44f;
	};

	class Model {
	public:
		Model() {};
		Model(const std::string& path);
		void draw(ew::Shader &shader) {
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