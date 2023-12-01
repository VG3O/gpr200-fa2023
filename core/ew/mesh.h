/*
	Author: Eric Winebrenner

	Modified by Brandon Salvietti
		added "Texture" struct
		modified "MeshData" to include the new Texture struct and tangent data
		added function drawLoadTex for loading model textures onto a model when draw calling
*/

#pragma once
#include "ewMath/ewMath.h"
#include "shader.h"

namespace vg3o {
	struct Material {
		std::string name;

		// lighting properties
		ew::Vec3 ambient;
		ew::Vec3 diffuse;
		ew::Vec3 specular;
		float shininess;
		float opacity;

		bool hasDiffuse = false;
		bool hasSpecular = false;
		bool hasBump = false;
	};
}

namespace ew {
	struct Vertex {
		ew::Vec3 pos;
		ew::Vec3 normal;
		ew::Vec2 uv;
		ew::Vec3 tangent;
		ew::Vec3 bitangents;
	};

	struct Texture {
		unsigned int id;
		std::string type;
		std::string path;
	};

	struct MeshData {
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		std::vector<Texture> textures;
		vg3o::Material material;
	};


	enum class DrawMode {
		TRIANGLES = 0,
		POINTS = 1
	};

	class Mesh {
	public:
		Mesh() {};
		Mesh(const MeshData& meshData);
		void load(const MeshData& meshData);
		void draw(ew::DrawMode drawmode = DrawMode::TRIANGLES)const;
		void drawLoadTex(Shader& shader) const;
		inline std::vector<Texture> getTextures()const { return mTextures; }
		inline int getNumVertices()const { return m_numVertices; }
		inline int getNumIndices()const { return m_numIndices; }
	private:
		bool m_initialized = false;
		std::vector<Texture> mTextures;
		vg3o::Material mMaterial;
		unsigned int m_vao = 0;
		unsigned int m_vbo = 0;
		unsigned int m_ebo = 0;
		int m_numVertices = 0;
		int m_numIndices = 0;
	};
}