#pragma once

#define _USE_MATH_DEFINES

#include <math.h>

#include "../ew/ewMath/mat4.h"
#include "../ew/ewMath/vec3.h"

namespace Util
{
	ew::Mat4& operator*=(ew::Mat4& lhs, const ew::Mat4& rhs)
	{
		lhs = lhs * rhs;
		return lhs;
	}

	inline constexpr float DegToRad(float deg)
	{
		return deg * (M_PI / 180.f);
	}

	inline ew::Mat4 Identity()
	{
		return ew::Mat4(
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
		);
	}

	inline ew::Mat4 Scale(ew::Vec3 scale)
	{
		return ew::Mat4(
			scale.x, 0, 0, 0,
			0, scale.y, 0, 0,
			0, 0, scale.z, 0,
			0, 0, 0, 1
		);
	}

	inline ew::Mat4 RotateX(float angleRad)
	{
		float sinAngle = sin(angleRad);
		float cosAngle = cos(angleRad);

		return ew::Mat4(
			1, 0, 0, 0,
			0, cosAngle, -sinAngle, 0,
			0, sinAngle, cosAngle, 0,
			0, 0, 0, 1
		);
	}

	inline ew::Mat4 RotateY(float angleRad)
	{
		float sinAngle = sin(angleRad);
		float cosAngle = cos(angleRad);

		return ew::Mat4(
			cosAngle, 0 , sinAngle, 0,
			0, 1, 0, 0,
			-sinAngle, 0, cosAngle, 0,
			0, 0, 0, 1
		);
	}

	inline ew::Mat4 RotateZ(float angleRad)
	{
		float sinAngle = sin(angleRad);
		float cosAngle = cos(angleRad);

		return ew::Mat4(
			cosAngle, -sinAngle, 0, 0,
			sinAngle, cosAngle, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
		);
	}

	inline ew::Mat4 Translate(ew::Vec3 translate)
	{
		return ew::Mat4(
			1, 0, 0, translate.x,
			0, 1, 0, translate.y,
			0, 0, 1, translate.z,
			0, 0, 0, 1
		);
	}

	struct Transform
	{
		ew::Vec3 position = ew::Vec3(0.f);
		ew::Vec3 rotateDeg = ew::Vec3(0.f);
		ew::Vec3 scale = ew::Vec3(1.f);

		ew::Mat4 getModelMat() const
		{
			ew::Mat4 result = Identity();

			result *= Translate(position);

			result *= RotateY(DegToRad(rotateDeg.y));
			result *= RotateX(DegToRad(rotateDeg.x));
			result *= RotateZ(DegToRad(rotateDeg.z));

			result *= Scale(scale);

			return result;
		}
	};
}