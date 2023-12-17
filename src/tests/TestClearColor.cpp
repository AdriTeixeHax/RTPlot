#include "TestClearColor.h"

#include "Renderer.h"

#include "imgui/imgui.h"

test::TestClearColor::TestClearColor(void) :
	_clearColor{ 0.2f, 0.3f, 0.8f, 1.0f }
{
}

test::TestClearColor::~TestClearColor(void)
{
}

void test::TestClearColor::OnUpdate(float deltaTime)
{
}

void test::TestClearColor::OnRender(void)
{
	glCall(glClearColor(_clearColor[0], _clearColor[1], _clearColor[2], _clearColor[3]));
	glCall(glClear(GL_COLOR_BUFFER_BIT));
}

void test::TestClearColor::OnImGuiRender(void)
{
	ImGui::ColorEdit4("Clear Color", _clearColor);
}
