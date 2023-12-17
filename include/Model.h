#ifndef _MODEL__H_
#define _MODEL__H_

#include "VertexArray.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"
#include <glm/glm.hpp>

class Model
{
private:
	uint32_t           _numVertices;
	uint32_t           _numTexCoords;
	uint32_t           _numIndices;
	float*             _positions;
	uint32_t*          _indices;
	VertexArray        _va;
	VertexBuffer       _vb;
	VertexBufferLayout _layout;
	IndexBuffer        _ib;
	glm::vec3          _position;

public:
	Model(uint32_t vertexSize, uint32_t textureSize, uint32_t numVertices, uint32_t numTexCoords, uint32_t numIndices, float* positions, uint32_t* indices, const glm::vec3& initialPos);
	virtual ~Model(void) {}

	const glm::vec3&   getPosition   (void) const { return  _position; }
	      glm::vec3*   getPositionPtr(void)       { return &_position; }
	const VertexArray& getVertexArray(void) const { return  _va; }
	const IndexBuffer& getIndexBuffer(void) const { return  _ib; }
};

#endif