#include "Renderer.h"

#include "imgui/imgui.h"
#include "TestTextures.h"

test::TestTextures::TestTextures(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix, const Model& baseModel, const std::string& shaderPath, const std::string& texturePath, Renderer* rendererPtr) :
    _projectionMatrix(projectionMatrix), _viewMatrix(viewMatrix), _model(baseModel), _shader(shaderPath), _texture(texturePath), _rendererPtr(rendererPtr)
{
    _shader.unbind();
    _shader.bind();

    _texture.bind(0);
    _shader.setUniform1i("u_Texture", 0);
}

test::TestTextures::~TestTextures(void)
{
}

void test::TestTextures::OnUpdate(float deltaTime)
{
}

void test::TestTextures::OnRender(void)
{
    glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), _model.getPosition());
    glm::mat4 mvp = _projectionMatrix * _viewMatrix * modelMatrix;
    _shader.bind();
    _shader.setUniformMat4f("u_MVP", mvp);
    _rendererPtr->draw(_model.getVertexArray(), _model.getIndexBuffer(), _shader);
}

void test::TestTextures::OnImGuiRender(void)
{
    ImGui::SliderFloat3("Translation", &_model.getPositionPtr()->x, 0.0f, 960.0f);
}
