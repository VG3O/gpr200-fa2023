#include "../ew/external/glad.h"
#include "../ew/ewMath/ewMath.h"
#include "../ew/external/stb_image.h"
#include "skybox.h"

#include <vector>

namespace vg3o
{	
	unsigned int loadCubemap(std::vector<std::string> faces)
	{	
		unsigned int textureID;
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

		int width, height, nrChannels;
		for (unsigned int i = 0; i < faces.size(); i++)
		{
			unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
			if (data)
			{
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
				stbi_image_free(data);
			}
			else
			{
				std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
				stbi_image_free(data);
			}
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		return textureID;
	}

	unsigned int createVAO(float* vertexData, int numVertices)
	{
		int stride = 7; // how many attributes per vertex
		unsigned int vbo;
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		//Allocate space for + send vertex data to GPU.
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * stride * numVertices, vertexData, GL_STATIC_DRAW);

		unsigned int vao;
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
		//Tell vao to pull vertex data from vbo
		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		//Define position attribute (3 floats)
		// first paramter is the location int, second one is the amount of data being sent through, third one is the size allocation
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * stride, (const void*)0);
		glEnableVertexAttribArray(0);

		//Define color attribute (4 floats)
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(float) * stride, (const void*)(sizeof(float) * 3));
		glEnableVertexAttribArray(1);
		return vao;
	};

}