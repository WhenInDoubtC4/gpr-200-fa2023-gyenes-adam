#pragma once

#include <map>

#include "../ew/external/glad.h"
#include "../ew/external/stb_image.h"

const auto COMPONENTS_TO_FORMAT = std::map<int, GLenum>{
	{1, GL_RED},
	{2, GL_RG},
	{3, GL_RGB},
	{4, GL_RGBA},
};

namespace Util
{
	GLuint loadTexture(const char* filepath, GLint wrapMode = GL_CLAMP_TO_EDGE, GLint filtering = GL_LINEAR, bool flipVertical = true);
}