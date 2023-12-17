#include "TestTexture2D.h"

#include "Renderer.h"

#include "imgui/imgui.h"

namespace test
{
    TestTexture2D::TestTexture2D(void) :
        _translationA(glm::vec3(200, 200, 0)), _translationB(glm::vec3(400, 200, 0)), _vao(std::make_unique<VertexArray>()),
        _proj(glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f)), _view(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0)))
    {
        float positions[] =
        {
            -50.0f, -50.0f, 0.0f, 0.0f,
             50.0f, -50.0f, 1.0f, 0.0f,
             50.0f,  50.0f, 1.0f, 1.0f,
            -50.0f,  50.0f, 0.0f, 1.0f
        };

        uint32_t indices[] =
        {
            0, 1, 2,
            2, 3, 0
        };

        _vao = std::make_unique<VertexArray>();
        _vbo = std::make_unique<VertexBuffer>(positions, sizeof(positions));
        _ibo = std::make_unique<IndexBuffer>(indices, 6);

        VertexBufferLayout layout;
        layout.push<float>(2);
        layout.push<float>(2);
        _vao->addBuffer(*_vbo, layout);

        _shader = std::make_unique<Shader>("res/shaders/Basic.shader");
        _shader->bind();
        _shader->setUniform4f("u_Color", 0.8f, 0.3f, 0.8f, 1.0f);

        _texture = std::make_unique<Texture>("res/textures/Test.png");
        _texture->bind(0);
        _shader->setUniform1i("u_Texture", 0);
    }

    TestTexture2D::~TestTexture2D(void)
    {
    }

    void TestTexture2D::OnUpdate(float deltaTime)
    {
    }

    void TestTexture2D::OnRender(void)
    {
        glCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
        glCall(glClear(GL_COLOR_BUFFER_BIT));

        Renderer renderer;

        {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), _translationA);
            glm::mat4 mvp = _proj * _view * model;
            _shader->bind();
            _shader->setUniformMat4f("u_MVP", mvp);
            renderer.draw(*_vao, *_ibo, *_shader);
        }

        {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), _translationB);
            glm::mat4 mvp = _proj * _view * model;
            _shader->bind();
            _shader->setUniformMat4f("u_MVP", mvp);
            renderer.draw(*_vao, *_ibo, *_shader);
        }
    }

    void TestTexture2D::OnImGuiRender(void)
    {
        ImGui::SliderFloat3("Translation A", &_translationA.x, 0.0f, 960.0f);
        ImGui::SliderFloat3("Translation B", &_translationB.x, 0.0f, 960.0f);
        ImGui::Text("Average GUI FPS: %.2f", ImGui::GetIO().Framerate);
    }
}
