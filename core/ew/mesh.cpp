/*
	Author: Eric Winebrenner
*/

#include "mesh.h"
#include "ewMath/ewMath.h"
#include "external/glad.h"
#include <iostream>

namespace ew {
	Mesh::Mesh(const MeshData& meshData)
	{
		load(meshData);
	}
	void Mesh::load(const MeshData& meshData)
	{
		if (!m_initialized) {
			glGenVertexArrays(1, &m_vao);
			glBindVertexArray(m_vao);

			glGenBuffers(1, &m_vbo);
			glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

			glGenBuffers(1, &m_ebo);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
			//Position attribute
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, pos));
			glEnableVertexAttribArray(0);

			//Normal attribute
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, normal));
			glEnableVertexAttribArray(1);

			//UV attribute
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)(offsetof(Vertex, uv)));
			glEnableVertexAttribArray(2);

			m_initialized = true;
		}

		glBindVertexArray(m_vao);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);

		if (meshData.vertices.size() > 0) {
			glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * meshData.vertices.size(), meshData.vertices.data(), GL_STATIC_DRAW);
		}
		if (meshData.indices.size() > 0) {
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * meshData.indices.size(), meshData.indices.data(), GL_STATIC_DRAW);
		}
		m_numVertices = meshData.vertices.size();
		m_numIndices = meshData.indices.size();
		mTextures = meshData.textures;
		mMaterial = meshData.material;

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	void Mesh::draw(ew::DrawMode drawMode) const
	{
		glBindVertexArray(m_vao);
		if (drawMode == DrawMode::TRIANGLES) {
			glDrawElements(GL_TRIANGLES, m_numIndices, GL_UNSIGNED_INT, NULL);
		}
		else {
			glDrawArrays(GL_POINTS, 0, m_numVertices);
		}
		
	}
	void Mesh::drawLoadTex(Shader& shader) const
	{
		if (mTextures.size() > 0) {
			unsigned int diffuseNum = 1;
			unsigned int specularNum = 1;

			for (unsigned int i = 0; i < mTextures.size(); i++) {
				glActiveTexture(GL_TEXTURE0 + i);
				std::string number;
				std::string name = mTextures[i].type;
				if (name == "texture_diffuse") {
					number = std::to_string(diffuseNum++);
				}
				else if (name == "texture_specular") {
					number = std::to_string(specularNum++); 
				}
				shader.setInt("texture_diffuse1", i);
				glBindTexture(GL_TEXTURE_2D, mTextures[i].id);
			}
			glActiveTexture(GL_TEXTURE0);
		}
		shader.setVec3("_Material.ambientColor", mMaterial.ambient);
		shader.setVec3("_Material.specularColor", mMaterial.specular);
		shader.setVec3("_Material.diffuseColor", mMaterial.diffuse);
		shader.setFloat("_Material.shininess", mMaterial.shininess);
		shader.setFloat("_Material.opacity", mMaterial.opacity);
		shader.setInt("_Material.hasDiffuse", mMaterial.hasDiffuse);
		shader.setInt("_Material.hasSpecular", mMaterial.hasSpecular);
		shader.setInt("_Material.hasBump", mMaterial.hasBump);

		glBindVertexArray(m_vao);
		glDrawElements(GL_TRIANGLES, m_numIndices, GL_UNSIGNED_INT, NULL);
	}
}