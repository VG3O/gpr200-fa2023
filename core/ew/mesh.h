/*
	Author: Eric Winebrenner

	Modified by Brandon Salvietti
		added "Texture" struct
		modified "MeshData" to include the new Texture struct
*/

#pragma once
#include "ewMath/ewMath.h"
#include "shader.h"

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
		unsigned int m_vao = 0;
		unsigned int m_vbo = 0;
		unsigned int m_ebo = 0;
		int m_numVertices = 0;
		int m_numIndices = 0;
	};
}