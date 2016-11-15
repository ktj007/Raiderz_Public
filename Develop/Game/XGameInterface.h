/*
	XGameInterface

	게임 인터페이스와 관련된 코드를 구성합니다.
	게임의 전반적인 UI를 다루기 보다는 게임의 전체 기능 등의 설정에 중점을 두고 작성합니다.
*/


#pragma once

#ifndef _XGAME_INTERFACE
#define _XGAME_INTERFACE


#include "XGameInterfaceOption.h"



/// class XGameInterface
class XGameInterface
{
// Member variables
public:
	XGameInterfaceOption		OPTIONS;



// Member functions
public:
	XGameInterface();
	virtual ~XGameInterface();

	static XGameInterface& GetInstance();
};




// Inline
inline XGameInterface&	XGetGameInterface()
{
	return XGameInterface::GetInstance();
}


#endif
