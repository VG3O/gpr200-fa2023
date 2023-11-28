/*
	Written by Brandon Salvietti
	28 December 2023

	model.h
	Description: This model class allows for multiple model objects to be created and loaded into an OpenGL scene.
*/

#pragma once
#include <vector>

#include "../ew/mesh.h"
#include "../ew/texture.h"

#include "../assimp/Importer.hpp"
#include "../assimp/scene.h"
#include "../assimp/postprocess.h"

namespace vg3o {
	class Model {
	public:
		Model() {};
		Model(const std::string& path);
		void draw();
	private:
		vector<ew::Mesh> meshes;
		std::string directoryPath;
		void loadModel(const std::string& path);
		void processAssimpMesh();
	};
}