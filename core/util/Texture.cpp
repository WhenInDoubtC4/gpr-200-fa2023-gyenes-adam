#include "Texture.h"

GLuint Util::loadTexture(const char* filepath, GLint wrapMode, GLint filtering, bool flipVertical)
{
	stbi_set_flip_vertically_on_load(flipVertical);

	int width;
	int height;
	int numComponents;

	stbi_uc* data = stbi_load(filepath, &width, &height, &numComponents, 0);
	if (!data)
	{
		printf("Failed to load image %s", filepath);
		stbi_image_free(data);
		return 0;
	}

	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	GLenum format = COMPONENTS_TO_FORMAT.at(numComponents);
	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filtering);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	stbi_image_free(data);
	return texture;
}
