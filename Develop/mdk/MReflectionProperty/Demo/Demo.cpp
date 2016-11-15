// Demo.cpp : 기본 프로젝트 파일입니다.

#include "stdafx.h"
#include "Form1.h"

using namespace Demo;

[STAThreadAttribute]
int main(array<System::String ^> ^args)
{
	// 컨트롤이 만들어지기 전에 Windows XP 시각 효과를 활성화합니다.
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false); 

	// 주 창을 만들어 실행합니다.
	Application::Run(gcnew Form1());
	return 0;
}
