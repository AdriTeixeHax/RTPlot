#ifndef _INDEXBUFFER__H_
#define _INDEXBUFFER__H_

#include <stdint.h>

class IndexBuffer
{
private:
	uint32_t _rendererID;
	uint32_t _numIndices;

public:
	IndexBuffer(const uint32_t* data, uint32_t numIndices);
	~IndexBuffer(void);

	uint32_t getNumIndices(void) const { return _numIndices; }

	void bind  (void) const;
	void unbind(void) const;
};

#endif