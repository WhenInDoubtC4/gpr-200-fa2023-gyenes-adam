#pragma once

#include <cassert>
#include <fstream>
#include <string>
#include <iostream>

#include "../ew/external/glad.h"
#include "../ew/ewMath/mat4.h"
#include <GLFW/glfw3.h>

#include "Global.h"

#define SET_SHADER_TEXTURE(shader, name, texture, id) \
	glActiveTexture(GL_TEXTURE##id); \
	glBindTexture(GL_TEXTURE_2D, texture); \
	shader.setInt(name, id);

namespace Util
{
	class Shader
	{
	public:
		Shader(const char* vertFilepath, const char* fragFilepath);

		static std::string loadSourceFromFile(const char* filepath);

		void setInt(const char* name, int value);
		void setFloat(const char* name, float value);
		void setVec2(const char* name, float x, float y);
		void setVec3(const char* name, float x, float y, float z);
		void setVec4(const char* name, float x, float y, float z, float w);
		void setMat4(const char* name, const ew::Mat4& value);

		void exec();

	private:
		GLuint createShader(GLenum type, const char* source);

		GLuint _shaderProgram;
	};
}