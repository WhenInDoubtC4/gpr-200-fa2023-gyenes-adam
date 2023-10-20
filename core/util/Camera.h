#pragma once

#include "../ew/ewMath/vec2.h"
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
		float ortographicHeight;
		ew::Mat4 ViewMatrix();
		ew::Mat4 ProjectionMatrix();
	};

	struct CameraControls
	{
		ew::Vec2 prevMousePos;
		float yawDeg = -90.f;
		float pitchDeg = 0.f; 
		float mouseSensitivity = 0.1f;
		bool storeInitialMousePos = true;
		float movementSpeed = 5.f;
	};
}