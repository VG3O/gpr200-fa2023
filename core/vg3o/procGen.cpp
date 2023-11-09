#include "procGen.h"

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
		ew::MeshData newMesh = ew::MeshData();
		// make verts
		for (int row = 0; row <= subdivisions; row++) {
			for (int column = 0; column <= subdivisions; column++) {
				ew::Vertex v = ew::Vertex();
				v.pos = ew::Vec3(width * (column / (float)subdivisions), 0, -height * (row / (float)subdivisions));
				v.normal = ew::Vec3(0, 1, 0);
				v.uv = ew::Vec2((column / (float)subdivisions), (row / (float)subdivisions));
				newMesh.vertices.push_back(v);
			}
		} 
		// make tris
		int totalColumns = (subdivisions + 1);
		for (int row = 0; row < subdivisions; row++) {
			for (int column = 0; column < subdivisions; column++) {
				unsigned int start = row * totalColumns + column;
				// bottom triangle
				newMesh.indices.push_back(start);
				newMesh.indices.push_back(start + 1);
				newMesh.indices.push_back(start + totalColumns + 1);
				// top triangle
				newMesh.indices.push_back(start + totalColumns + 1);
				newMesh.indices.push_back(start + totalColumns);
				newMesh.indices.push_back(start);
			}
		}
		return newMesh;
	}
}