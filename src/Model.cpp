#include "Model.h"

Model::Model
(
	uint32_t  vertexSize,
	uint32_t  textureSize,
	uint32_t  numVertices,
	uint32_t  numTexCoords,
	uint32_t  numIndices,
	float*    positions,
	uint32_t* indices,
	const glm::vec3& initialPos
) :
	_numVertices(numVertices),
	_numTexCoords(numTexCoords),
	_numIndices(numIndices),
	_positions(positions),
	_indices(indices),
	_va(),
	_vb(_positions, (_numVertices + _numTexCoords) * sizeof(float)),
	_layout(),
	_ib(_indices, _numIndices),
	_position(initialPos)
{
	if (vertexSize > 0)  _layout.push<float>(vertexSize);
	if (textureSize > 0) _layout.push<float>(textureSize);

	_va.addBuffer(_vb, _layout);

	_va.unbind();
	_vb.unbind();
	_ib.unbind();
}
