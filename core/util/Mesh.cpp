/*
* Created by Adam Gyenes
*/

#include "Mesh.h"

constexpr GLuint POSITION_ATTRIBUTE_INDEX = 0;
constexpr GLuint NORMAL_ATTRIBUTE_INDEX = 1;
constexpr GLuint TANGENT_ATTRIBUTE_INDEX = 2;
constexpr GLuint BITANGENT_ATTRIBUTE_INDEX = 3;
constexpr GLuint UV_ATTRIBUTE_INDEX = 4;

bool operator==(const ew::Vec3& lhs, const ew::Vec3& rhs)
{
	return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
}

Util::Mesh::Mesh(const ew::MeshData& meshData)
{
	load(meshData);
}

void Util::Mesh::load(const ew::MeshData& meshData)
{
	if (meshData.vertices.empty()) return;

	//Construct extended vertex data
	_exVertexData.reserve(meshData.vertices.size());
	
	TBArray tbData = calculateTB(meshData);
	for (size_t i = 0; i < meshData.vertices.size(); i++)
	{
		ExVertex newVertex;

		newVertex.pos = meshData.vertices[i].pos;
		newVertex.normal = meshData.vertices[i].normal;
		newVertex.uv = meshData.vertices[i].uv;

		if (tbData.size() - 1 < i) continue;

		newVertex.tangent = tbData[i].first;
		newVertex.bitangent = tbData[i].second;

		_exVertexData.push_back(newVertex);
	}

	if (!_initialized)
	{
		glGenVertexArrays(1, &_vao);
		glBindVertexArray(_vao);

		glGenBuffers(1, &_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, _vbo);

		glGenBuffers(1, &_ebo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);

		glVertexAttribPointer(POSITION_ATTRIBUTE_INDEX, 3, GL_FLOAT, GL_FALSE, sizeof(ExVertex), reinterpret_cast<void*>(offsetof(ExVertex, pos)));
		glEnableVertexAttribArray(POSITION_ATTRIBUTE_INDEX);

		glVertexAttribPointer(NORMAL_ATTRIBUTE_INDEX, 3, GL_FLOAT, GL_FALSE, sizeof(ExVertex), reinterpret_cast<void*>(offsetof(ExVertex, normal)));
		glEnableVertexAttribArray(NORMAL_ATTRIBUTE_INDEX);

		glVertexAttribPointer(TANGENT_ATTRIBUTE_INDEX, 3, GL_FLOAT, GL_FALSE, sizeof(ExVertex), reinterpret_cast<void*>(offsetof(ExVertex, tangent)));
		glEnableVertexAttribArray(TANGENT_ATTRIBUTE_INDEX);

		glVertexAttribPointer(BITANGENT_ATTRIBUTE_INDEX, 3, GL_FLOAT, GL_FALSE, sizeof(ExVertex), reinterpret_cast<void*>(offsetof(ExVertex, bitangent)));
		glEnableVertexAttribArray(BITANGENT_ATTRIBUTE_INDEX);

		glVertexAttribPointer(UV_ATTRIBUTE_INDEX, 2, GL_FLOAT, GL_FALSE, sizeof(ExVertex), reinterpret_cast<void*>(offsetof(ExVertex, uv)));
		glEnableVertexAttribArray(UV_ATTRIBUTE_INDEX);
	}

	_initialized = true;

	glBindVertexArray(_vao);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);

	glBufferData(GL_ARRAY_BUFFER, sizeof(ExVertex) * _exVertexData.size(), _exVertexData.data(), GL_STATIC_DRAW);

	if (!meshData.indices.empty())
	{
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * meshData.indices.size(), meshData.indices.data(), GL_STATIC_DRAW);
	}

	_vertexCount = _exVertexData.size();
	_indexCount = meshData.indices.size();

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

//Referencing https://stackoverflow.com/questions/17000255/calculate-tangent-space-in-c
//and https://gamedev.stackexchange.com/questions/68612/how-to-compute-tangent-and-bitangent-vectors
Util::Mesh::TBArray Util::Mesh::calculateTB(const ew::MeshData& completedMeshData)
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

		ew::Vec3 deltaPos1 = pos1 - pos0;
		ew::Vec3 deltaPos2 = pos2 - pos0;

		ew::Vec2 deltaUV1 = uv1 - uv0;
		ew::Vec2 deltaUV2 = uv2 - uv1;

		float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
		ew::Vec3 xDir(
			(deltaUV2.y * deltaPos1.x - deltaUV1.y * deltaPos2.x) * r,
			(deltaUV2.y * deltaPos1.y - deltaUV1.y * deltaPos2.y) * r,
			(deltaUV2.y * deltaPos1.z - deltaUV1.y * deltaPos2.z) * r);

		result[i0] = std::make_pair(-xDir, -xDir);
		result[i1] = std::make_pair(-xDir, -xDir);
		result[i2] = std::make_pair(-xDir, -xDir);
	}

	for (size_t i = 0; i < completedMeshData.vertices.size(); i++)
	{
		ew::Vec3 oldTangent = result[i].first;
		ew::Vec3 normal = completedMeshData.vertices[i].normal;

		ew::Vec3 tangent = ew::Normalize(oldTangent - normal * ew::Dot(normal, oldTangent));
		ew::Vec3 bitangent = ew::Normalize(ew::Cross(normal, tangent));

		result[i] = std::make_pair(tangent, bitangent);
	}

	return result;
}

void Util::Mesh::draw(ew::DrawMode drawMode) const
{
	glBindVertexArray(_vao);
	if (drawMode == ew::DrawMode::TRIANGLES)
	{
		glDrawElements(GL_TRIANGLES, _indexCount, GL_UNSIGNED_INT, nullptr);
	}
	else
	{
		glDrawArrays(GL_POINTS, 0, _vertexCount);
	}
}