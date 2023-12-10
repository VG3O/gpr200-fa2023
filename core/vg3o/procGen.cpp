#include "procGen.h"
#include <iostream>

namespace vg3o {
	ew::MeshData createSphere(float radius, int numSegments) {
		ew::MeshData newMesh = ew::MeshData();

		float thetaStep = (2.0 * ew::PI) / numSegments;
		float phiStep = ew::PI / numSegments;
		for (int row = 0; row <= numSegments; row++) {
			//First and last row converge at poles
			float phi = row * phiStep;
			for (int column = 0; column <= numSegments; column++) {
				ew::Vertex v = ew::Vertex();
				float theta = column * thetaStep;
				v.pos = ew::Vec3(radius * cos(theta) * sin(phi), radius * cos(phi), radius * sin(theta) * sin(phi));
				v.normal = ew::Normalize(v.pos);
				v.uv = ew::Vec2(column/(float)(numSegments+1), row/(float)(numSegments+1));
				newMesh.vertices.push_back(v);
			}
		}
		// indices
		int poleStart = 0;
		int sideStart = numSegments + 1;
		for (int i = 0; i < numSegments; i++) {
			newMesh.indices.push_back(sideStart + i);
			newMesh.indices.push_back(poleStart + i);
			newMesh.indices.push_back(sideStart + i + 1);
		}

		int totalColumns = numSegments + 1;
		for (int row = 1; row < numSegments - 1; row++) {
			for (int column = 0; column < numSegments; column++) {
				int start = row * totalColumns + column;

				newMesh.indices.push_back(start);
				newMesh.indices.push_back(start + 1);
				newMesh.indices.push_back(start + totalColumns);

				newMesh.indices.push_back(start + 1);
				newMesh.indices.push_back(start + totalColumns + 1);
				newMesh.indices.push_back(start + totalColumns);
			}
		}
		poleStart = newMesh.vertices.size() - (numSegments)-1;
		sideStart = poleStart - numSegments-1;
		for (int i = 0; i < numSegments; i++) {
			newMesh.indices.push_back(sideStart + i + 1);
			newMesh.indices.push_back(poleStart + i);
			newMesh.indices.push_back(sideStart + i);
		}
		return newMesh;
	}
	ew::MeshData createCylinder(float height, float radius, int numSegments) {
		ew::MeshData newMesh = ew::MeshData();
		float topY = height / 2, bottomY = -topY;
		float step = (2 * ew::PI) / (float)numSegments;
		// make vertices
		ew::Vertex top = ew::Vertex();
		top.pos = ew::Vec3(0, topY, 0);
		top.normal = ew::Vec3(0, 1, 0);
		top.uv = ew::Vec2(0.5, 0.5);
		newMesh.vertices.push_back(top);
			
		// top cap verticies
		// up normal - use theta for u and v , but convert it to 0,1 space from -1,1 space like we did in assignment 2
		for (int segment = 0; segment <= numSegments; segment++) {
			ew::Vertex v = ew::Vertex();
			float theta = segment * step;
			v.pos = ew::Vec3(cos(theta) * radius, topY, sin(theta) * radius);
			v.normal = ew::Vec3(0, 1, 0);
			ew::Vec2 uvPos = ew::Vec2(cos(theta), sin(theta)) * 0.5 + 0.5;
			v.uv = uvPos;
			newMesh.vertices.push_back(v);
		}
		// side normal
		for (int segment = 0; segment <= numSegments; segment++) {
			ew::Vertex v = ew::Vertex();
			float theta = segment * step;
			v.pos = ew::Vec3(cos(theta) * radius, topY, sin(theta) * radius);
			v.normal = ew::Vec3(cos(theta), 0, sin(theta));
			v.uv = ew::Vec2(segment / (float)(numSegments+2), 1);
			newMesh.vertices.push_back(v);
		}

		// bottom cap verticies
		// side normal
		for (int segment = 0; segment <= numSegments; segment++) {
			ew::Vertex v = ew::Vertex();
			float theta = segment * step;
			v.pos = ew::Vec3(cos(theta) * radius, bottomY, sin(theta) * radius);
			v.normal = ew::Vec3(cos(theta), 0, sin(theta));
			v.uv = ew::Vec2(segment / (float)(numSegments+2), 0);
			newMesh.vertices.push_back(v);
		}
		// down normal
		for (int segment = 0; segment <= numSegments; segment++) {
			ew::Vertex v = ew::Vertex();
			float theta = segment * step;
			v.pos = ew::Vec3(cos(theta) * radius, bottomY, sin(theta) * radius);
			v.normal = ew::Vec3(0, -1, 0);
			ew::Vec2 uvPos = ew::Vec2(cos(theta), sin(theta)) * 0.5 + 0.5;
			v.uv = uvPos;
			newMesh.vertices.push_back(v);
		}

		ew::Vertex bottom = ew::Vertex();
		bottom.pos = ew::Vec3(0, bottomY, 0);
		bottom.normal = ew::Vec3(0, -1, 0);
		bottom.uv = ew::Vec2(0.5, 0.5);
		newMesh.vertices.push_back(bottom);

		// cap indices
		for (int cap = 0; cap < 2; cap++) {
			int start = cap == 1 ? (numSegments*3) + 3 : 1, center = cap == 1 ? newMesh.vertices.size()-1 : 0;
			int topWind = cap == 0 ? 1 : 0, bottomWind = cap == 1 ? 1 : 0;
			for (int segment = 0; segment < numSegments+1; segment++) {
				newMesh.indices.push_back(start + segment + bottomWind);
				newMesh.indices.push_back(center);
				newMesh.indices.push_back(start + segment + topWind);
			}
		}
		// side indices
		int side = numSegments;
		int columns = numSegments + 2;
		for (int column = 0; column < columns; column++) {
			int start = side + column;
			newMesh.indices.push_back(start);
			newMesh.indices.push_back(start + 1);
			newMesh.indices.push_back(start + columns);

			newMesh.indices.push_back(start + columns);
			newMesh.indices.push_back(start + 1);
			newMesh.indices.push_back(start + columns + 1);
		}
		return newMesh;
	}
	ew::MeshData createPlane(float width, float height, int subdivisions) {
		ew::MeshData mesh;
		int columns = subdivisions + 1;
		for (size_t row = 0; row <= subdivisions; row++)
		{
			for (size_t col = 0; col <= subdivisions; col++)
			{
				ew::Vertex v;
				v.uv.x = ((float)col / subdivisions);
				v.uv.y = ((float)row / subdivisions);
				v.pos.x = -width / 2 + width * v.uv.x;
				v.pos.y = 0;
				v.pos.z = height / 2 - height * v.uv.y;
				v.normal = ew::Vec3(0, 1, 0);
				mesh.vertices.push_back(v);
			}
		}
		//INDICES
		for (size_t row = 0; row < subdivisions; row++)
		{
			for (size_t col = 0; col < subdivisions; col++)
			{
				int start = row * columns + col;
				// triangle 1
				mesh.indices.push_back(start); // index 0 
				mesh.indices.push_back(start + 1); // index 1
				mesh.indices.push_back(start + columns + 1); // index 3
				// triangle 2
				mesh.indices.push_back(start + columns + 1); // index 3
				mesh.indices.push_back(start + columns); // index 2
				mesh.indices.push_back(start); // index 0
			}
		}

		getTangentVectors(mesh.vertices, mesh.indices);

		return mesh;
	}
	// thank you Michael Grieco for the tutorial on how to implement tangent space
	void getTangentVectors(std::vector<ew::Vertex>& vertices, std::vector<unsigned int>& indices) {
		// this exists to count the number of contributions this vertex has made
		// to the overall tangent average
		unsigned char* counts = (unsigned char*)malloc(vertices.size() * sizeof(unsigned char));
		for (unsigned int i = 0; i < vertices.size(); i++) {
			counts[i] = 0;
		}

		for (unsigned int i = 0; i < indices.size(); i += 3) {
			ew::Vertex v1 = vertices[indices[i]];
			ew::Vertex v2 = vertices[indices[i+1]];
			ew::Vertex v3 = vertices[indices[i+2]];
		
			// triangle 1 tangents
			ew::Vec3 tangent;
			// get the edges to the triangle
			ew::Vec3 edge1 = v2.pos - v1.pos;
			ew::Vec3 edge2 = v3.pos - v1.pos;
			// get the uv delta
			ew::Vec2 uvDelta1 = v2.uv - v1.uv;
			ew::Vec2 uvDelta2 = v3.uv - v1.uv;

			// calculate tangent vectors for these vertices
			float f = 1.0f / (uvDelta1.x * uvDelta2.y - uvDelta2.x * uvDelta1.y);
			tangent.x = f * (uvDelta2.y * edge1.x - uvDelta1.y * edge2.x);
			tangent.y = f * (uvDelta2.y * edge1.y - uvDelta1.y * edge2.y);
			tangent.z = f * (uvDelta2.y * edge1.z - uvDelta1.y * edge2.z);
			
			// average vectors
			averageVectors(vertices[indices[i]].tangent, tangent, counts[indices[i]]++);
			averageVectors(vertices[indices[i + 1]].tangent, tangent, counts[indices[i + 1]]++);
			averageVectors(vertices[indices[i + 2]].tangent, tangent, counts[indices[i + 2]]++);
		}
	}
	void averageVectors(ew::Vec3& base, ew::Vec3 addition, unsigned char existingContributions) {
		if (!existingContributions) { base = addition; }
		else {
			float f = 1 / ((float)existingContributions + 1);
			base *= (float)(existingContributions)*f;
			base += addition * f;
		}
	}
}