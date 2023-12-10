#pragma once
#include "../ew/mesh.h"

namespace vg3o {
	ew::MeshData createSphere(float radius, int numSegments);
	ew::MeshData createCylinder(float height, float radius, int numSegments);
	ew::MeshData createPlane(float width, float height, int subdivisions);
	
	void getTangentVectors(std::vector<ew::Vertex>& vertices, std::vector<unsigned int>& indices);
	void averageVectors(ew::Vec3& base, ew::Vec3 addition, unsigned char existingContributions);
}