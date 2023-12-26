#include "VertexBuffer.h"

#include "Renderer.h"

VertexBuffer::VertexBuffer(const void* data, uint32_t size)
{
    glCall(glGenBuffers(1, &_rendererID));
    glCall(glBindBuffer(GL_ARRAY_BUFFER, _rendererID));
    glCall(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
}

VertexBuffer::~VertexBuffer(void)
{
    glCall(glDeleteBuffers(1, &_rendererID));
}

void VertexBuffer::bind(void) const
{
    glCall(glBindBuffer(GL_ARRAY_BUFFER, _rendererID));
}

void VertexBuffer::unbind(void) const
{
    glCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
}