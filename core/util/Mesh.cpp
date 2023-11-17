#include "Mesh.h"

bool operator==(const ew::Vec3& lhs, const ew::Vec3& rhs)
{
	return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
}

//Referencing https://stackoverflow.com/questions/17000255/calculate-tangent-space-in-c
Util::Mesh::TBArray Util::Mesh::calculateTB(ew::MeshData & completedMeshData)
{
	Util::Mesh::TBArray result(completedMeshData.vertices.size());

	//Traverse each triangle in the completed mesh
	for (unsigned int i = 0; i < completedMeshData.indices.size(); i += 3)
	{
		unsigned int i0 = completedMeshData.indices[i];
		unsigned int i1 = completedMeshData.indices[i + 1];
		unsigned int i2 = completedMeshData.indices[i + 2];

		ew::Vec3 pos0 = completedMeshData.vertices[i0].pos;
		ew::Vec3 pos1 = completedMeshData.vertices[i1].pos;
		ew::Vec3 pos2 = completedMeshData.vertices[i2].pos;

		ew::Vec2 uv0 = completedMeshData.vertices[i0].uv;
		ew::Vec2 uv1 = completedMeshData.vertices[i1].uv;
		ew::Vec2 uv2 = completedMeshData.vertices[i2].uv;

		ew::Vec3 normal = completedMeshData.vertices[i0].normal;

		ew::Vec3 deltaPos = pos0 == pos1 ? pos2 - pos0 : pos1 - pos0;

		ew::Vec2 deltaUV1 = uv1 - uv0;
		ew::Vec2 deltaUV2 = uv2 - uv1;

		//Avoid division by 0
		ew::Vec3 tangent = ew::Magnitude(deltaUV1) == 0.f ? deltaPos : deltaPos / ew::Magnitude(deltaUV1);
		tangent = ew::Normalize(tangent - ew::Dot(normal, tangent) * normal);

		ew::Vec3 bitangent = ew::Normalize(ew::Cross(normal, tangent));

		result[i0] = std::make_pair(tangent, bitangent);
		result[i1] = result[i0];
		result[i2] = result[i0];
	}

	return result;
}
