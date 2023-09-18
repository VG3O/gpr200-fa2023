#pragma once
#include <sstream>
#include <fstream>

namespace vg3o {
	std::string loadShaderSourceFromFile(const std::string& filePath);
	unsigned int createVAO(float* vertexData, int numVertices);

	class Shader {
	public:
		Shader(const std::string& vertexShader, const std::string& fragmentShader);
		void use();
		void setInt(const std::string& name, int value) const;
		void setFloat(const std::string& name, float value) const;
		void setVec2(const std::string& name, float x, float y) const;
		void setVec3(const std::string& name, float x, float y, float z) const;
		void setVec4(const std::string& name, float x, float y, float z, float a) const;
	private:
		unsigned int m_id; 
	};
}