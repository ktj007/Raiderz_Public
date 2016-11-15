#pragma once

//class MBuffer;
#include "MBuffer.h"
//#include "RLib.h"

namespace rs3 
{

// Version
// 2011.04.05 - v1.0 : Resource Analyzer 에 GPU Time 기능 추가.
#define DEBUGGER_DIALOG_NAME				"RS3Analyzer v1.0"
#define DEBUGGER_PROFILE_DIALOG_NAME		"CPU&GPU Analyzer"
#define DEBUGGER_RESOURCE_DIALOG_NAME		"Resource Analyzer"

//debug info command 
#define DI_CM_PROFILEINFO	1
#define DI_CM_PROFILESTRING	2

#define DI_CM_PERFORMANCEINFO	100


	//------------------------------------------------------------------------------------------------------------------------------------------------
	class RS_API	RPRofileDebuggingInfo
	{
	public:
		RPRofileDebuggingInfo(void);
		~RPRofileDebuggingInfo(void);

	public:
		void	SendOut(HWND DestHwnd,HWND SourceHwnd);

	private://ProfileInfo관련
		MBuffer*	m_pProfileBuffer;
		MBuffer*	m_pTempBuffer1;
		MBuffer*	m_pTempBuffer2;
	};

	//------------------------------------------------------------------------------------------------------------------------------------------------
	class RS_API	RPerformanceDebuggingInfo
	{
	public:
		RPerformanceDebuggingInfo(void);
		~RPerformanceDebuggingInfo(void);

	public:
		void	SendOut(HWND DestHwnd,HWND SourceHwnd);

	private:
		MBuffer m_Buffer;
	};

	
	//------------------------------------------------------------------------------------------------------------------------------------------------
	class RS_API	RDebuggingInfo
	{
	public:
		RDebuggingInfo(void);
		~RDebuggingInfo(void);

	public:
		void	SendOut(HWND SourceHwnd);

	private:
		RPRofileDebuggingInfo		m_ProfileDebuggingInfo;		//ProfileInfo관련
		RPerformanceDebuggingInfo	m_PerformanceDebuggingInfo;	//PerformanceInfo관련
	};


}