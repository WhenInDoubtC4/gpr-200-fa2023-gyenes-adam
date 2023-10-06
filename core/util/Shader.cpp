#include "Shader.h"

Util::Shader::Shader(const char* vertFilepath, const char* fragFilepath)
{
	std::string vertSource = loadSourceFromFile(vertFilepath);
	std::string fragSource = loadSourceFromFile(fragFilepath);

	GLuint vertShader = createShader(GL_VERTEX_SHADER, vertSource.c_str());
	GLuint fragShader = createShader(GL_FRAGMENT_SHADER, fragSource.c_str());

	_shaderProgram = glCreateProgram();
	glAttachShader(_shaderProgram, vertShader);
	glAttachShader(_shaderProgram, fragShader);

	GLint status;
	glLinkProgram(_shaderProgram);
	glGetProgramiv(_shaderProgram, GL_LINK_STATUS, &status);
	if (!status)
	{
		char log[INFO_LOG_SIZE];
		glGetProgramInfoLog(_shaderProgram, INFO_LOG_SIZE, nullptr, log);
		std::cout << "Failed to link shader program: " << log << "\n";
	}

	glDeleteShader(vertShader);
	glDeleteShader(fragShader);
}

std::string Util::Shader::loadSourceFromFile(const char* filepath)
{
	std::ifstream file(filepath);
	assert(file.is_open());

	std::string source;
	std::getline(file, source, '\0');

	file.close();
	return source;
}

void Util::Shader::setInt(const char* name, int value)
{
	glUniform1i(glGetUniformLocation(_shaderProgram, name), value);
}

void Util::Shader::setFloat(const char* name, float value)
{
	glUniform1f(glGetUniformLocation(_shaderProgram, name), value);
}

void Util::Shader::setVec2(const char* name, float x, float y)
{
	glUniform2f(glGetUniformLocation(_shaderProgram, name), x, y);
}

void Util::Shader::setVec3(const char* name, float x, float y, float z)
{
	glUniform3f(glGetUniformLocation(_shaderProgram, name), x, y, z);
}

void Util::Shader::setVec4(const char* name, float x, float y, float z, float w)
{
	glUniform4f(glGetUniformLocation(_shaderProgram, name), x, y, z, w);
}

void Util::Shader::setMat4(const char* name, const ew::Mat4& value)
{
	glUniformMatrix4fv(glGetUniformLocation(_shaderProgram, name), 1, GL_FALSE, &value[0][0]);
}

void Util::Shader::exec()
{
	glUseProgram(_shaderProgram);
}

GLuint Util::Shader::createShader(GLenum type, const char* source)
{
	GLuint shader = glCreateShader(type);

	glShaderSource(shader, 1, &source, nullptr);

	GLint status;
	glCompileShader(shader);
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (!status)
	{
		char log[INFO_LOG_SIZE];
		glGetShaderInfoLog(shader, INFO_LOG_SIZE, nullptr, log);
		std::cout << "Error compiling shader: " << log << "\n";
	}

	return shader;
}
