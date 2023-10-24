#include "ProcGen.h"

ew::MeshData Util::createPlane(float width, float height, int subdivisions)
{
	ew::MeshData result;

	int totalCols = subdivisions + 1;
	for (int row = 0; row <= subdivisions; row++)
	{
		for (int col = 0; col <= subdivisions; col++)
		{
			//Generate vert
			ew::Vertex currentVert;
			currentVert.pos.x = width * col / float(subdivisions);
			currentVert.pos.y = height * row / float(subdivisions);

			//Generate normals
			currentVert.normal = ew::Vec3(0.f, 0.f, -1.f);

			//Generate UVs
			currentVert.uv = ew::Vec2(width * (float(col) / subdivisions), height * (float(row) / subdivisions));

			result.vertices.push_back(currentVert);

			//Skip last quad
			if (row == subdivisions || col == subdivisions) continue;
			
			//Generate indicies
			int startIndex = row * totalCols + col;
			//Bottom right
			result.indices.push_back(startIndex);
			result.indices.push_back(startIndex + 1);
			result.indices.push_back(startIndex + totalCols + 1);
			//Top left
			result.indices.push_back(startIndex + totalCols + 1);
			result.indices.push_back(startIndex + totalCols);
			result.indices.push_back(startIndex);
		}
	}

	return result;
}

ew::MeshData Util::createCylidner(float height, float radius, int segments)
{
	ew::MeshData result;

	//Top center
	float topY = height / 2.f;
	ew::Vertex topCenterVert;
	topCenterVert.pos = ew::Vec3(0.f, topY, 0.f);
	result.vertices.push_back(topCenterVert);
	
	//Top ring
	float angularStep = 2 * M_PI / float(segments);
	for (int i = 0; i <= segments; i++)
	{
		float currentAngle = i * angularStep;

		ew::Vertex currentVert;
		currentVert.pos = ew::Vec3(cos(currentAngle) * radius, topY, sin(currentAngle) * radius);
		result.vertices.push_back(currentVert);

		result.indices.push_back(i);
		result.indices.push_back(0); //Top center
		result.indices.push_back(i + 1);
	}

	//Bottom center
	float bottomY = -topY;
	ew::Vertex bottomCenterVert;
	bottomCenterVert.pos = ew::Vec3(0.f, bottomY, 0.f);
	result.vertices.push_back(bottomCenterVert);

	//Bottom ring
	int bottomCenterIndex = segments + 2;
	for (int i = 0; i <= segments; i++)
	{
		float currentAngle = i * angularStep;

		ew::Vertex currentVert;
		currentVert.pos = ew::Vec3(cos(currentAngle) * radius, bottomY, sin(currentAngle) * radius);
		result.vertices.push_back(currentVert);

		result.indices.push_back(bottomCenterIndex + i);
		result.indices.push_back(bottomCenterIndex); //Bottom center
		result.indices.push_back(bottomCenterIndex + i + 1);
	}

	//Generate side indicies
	int topVertIndex = 1;
	for (int i = 0; i < segments; i++)
	{
		int startIndex = topVertIndex + i;

		result.indices.push_back(startIndex);
		result.indices.push_back(startIndex + 1);
		result.indices.push_back(startIndex + segments + 2);

		result.indices.push_back(startIndex + 1);
		result.indices.push_back(startIndex + segments + 3);
		result.indices.push_back(startIndex + segments + 2);
	}

	return result;
}

ew::MeshData Util::createSphere(float radius, int segments)
{
	ew::MeshData result;

	return result;
}