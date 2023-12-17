#ifndef _TESTCLEARCOLOR__H_
#define _TESTCLEARCOLOR__H_

#include "Test.h"

namespace test
{
	class TestClearColor : public Test
	{
	private:
		float _clearColor[4];

	public:
		TestClearColor(void);
		~TestClearColor(void);

		void OnUpdate(float deltaTime) override;
		void OnRender(void) override;
		void OnImGuiRender(void) override;
	};
}

#endif