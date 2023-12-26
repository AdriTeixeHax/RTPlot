#include "VertexArray.h"
#include "VertexBufferLayout.h"
#include "Renderer.h"

VertexArray::VertexArray(void)
{
	glCall(glGenVertexArrays(1, &_rendererID));
}

VertexArray::~VertexArray(void)
{
	glCall(glDeleteVertexArrays(1, &_rendererID));
}

void VertexArray::addBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout)
{
	bind();
	vb.bind();
	const auto& elements = layout.getElements();
	uint32_t offset = 0;
	for (uint32_t i = 0; i < elements.size(); i++)
	{
		const auto& element = elements[i];
		glCall(glEnableVertexAttribArray(i));
		glCall(glVertexAttribPointer(i, element.count, element.type, element.normalized, layout.getStride(), (const void*)offset)); // Links the vertex array with the vertex buffer
		offset += element.count * VertexBufferElement::sizeOfGLType(element.type);
	}
}

void VertexArray::bind(void) const
{
	glCall(glBindVertexArray(_rendererID));
}

void VertexArray::unbind(void) const
{
	glCall(glBindVertexArray(0));
}
