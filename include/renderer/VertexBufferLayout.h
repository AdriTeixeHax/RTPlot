#ifndef _VERTEXBUFFERLAYOUT__H_
#define _VERTEXBUFFERLAYOUT__H_

#include <vector>

// OpenGL libraries
#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif
#include <GL/glew.h>

#include "Renderer.h"

struct VertexBufferElement
{
	uint32_t type;
	uint32_t count;
	uint8_t normalized;

	static uint32_t sizeOfGLType(uint32_t type)
	{
		switch (type)
		{
			case GL_FLOAT:		   return sizeof(float);
			case GL_UNSIGNED_INT:  return sizeof(uint32_t);
			case GL_UNSIGNED_BYTE: return sizeof(uint8_t);
		}
		assert(false);
		return 0;
	}
};

class VertexBufferLayout
{
private:
	std::vector<VertexBufferElement> _elements;
	uint32_t _stride;

public:
	VertexBufferLayout(void) : _elements(), _stride(0) {}

	void push(uint32_t count)
	{
		_elements.push_back({ GL_FLOAT, count, GL_FALSE });
		_stride += sizeof(float) * count;
	}

	inline const std::vector<VertexBufferElement> getElements(void) const { return _elements; }
	inline uint32_t getStride(void) const { return _stride; }
};

#endif