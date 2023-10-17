#pragma once

#define _USE_MATH_DEFINES

#include <math.h>

#include "../ew/ewMath/mat4.h"
#include "../ew/ewMath/vec3.h"

#include "Global.h"

namespace Util
{
	ew::Mat4& operator*=(ew::Mat4& lhs, const ew::Mat4& rhs)
	{
		lhs = lhs * rhs;
		return lhs;
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

			result *= Util::Translate(position);

			result *= Util::RotateY(DegToRad(rotateDeg.y));
			result *= Util::RotateX(DegToRad(rotateDeg.x));
			result *= Util::RotateZ(DegToRad(rotateDeg.z));

			result *= Util::Scale(scale);

			return result;
		}
	};

	inline ew::Mat4 LookAt(ew::Vec3 cameraPosition, ew::Vec3 target, ew::Vec3 upVector = ew::Vec3(0.f, 1.f, 0.f))
	{
		//Calculate new basis vectors
		ew::Vec3 forward = ew::Normalize(cameraPosition - target);
		ew::Vec3 right = ew::Normalize(ew::Cross(upVector, forward));
		ew::Vec3 newUp = ew::Normalize(ew::Cross(forward, right));

		//Transpose (=inverse) of rotation matrix * inverse translate, precomputed
		return ew::Mat4(
			right.x, right.y, right.z, -ew::Dot(right, cameraPosition),
			newUp.x, newUp.y, newUp.z, -ew::Dot(newUp, cameraPosition),
			forward.x, forward.y, forward.z, -ew::Dot(forward, cameraPosition),
			0, 0, 0, 1
		);
	}

	inline ew::Mat4 Ortographic(float height, float aspectRatio, float nearPlane, float farPlane)
	{
		float width = height * aspectRatio;

		//Bounds
		float right = width / 2.f;
		float top = height / 2.f;
		float left = -right;
		float bottom = -top;

		return ew::Mat4(
			2.f / (right - left), 0, 0, -(right + left) / (right - left),
			0, 2.f / (top - bottom), 0, -(top + bottom) / (top - bottom),
			0, 0, -2.f / (farPlane - nearPlane), -(farPlane + nearPlane) / (farPlane - nearPlane),
			0, 0, 0, 1
		);
	}

	inline ew::Mat4 Perspective(float fov, float aspectRatio, float nearPlane, float farPlane)
	{
		float fovRad = DegToRad(fov);

		return ew::Mat4(
			1.f / (tan(fovRad / 2.f) * aspectRatio), 0, 0, 0,
			0, 1.f / tan(fovRad / 2.f), 0, 0,
			0, 0, (nearPlane + farPlane) / (nearPlane - farPlane), (2 * farPlane * nearPlane) / (nearPlane - farPlane),
			0, 0, -1, 0
		);
	}
}