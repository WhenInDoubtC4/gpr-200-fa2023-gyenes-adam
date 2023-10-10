#pragma once

#include "../ew/ewMath/vec3.h"
#include "../ew/ewMath/mat4.h"

namespace Util
{
	struct Camera
	{
		ew::Vec3 position;
		ew::Vec3 target;
		float fov;
		float aspectRatio;
		float nearPlane;
		float farPlane;
		bool isOrthographic;
		float ortographicSize;
		ew::Mat4 ViewMatrix();
		ew::Mat4 ProjectionMatrix();
	};
}