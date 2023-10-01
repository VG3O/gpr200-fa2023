#pragma once

namespace vg3o {
	unsigned int loadTexture(const char* filePath, int horizontalWrap, int verticalWrap, int minFilter, int magFilter = 0, int mipmappingMin = 0, int mipmappingMag = 0);
}