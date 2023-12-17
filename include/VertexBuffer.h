#ifndef _VERTEXBUFFER__H_
#define _VERTEXBUFFER__H_

#include <stdint.h>

class VertexBuffer
{
private:
	uint32_t _rendererID;

public:
	VertexBuffer(const void* data, uint32_t size);
	~VertexBuffer(void);

	void bind  (void) const;
	void unbind(void) const;
};

#endif