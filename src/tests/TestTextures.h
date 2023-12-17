#ifndef _TESTTEXTURES__H_
#define _TESTTEXTURES__H_

#include "Test.h"

#include "Model.h"
#include "Shader.h"
#include "Texture.h"

namespace test
{
	class TestTextures : public Test
	{
	private:
		glm::mat4 _projectionMatrix;
		glm::mat4 _viewMatrix;
		Model _model;
		Shader _shader;
		Texture _texture;
		Renderer* _rendererPtr;

	public:
		TestTextures(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix, const Model& baseModel, const std::string& shaderPath, const std::string& texturePath, Renderer* rendererPtr);
		~TestTextures(void);

		void OnUpdate(float deltaTime) override;
		void OnRender(void) override;
		void OnImGuiRender(void) override;
	};
}

#endif