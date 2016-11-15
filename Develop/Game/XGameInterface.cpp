/*
	XGameInterface

	게임 인터페이스와 관련된 코드를 구성합니다.
	게임의 전반적인 UI를 다루기 보다는 게임의 전체 기능 등의 설정에 중점을 두고 작성합니다.
*/


#include "stdafx.h"
#include "XGameInterface.h"




XGameInterface::XGameInterface()
{
}


XGameInterface::~XGameInterface()
{
}

XGameInterface& XGameInterface::GetInstance()
{
	static XGameInterface	instance;
	return instance;
}
