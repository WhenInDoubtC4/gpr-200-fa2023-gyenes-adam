/*
* Author: Adam Gyenes
* Extension of ew::mesh to calculate TBN
*/

#include "../ew/mesh.h"
/*
* Created by Adam Gyenes
*/

#include "../ew/external/glad.h"

namespace Util
{
	class Mesh
	{
	//Based on ew::Mesh
	public:
		Mesh() {};
		Mesh(const ew::MeshData& meshData);

		void load(const ew::MeshData& meshData);
		void draw(ew::DrawMode drawMode = ew::DrawMode::TRIANGLES) const;

	private:
		typedef std::vector<std::pair<ew::Vec3, ew::Vec3>> TBArray;

		struct ExVertex
		{
			ew::Vec3 pos;
			ew::Vec3 normal;
			ew::Vec3 tangent;
			ew::Vec3 bitangent;
			ew::Vec2 uv;
		};

		std::vector<ExVertex> _exVertexData;

		//bool operator==(const ew::Vec3& lhs, const ew::Vec3& rhs);

		TBArray calculateTB(const ew::MeshData& completedMeshData);

		bool _initialized = false;

		GLuint _vao = 0;
		GLuint _vbo = 0;
		GLuint _ebo = 0;
		int _vertexCount = 0;
		int _indexCount = 0;
	};
}