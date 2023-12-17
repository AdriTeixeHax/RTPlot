#ifndef _TESTTEXTURE2D__H_
#define _TESTTEXTURE2D__H_

#include "Test.h"
#include "glm/glm.hpp"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"
#include "Texture.h"

#include <memory>

namespace test
{
	class TestTexture2D : public Test
	{
	private:
		glm::vec3 _translationA;
		glm::vec3 _translationB;

		std::unique_ptr<VertexArray>  _vao;
		std::unique_ptr<VertexBuffer> _vbo;
		std::unique_ptr<IndexBuffer>  _ibo;
		std::unique_ptr<Shader>       _shader;
		std::unique_ptr<Texture>      _texture;

		glm::mat4 _proj;
		glm::mat4 _view;

	public:
		TestTexture2D(void);
		~TestTexture2D(void);

		void OnUpdate(float deltaTime) override;
		void OnRender(void) override;
		void OnImGuiRender(void) override;
	};
}

#endif