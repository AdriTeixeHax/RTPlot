#include "Texture.h"

#include "Renderer.h"
// OpenGL libraries
#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif
#include <GL/glew.h>

#include <stb_image/stb_image.h>

Texture::Texture(std::string filepath) :
	_rendererID(0), _filepath(filepath), _localBuffer(nullptr), _width(0), _height(0), _bpp(0)
{
	stbi_set_flip_vertically_on_load(1); // Flips the texture upside down because OpenGL is reversed (PNG)
	_localBuffer = stbi_load(filepath.c_str(), &_width, &_height, &_bpp, STBI_rgb_alpha);

	glCall(glGenTextures(1, &_rendererID));
	glCall(glBindTexture(GL_TEXTURE_2D, _rendererID));

	glCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	glCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	glCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	glCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

	glCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, _width, _height, 0, GL_RGBA, GL_UNSIGNED_BYTE, _localBuffer));
	glCall(glBindTexture(GL_TEXTURE_2D, 0));

	if (_localBuffer) stbi_image_free(_localBuffer);
}

Texture::~Texture(void)
{
	glCall(glDeleteTextures(1, &_rendererID));
}

void Texture::bind(uint32_t slot) const
{
	glCall(glActiveTexture(GL_TEXTURE0 + slot));
	glCall(glBindTexture(GL_TEXTURE_2D, _rendererID));
}

void Texture::unbind(void) const
{
	glCall(glBindTexture(GL_TEXTURE_2D, 0));
}
