#pragma once
#include <sstream>
#include <fstream>

namespace vg3o {
	struct Vertex {
		float x, y, z;
		float u, v;
	};

	std::string loadShaderSourceFromFile(const std::string& filePath);
	unsigned int createVAO(Vertex* vertexData, int numVertices, unsigned int* indicesData, int numIndices);

	class Shader {
	public:
		Shader(const std::string& vertexShader, const std::string& fragmentShader);
		void use();
		void setInt(const std::string& name, int value) const;
		void setFloat(const std::string& name, float value) const;
		void setVec2(const std::string& name, float x, float y) const;
		void setVec3(const std::string& name, float x, float y, float z) const;
		void setVec4(const std::string& name, float x, float y, float z, float a) const;
		void setVec3Array(const std::string& name, int arrLength, float arr[][3]) const;
	private:
		unsigned int m_id; 
	};
}