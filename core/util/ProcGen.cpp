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
			currentVert.normal = ew::Vec3(0.f, 0.f, 1.f);

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
	topCenterVert.normal = ew::Vec3(0.f, 1.f, 0.f);
	topCenterVert.uv = ew::Vec2(0.5f, 0.5f);
	result.vertices.push_back(topCenterVert);
	
	//Top ring
	float angularStep = 2 * M_PI / float(segments);
	for (int i = 0; i <= segments; i++)
	{
		float currentAngle = i * angularStep;

		ew::Vertex currentVert;
		currentVert.pos = ew::Vec3(cos(currentAngle) * radius, topY, sin(currentAngle) * radius);
		currentVert.normal = ew::Vec3(0.f, 1.f, 0.f);
		currentVert.uv = ew::Vec2(cos(currentAngle) / 2.f + 0.5f, sin(currentAngle) / 2.f + 0.5f);
		result.vertices.push_back(currentVert);

		result.indices.push_back(i);
		result.indices.push_back(0); //Top center
		result.indices.push_back(i + 1);
	}
	for (int i = 0; i <= segments; i++)
	{
		float currentAngle = i * angularStep;

		ew::Vertex currentVert;
		currentVert.pos = ew::Vec3(cos(currentAngle) * radius, topY, sin(currentAngle) * radius);
		currentVert.normal = ew::Normalize(currentVert.pos - topCenterVert.pos);
		currentVert.uv = ew::Vec2(i / float(segments), 1.f);
		result.vertices.push_back(currentVert);
	}

	//Bottom center
	float bottomY = -topY;
	ew::Vertex bottomCenterVert;
	bottomCenterVert.pos = ew::Vec3(0.f, bottomY, 0.f);
	bottomCenterVert.normal = ew::Vec3(0.f, -1.f, 0.f);
	bottomCenterVert.uv = ew::Vec2(0.5f, 0.5f);
	result.vertices.push_back(bottomCenterVert);

	//Bottom ring
	int bottomCenterIndex = segments * 2 + 3;
	for (int i = 0; i <= segments; i++)
	{
		float currentAngle = i * angularStep;

		ew::Vertex currentVert;
		currentVert.pos = ew::Vec3(cos(currentAngle) * radius, bottomY, sin(currentAngle) * radius);
		currentVert.normal = ew::Vec3(0.f, -1.f, 0.f);
		currentVert.uv = ew::Vec2(cos(currentAngle) / 2.f + 0.5f, sin(currentAngle) / 2.f + 0.5f);
		result.vertices.push_back(currentVert);

		result.indices.push_back(bottomCenterIndex); //Bottom center
		result.indices.push_back(bottomCenterIndex + i);
		result.indices.push_back(bottomCenterIndex + i + 1);
	}
	for (int i = 0; i <= segments; i++)
	{
		float currentAngle = i * angularStep;

		ew::Vertex currentVert;
		currentVert.pos = ew::Vec3(cos(currentAngle) * radius, bottomY, sin(currentAngle) * radius);
		currentVert.normal = ew::Normalize(currentVert.pos - bottomCenterVert.pos);
		currentVert.uv = ew::Vec2(i / float(segments), 0.f);
		result.vertices.push_back(currentVert);
	}

	//Generate side indicies
	int topVertIndex = segments + 2;
	for (int i = 0; i < segments; i++)
	{
		int startIndex = topVertIndex + i;

		result.indices.push_back(startIndex);
		result.indices.push_back(startIndex + 1);
		result.indices.push_back(startIndex + segments * 2 + 1 + 2);

		result.indices.push_back(startIndex + 1);
		result.indices.push_back(startIndex + segments * 2 + 1 + 3);
		result.indices.push_back(startIndex + segments * 2 + 1 + 2);
	}

	return result;
}

