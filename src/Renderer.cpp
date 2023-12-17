#include "Renderer.h"

#include <iostream>

void glClearError(void)
{
    while (glGetError() != GL_NO_ERROR);
}

bool glLogCall(const char* function, const char* file, int line)
{
    while (uint32_t error = glGetError())
    {
        std::cout << "[OpenGL error]: (" << error << "): " << function << " " << file << ":" << line << std::endl;
        return false;
    }
    return true;
}

void Renderer::draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const
{
    shader.bind();
    va.bind();
    ib.bind();
    glCall(glDrawElements(GL_TRIANGLES, ib.getNumIndices(), GL_UNSIGNED_INT, nullptr));
}
