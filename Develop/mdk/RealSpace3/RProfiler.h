#pragma once

#include "MProfiler.h"
#include "RGPUProfiler.h"
//#include "RProfile.h"

//MProfiler.h랑 완전 동일한 매크로
//하지만 MProfiler.h는 엔진외에서도 쓴다. 그래서 엔진과 엔진외의 프로그램의 프로파일링을 분리하고자 똑같은 매크로를 이름만 바꿈
//그렇기 때문에 MProfiler.h에서 프로파일링 매크로를 꺼서 엔진외의 프로그램의 프로파일링을 끄고,
//RProfiler.h에서는 프로파일링 매크로를 켜서 엔진의 프로파일링을 킬 수 있다. 결국 엔진의 프로파일링 정보만 얻어옴

#ifdef _USE_PROFILER
#define _USE_ENGINE_CPU_PROFILE
#endif 

//#define _USE_ENGINE_RESOURCE_PROFILE
//#define _USE_ENGINE_CPU_PROFILE		// CPU
//#define _USE_ENGINE_GPU_PROFILE		// GPU

#ifdef _USE_ENGINE_CPU_PROFILE

// ProFilerCallstack version Simple Macro
#define RPFC_B(_szProfileName)			GetProfileManager().Start_CallStackProfile(_szProfileName)
#define RPFC_E							GetProfileManager().Stop_CallStackProfile()
#define RPFC_THISFUNC					CCallStackProfileSample __CallStackProfile( __FUNCTION__ )
#define RPFC_BLOCK(x)					CCallStackProfileSample __CallStackProfile( x )
#define RPFC_RESET						GetProfileManager().Reset()
#define RPFC_REST_CURRENTINFO			GetProfileManager().ResetCurrentInfo()
#define RPFC_FINALANALYSIS(_szFileName)	GetProfileManager().SaveFile(_szFileName)

// ProFilerNoCallstack version Simple Macro
#define RPFN_B(_szProfileName)			GetProfileManager().Start_NoCallStackProfile(_szProfileName)
#define RPFN_E(_szProfileName)			GetProfileManager().Stop_NoCallStackProfile(_szProfileName)
#define RPFN_THISFUNC					CNoCallStackProfileSample __NoCallStackProfile( __FUNCTION__ )

#else

#define RPFC_B(_szProfileName)			;
#define RPFC_E							;
#define RPFC_THISFUNC					;
#define RPFC_BLOCK(x)					;
#define RPFC_RESET						;
#define RPFC_REST_CURRENTINFO			;
#define RPFC_FINALANALYSIS(_szFileName)	;

#define RPFN_B(_szProfileName)			;
#define RPFN_E							;
#define RPFN_THISFUNC					;

#endif

#ifdef _USE_ENGINE_GPU_PROFILE

#define RPFC_GPU_RESET					GetGPUProfiler().Create()
#define RPFC_GPU_CALL					GetGPUProfiler().Call( GetProfileManager().GetCallStackProfileCurrentNode() )
//#define RPFN_GPU_THISFUNC				rs3::RGPUProfileSample __GPUProfile( __FUNCTION__ )

#else

#define RPFC_GPU_RESET					;
#define RPFC_GPU_CALL					;
#define RPFN_GPU_THISFUNC				;

#endif