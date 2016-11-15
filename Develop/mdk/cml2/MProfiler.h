#pragma once

#pragma push_macro("_SECURE_SCL")
#pragma push_macro("_HAS_ITERATOR_DEBUGGING")
#pragma warning ( disable : 4005)

#define _SECURE_SCL 0
#define _HAS_ITERATOR_DEBUGGING 0

////////////////////////////////////////////////////////////////////////////////////////////
// 기본 프로파일러는 GPG3권 (1.17)의 것을 사용합니다
/*

 호출 스택에 따라 계층적으로 기록되는 프로파일러이며
 _BEGINPROFILE(<name>) ~ _ENDPROFILE 을 한 쌍으로 사용하거나
 또는 PFC_THISFUNC 을 사용할 수 있습니다 ( 이 매크로는 PFC_B(<functionname>)을 자동으로 지정하고, 스택공간에서 벗어날 때 PFC_E을 실행합니다 )

 결과를 기록하기 위해서 PFC_FINALANALYSIS(<filename>)을 사용

 例1)
 int main()
 {
	PFC_B("main");
	...
	PFC_E;

	PFC_FINALANALYSIS("profile.log");
 }

 例2)
 bool run()
 {
	PFC_THISFUNC;
 }

 int main()
 {
	Initialize();
	do {
	 ...
	} while (run());

	PFC_FINALANALYSIS("profile.log");
 }

*/

#include "Profile/profile.h"
#include <set>
#include <string>
#include "RLib.h"

#ifdef _USE_PROFILER

	// ProFilerCallstack version Simple Macro
	#define PFC_B(_szProfileName)			GetProfileManager().Start_CallStackProfile(_szProfileName)
	#define PFC_E							GetProfileManager().Stop_CallStackProfile()
	#define PFC_THISFUNC					CCallStackProfileSample __CallStackProfile( __FUNCTION__ )
	#define PFC_BLOCK(x)					CCallStackProfileSample __CallStackProfile( x )
	#define PFC_RESET						GetProfileManager().Reset()
	#define PFC_FINALANALYSIS(_szFileName)	GetProfileManager().SaveFile(_szFileName)

	// ProFilerNoCallstack version Simple Macro
	#define PFN_B(_szProfileName)			GetProfileManager().Start_NoCallStackProfile(_szProfileName)
	#define PFN_E(_szProfileName)			GetProfileManager().Stop_NoCallStackProfile(_szProfileName)
	#define PFN_THISFUNC					CNoCallStackProfileSample __NoCallStackProfile( __FUNCTION__ )

#else

	#define PFC_B(_szProfileName)			;
	#define PFC_E							;
	#define PFC_THISFUNC					;
	#define PFC_BLOCK(x)					;
	#define PFC_RESET						;
	#define PFC_FINALANALYSIS(_szFileName)	;

	#define PFN_B(_szProfileName)			;
	#define PFN_E							;
	#define PFN_THISFUNC					;


#endif


#pragma pop_macro("_SECURE_SCL")
#pragma pop_macro("_HAS_ITERATOR_DEBUGGING")

