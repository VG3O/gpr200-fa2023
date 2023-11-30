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

	struct Material {
		float ambientK;
		float diffuseK;
		float specularK;
		float shininess;

		ew::Vec3 ambientCol;
		ew::Vec3 diffuseCol;
		ew::Vec3 specularCol;

		ew::Texture diffuseMap;
		ew::Texture normalMap;
		ew::Texture specularMap;
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
		std::vector<Material> materials;

		std::string directory;

		void loadModel(const std::string& path);
		void processAssimpNode(aiNode* node, const aiScene* scene);
		ew::Mesh processAssimpMesh(aiMesh* mesh, const aiScene* scene);
		
		std::vector<ew::Texture> loadMaterialTextures(aiMaterial* material, aiTextureType type, std::string typeName);
	};
}