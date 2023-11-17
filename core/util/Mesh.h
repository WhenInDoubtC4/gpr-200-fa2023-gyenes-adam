/*
* Author: Adam Gyenes
* Extension of ew::mesh to calculate TBN
*/

#include "../ew/mesh.h"

namespace Util
{
	class Mesh
	{
	private:
		typedef std::vector<std::pair<ew::Vec3, ew::Vec3>> TBArray;

		//bool operator==(const ew::Vec3& lhs, const ew::Vec3& rhs);

		TBArray calculateTB(ew::MeshData& completedMeshData);
	};
}