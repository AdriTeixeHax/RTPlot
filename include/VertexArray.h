#ifndef _VERTEXARRAY__H_
#define _VERTEXARRAY__H_

#include "VertexBuffer.h"

class VertexBufferLayout;

class VertexArray
{
private:
	uint32_t _rendererID;

public:
	VertexArray(void);
	~VertexArray(void);

	void addBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout);
	void bind(void) const;
	void unbind(void) const;
};

#endif