#include "Test.h"

#include "imgui/imgui.h"

test::TestMenu::TestMenu(Test*& currentTestPtr) :
	_currentTest(currentTestPtr)
{
}

void test::TestMenu::OnImGuiRender(void)
{
	for (auto& test : _tests)
	{
		if (ImGui::Button(test.first.c_str()))
			_currentTest = test.second();
	}
}
