#pragma once

#define _USE_MATH_DEFINES

#include <math.h>

#include "../ew/mesh.h"

namespace Util
{
	ew::MeshData createPlane(float width, float height, int subdivisions);
	ew::MeshData createCylidner(float height, float radius, int segments);
	ew::MeshData createSphere(float radius, int segments);
	ew::MeshData createTorus(float innerRadius, float outerRadius, int innerSegments, int outerSegments);
}