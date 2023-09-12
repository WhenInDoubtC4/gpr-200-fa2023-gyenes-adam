#include "Shader.h"

std::string Shader::loadSourceFromFile(const char* filepath)
{
	std::ifstream file(filepath);
	assert(file.is_open());

	std::string source;
	std::getline(file, source, '\0');

	file.close();
	return source;
}
