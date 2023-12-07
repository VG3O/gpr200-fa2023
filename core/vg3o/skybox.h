#pragma once
#include <iostream>
#include <vector>
#include <string>

namespace vg3o
{
	unsigned int loadCubemap(std::vector<std::string> faces);
    unsigned int createVAO(float* vertexData, int numVertices);
}