ew::MeshData Util::createSphere(float radius, int segments)
{
	ew::MeshData result;

	float yawStep = 2.f * M_PI / segments; //Theta (pls use descriptive names instead of random letters)
	float pitchStep = M_PI / segments; //Phi
	for (int row = 0; row <= segments; row++)
	{
		float pitch = row * pitchStep;
		for (int col = 0; col <= segments; col++)
		{
			float yaw = col * yawStep;

			ew::Vertex currentVertex;
			currentVertex.pos.x = radius * sin(pitch) * sin(yaw);
			currentVertex.pos.y = radius * cos(pitch);
			currentVertex.pos.z = radius * sin(pitch) * cos(yaw);
			currentVertex.uv = ew::Vec2(col / float(segments), (row + 1) / float(segments + 2));
			currentVertex.normal = ew::Normalize(currentVertex.pos);
			result.vertices.push_back(currentVertex);
		}
	}

	//Top cap indicies
	int poleVertexIndex = 0;
	int lastCapVertexIndex = segments + 1;
	for (int i = 0; i < segments; i++)
	{
		result.indices.push_back(poleVertexIndex + i);
		result.indices.push_back(lastCapVertexIndex + i);
		result.indices.push_back(lastCapVertexIndex + i + 1);
	}

	//Bottom cap indicies
	poleVertexIndex = pow(segments, 2) + segments;
	lastCapVertexIndex = poleVertexIndex - segments - 1;
	for (int i = 0; i < segments; i++)
	{
		result.indices.push_back(lastCapVertexIndex + i);
		result.indices.push_back(poleVertexIndex + i);
		result.indices.push_back(lastCapVertexIndex + i + 1);
	}

	//Shell indicies
	for (int row = 1; row < segments - 1; row++)
	{
		for (int col = 0; col < segments; col++)
		{
			int startIndex = row * (segments + 1) + col;

			result.indices.push_back(startIndex);
			result.indices.push_back(startIndex + segments + 1);
			result.indices.push_back(startIndex + 1);

			result.indices.push_back(startIndex + 1);
			result.indices.push_back(startIndex + segments + 1);
			result.indices.push_back(startIndex + segments + 2);
		}
	}

	return result;
}

//From https://lindenreidblog.com/2017/11/06/procedural-torus-tutorial/
ew::MeshData Util::createTorus(float innerRadius, float outerRadius, int innerSegments, int outerSegments)
{
	ew::MeshData result;

	float innerAngleStep = 2.f * M_PI / innerSegments; //phi
	float outerAngleStep = 2.f * M_PI / outerSegments; //theta

	for (int stack = 0; stack < outerSegments ; stack++)
	{
		float outerAngle = stack * outerAngleStep;
		ew::Vec3 sliceCenterPos(cos(outerAngle) * outerRadius, sin(outerAngle) * outerRadius, 0.f);

		for (int slice = 0; slice < innerSegments; slice++)
		{
			float innerAngle = slice * innerAngleStep;

			//Generate vertices
			ew::Vertex currentVert;
			currentVert.pos.x = cos(outerAngle) * (outerRadius + cos(innerAngle) * innerRadius);
			currentVert.pos.y = sin(outerAngle) * (outerRadius + cos(innerAngle) * innerRadius);
			currentVert.pos.z = sin(innerAngle) * innerRadius;
			//Generate normals
			currentVert.normal = ew::Normalize(currentVert.pos - sliceCenterPos);
			//Generate UVs
			currentVert.uv = ew::Vec2(stack / float(outerSegments), slice / float(innerSegments));

			result.vertices.push_back(currentVert);
		}

		//Extra vertex in each slice for UV seam
		ew::Vertex extraSliceVert;
		extraSliceVert.pos.x = cos(outerAngle) * (outerRadius + innerRadius);
		extraSliceVert.pos.y = sin(outerAngle) * (outerRadius +  innerRadius);
		extraSliceVert.pos.z = 0.f;
		extraSliceVert.normal = ew::Normalize(extraSliceVert.pos - sliceCenterPos);
		extraSliceVert.uv = ew::Vec2(stack / float(outerSegments), 1);

		result.vertices.push_back(extraSliceVert);
	}

	//Extra slice for UV seam
	for (int slice = 0; slice < innerSegments; slice++)
	{
		float innerAngle = slice * innerAngleStep;
		ew::Vertex extraSliceVert;
		extraSliceVert.pos.x = (outerRadius + cos(innerAngle) * innerRadius);
		extraSliceVert.pos.y = 0;
		extraSliceVert.pos.z = sin(innerAngle) * innerRadius;
		extraSliceVert.normal = ew::Normalize(extraSliceVert.pos - ew::Vec3(outerRadius, 0.f, 0.f));
		extraSliceVert.uv = ew::Vec2(1, slice / float(innerSegments));
		result.vertices.push_back(extraSliceVert);
	}
	ew::Vertex sliceEndVert;
	sliceEndVert.pos.x = (outerRadius + innerRadius);
	sliceEndVert.pos.y = 0.f;
	sliceEndVert.pos.z = 0.f;
	sliceEndVert.normal = ew::Vec3(1.f, 0.f, 0.f);
	sliceEndVert.uv = ew::Vec2(1, 1);
	result.vertices.push_back(sliceEndVert);

	//Generate indicies
	for (int stack = 0; stack < outerSegments; stack++)
	{
		for (int slice = 0; slice <= innerSegments; slice++)
		{
			int innerStart = stack * (innerSegments + 1);

			result.indices.push_back(innerStart + slice);
			result.indices.push_back(innerStart + innerSegments + 1	+ slice);
			result.indices.push_back(innerStart + slice + 1);

			result.indices.push_back(innerStart + innerSegments + 1 + slice);
			result.indices.push_back(innerStart + innerSegments + 1 + slice + 1);
			result.indices.push_back(innerStart + slice + 1);
		}
	}

	return result;
}
