#include "texture.h"
#include "../ew/external/stb_image.h"
#include "../ew/external/glad.h"

/*GLenum getWrapMode(int wrapMode)
{
	switch (wrapMode)
	{
		case 1:
			return GL_REPEAT;
			break;
		case 2:
			return GL_MIRRORED_REPEAT;
			break;
		case 3:
			return GL_CLAMP_TO_EDGE;
			break;
	}
}

GLenum getFilterMode(int filterMode, int mipmappingMode = 0)
{
	GLenum selected;
	switch (filterMode)
	{
		case 1:
			selected = GL_LINEAR;
			break;
		case 2:
			selected = GL_NEAREST;
			break;
	}
	if (mipmappingMode != 0)
	{
		switch (mipmappingMode)
		{
			case 1:
				if (selected == GL_LINEAR) { return GL_LINEAR_MIPMAP_NEAREST; }
				else if (selected == GL_NEAREST) { return GL_NEAREST_MIPMAP_NEAREST; }
				break;
			case 2:
				if (selected == GL_LINEAR) { return GL_LINEAR_MIPMAP_LINEAR; }
				else if (selected == GL_NEAREST) { return GL_NEAREST_MIPMAP_LINEAR; }
				break;
		}
	}
	else { return selected; }
}*/

namespace vg3o {
	unsigned int loadTexture(const char* filePath, int horizontalWrap, int verticalWrap, int minFilter, int magFilter)
	{
		stbi_set_flip_vertically_on_load(true);

		int width, height, numComponents;
		unsigned char* data = stbi_load(filePath, &width, &height, &numComponents, 0);
		if (data == NULL)
		{
			printf("Failed to load image %s", filePath);
			stbi_image_free(data);
			return 0;
		}

		unsigned int texture;
		glGenTextures(1, &texture);
 		glBindTexture(GL_TEXTURE_2D, texture);

		GLenum glFormat;
		switch (numComponents)
		{
		case 1:
			glFormat = GL_RED;
			break;
		case 2:
			glFormat = GL_RG;
			break;
		case 3:
			glFormat = GL_RGB;
			break;
		case 4:
			glFormat = GL_RGBA;
			break;
		}

		glTexImage2D(GL_TEXTURE_2D, 0, glFormat, width, height, 0, glFormat, GL_UNSIGNED_BYTE, data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, horizontalWrap);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, verticalWrap);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
		if (magFilter != 0) { glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter); }

		glGenerateMipmap(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, 0);
		stbi_image_free(data);
		return texture;
	}
}