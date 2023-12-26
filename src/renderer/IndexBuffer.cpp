#include "IndexBuffer.h"

#include "Renderer.h"

IndexBuffer::IndexBuffer(const uint32_t* data, uint32_t numIndices)
    : _numIndices(numIndices)
{
    glCall(glGenBuffers(1, &_rendererID));
    glCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _rendererID));
    glCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, _numIndices * sizeof(uint32_t), data, GL_STATIC_DRAW));
}

IndexBuffer::~IndexBuffer(void)
{
    glCall(glDeleteBuffers(1, &_rendererID));
}

void IndexBuffer::bind(void) const
{
    glCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _rendererID));
}

void IndexBuffer::unbind(void) const
{
    glCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}