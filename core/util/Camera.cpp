#include "Camera.h"

#include "Transformations.h"

ew::Mat4 Util::Camera::ViewMatrix()
{
	return Util::LookAt(position, target);
}

ew::Mat4 Util::Camera::ProjectionMatrix()
{
	if (isOrthographic) return Util::Ortographic(ortographicHeight, aspectRatio, nearPlane, farPlane);
	
	return Util::Perspective(fov, aspectRatio, nearPlane, farPlane);
}