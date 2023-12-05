/*
	Written by Brandon Salvietti
	28 December 2023
*/

#include "../ew/external/glad.h"
#include "../ew/ewMath/ewMath.h"
#include "model.h"


namespace vg3o {
	Model::Model(const std::string& path) {
		loadModel(path);
	}

	void Model::loadModel(const std::string& path) {
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
			std::cout << "ASSIMP::ERROR -> " << importer.GetErrorString() << std::endl;
			return;
		}
		directory = path.substr(0, path.find_last_of('/'));

		processAssimpNode(scene->mRootNode, scene);
	}

	void Model::processAssimpNode(aiNode* node, const aiScene* scene) {
		for (unsigned int i = 0; i < node->mNumMeshes; i++) 
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			meshes.push_back(processAssimpMesh(mesh, scene));
		}
		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			processAssimpNode(node->mChildren[i], scene);
		}
	}

	ew::Mesh Model::processAssimpMesh(aiMesh* mesh, const aiScene* scene) {
		ew::MeshData meshData;

		// verticies
		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			ew::Vertex vertex;
			
			// position
			vertex.pos = ew::Vec3(
				mesh->mVertices[i].x,
				mesh->mVertices[i].y,
				mesh->mVertices[i].z
			);
			// normals
			if (mesh->HasNormals()) {
				vertex.normal = ew::Vec3(
					mesh->mNormals[i].x,
					mesh->mNormals[i].y,
					mesh->mNormals[i].z
				);
			}

			// uv
			if (mesh->mTextureCoords[0])
			{
				vertex.uv = ew::Vec2(
					mesh->mTextureCoords[0][i].x,
					mesh->mTextureCoords[0][i].y
				);

				vertex.tangent = ew::Vec3(
					mesh->mTangents[i].x,
					mesh->mTangents[i].y,
					mesh->mTangents[i].z
				);
				vertex.bitangents = ew::Vec3(
					mesh->mBitangents[i].x,
					mesh->mBitangents[i].y,
					mesh->mBitangents[i].z
				);
			}
			else
			{
				vertex.uv = ew::Vec2(0.0f, 0.0f);
			}

			meshData.vertices.push_back(vertex);
		}
		// indices
		for (unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; j++)
			{
				meshData.indices.push_back(face.mIndices[j]);
			}
		}

		// material
		if (mesh->mMaterialIndex >= 0)
		{
			aiMaterial* meshMaterial = scene->mMaterials[mesh->mMaterialIndex];
			Material newMat;

			std::vector<ew::Texture> diffuseMaps = loadMaterialTextures(meshMaterial, aiTextureType_DIFFUSE, "texture_diffuse");
			meshData.textures.insert(meshData.textures.end(), diffuseMaps.begin(), diffuseMaps.end());
			if (diffuseMaps.size() > 0) { newMat.hasDiffuse = true; }

			std::vector<ew::Texture> specularMaps = loadMaterialTextures(meshMaterial, aiTextureType_SPECULAR, "texture_specular");
			meshData.textures.insert(meshData.textures.end(), specularMaps.begin(), specularMaps.end());
			if (specularMaps.size() > 0) { newMat.hasSpecular = true; }

			std::vector<ew::Texture> normalMaps = loadMaterialTextures(meshMaterial, aiTextureType_HEIGHT, "texture_normal");
			meshData.textures.insert(meshData.textures.end(), normalMaps.begin(), normalMaps.end());
			if (normalMaps.size() > 0) { newMat.hasBump = true; }

			aiColor3D diffuseColor(0.f, 0.f, 0.f);
			aiColor3D ambientColor(0.f, 0.f, 0.f);
			aiColor3D specularColor(0.f, 0.f, 0.f);
			aiString name;
			float shininess = 0.f;
			float opacity = 1.f;

			meshMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor);
			meshMaterial->Get(AI_MATKEY_COLOR_AMBIENT, ambientColor);
			meshMaterial->Get(AI_MATKEY_COLOR_SPECULAR, specularColor);
			meshMaterial->Get(AI_MATKEY_SHININESS, shininess);
			meshMaterial->Get(AI_MATKEY_NAME, name);
			meshMaterial->Get(AI_MATKEY_OPACITY, opacity);

			newMat.diffuse = ew::Vec3(diffuseColor.r, diffuseColor.g, diffuseColor.b);
			newMat.ambient = ew::Vec3(ambientColor.r, ambientColor.g, ambientColor.b);
			newMat.specular = ew::Vec3(specularColor.r, specularColor.g, specularColor.b);
			newMat.shininess = shininess;
			newMat.opacity = opacity;
			/*
			std::cout << "Material " << name.C_Str() << std::endl; 
			std::cout << "Texture Count: " << diffuseMaps.size() + specularMaps.size() + normalMaps.size() << std::endl;
			std::cout << "Diffuse: (" << newMat.diffuse.x << ", " << newMat.diffuse.y << ", " << newMat.diffuse.z << ")\n";
			std::cout << "Specular: (" << newMat.specular.x << ", " << newMat.specular.y << ", " << newMat.specular.z << ")\n";
			std::cout << "Specular Shininess: " << newMat.shininess << std::endl << std::endl;
			*/
			meshData.material = newMat;
		}

		return ew::Mesh(meshData);
	}

	std::vector<ew::Texture> Model::loadMaterialTextures(aiMaterial* material, aiTextureType type, std::string typeName) {
		std::vector<ew::Texture> textures;
		for (unsigned int i = 0; i < material->GetTextureCount(type); i++)
		{
			aiString string;
			material->GetTexture(type, i, &string);
			bool skip = false;
			for (unsigned int j = 0; j < loaded_textures.size(); j++)
			{
				if (std::strcmp(loaded_textures[j].path.data(), string.C_Str()) == 0)
				{
					textures.push_back(loaded_textures[j]);
					skip = true;
					break;
				}
			}
			if (skip) { continue; }

			ew::Texture texture;
			texture.id = vg3o::loadTexture(string.C_Str(), directory);
			texture.type = typeName;
			texture.path = string.C_Str();
			textures.push_back(texture);
		}
		return textures;
	}
}