#ifndef _TEST__H_
#define _TEST__H_

#include <vector>
#include <string>
#include <functional>
#include <iostream>

namespace test
{
	class Test
	{
	public:
		Test(void) {}
		virtual ~Test(void) {}

		virtual void OnUpdate(float deltaTime) {}
		virtual void OnRender(void) {}
		virtual void OnImGuiRender(void) {}
	};

	class TestMenu : public Test
	{
	private:
		std::vector<std::pair<std::string, std::function<Test* (void)>>> _tests;
		Test*& _currentTest;

	public:
		TestMenu(Test*& currentTestPtr);
		~TestMenu(void) {}

		virtual void OnImGuiRender(void) override;

		template<typename Type>
		void registerTest(const std::string& name)
		{
			std::cout << "Registering test" << name << std::endl;
			_tests.push_back(std::make_pair(name, []() { return new Type(); }));
		}
	};
}

#endif