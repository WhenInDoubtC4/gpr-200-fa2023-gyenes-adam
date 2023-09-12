#pragma once

#include <cassert>
#include <fstream>
#include <string>

class Shader
{
public:
	static std::string loadSourceFromFile(const char* filepath);
};