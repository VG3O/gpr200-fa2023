/*
	Author: Brandon Salvietti

	Base functions "getTextureFormat" and "loadTexture" written by Eric Winebrenner
	"loadTexture" modified by Brandon Salvietti
*/

#include "texture.h"
#include "../ew/external/stb_image.h"
#include "../ew/external/glad.h"


static int getTextureFormat(int numComponents) {
	switch (numComponents) {
	default:
		return GL_RGBA;
	case 3:
		return GL_RGB;
	case 2:
		return GL_RG;
	}
}
namespace vg3o {
	unsigned int loadTexture(const char* filePath, const std::string& directory) {
		std::string filename = std::string(filePath);
		filename = directory + '/' + filename;

		int width, height, numComponents;
		unsigned char* data = stbi_load(filename.c_str(), &width, &height, &numComponents, 0);
		if (data == NULL) {
			printf("Failed to load image %s", filePath);
			stbi_image_free(data);
			return 0;
		}
		unsigned int texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		int format = getTextureFormat(numComponents);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		float borderColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

		glGenerateMipmap(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, NULL);
		stbi_image_free(data);
		return texture;
	}
}

