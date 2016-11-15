#include "stdafx.h"
#include "RDebuggingInfo.h"
#include ".\Profile\profile.h"
#include ".\Profile\profileVisitor.h"
#include "Tlhelp32.h"

//#define VALIDATOR_PROFILING_DEBUGGING_INFO	

namespace rs3
{
	RPRofileDebuggingInfo::RPRofileDebuggingInfo( void )
	{
		m_pProfileBuffer = new MBuffer;
		m_pTempBuffer1 = new MBuffer;
		m_pTempBuffer2 = new MBuffer;

		m_pProfileBuffer->AllocateBuffer(1024*1024); //1024 kb·Î ³Ë³ËÈ÷ Àâ¾ÆµÐ´Ù
		m_pTempBuffer1->AllocateBuffer(1024*1024); //1024 kb·Î ³Ë³ËÈ÷ Àâ¾ÆµÐ´Ù
		m_pTempBuffer2->AllocateBuffer(1024*1024); //1024 kb·Î ³Ë³ËÈ÷ Àâ¾ÆµÐ´Ù
	}

	RPRofileDebuggingInfo::~RPRofileDebuggingInfo( void )
	{
		if( NULL != m_pProfileBuffer )
		{
			delete m_pProfileBuffer;
			m_pProfileBuffer = NULL;
		}

		if( NULL != m_pTempBuffer1 )
		{
			delete m_pTempBuffer1;
			m_pTempBuffer1 = NULL;
		}

		if( NULL != m_pTempBuffer2 )
		{
			delete m_pTempBuffer2;
			m_pTempBuffer2 = NULL;
		}
	}
	
	void RPRofileDebuggingInfo::SendOut( HWND DestHwnd,HWND SourceHwnd )
	{
	#ifdef VALIDATOR_PROFILING_DEBUGGING_INFO
		CValidatorProfileVisitor validatorVisitor;
		GetProfileManager().Traverse(&validatorVisitor,GetProfileManager().GetCallStackProfileRootNode());
		_ASSERT(validatorVisitor.IsValid());
	#endif

		if( NULL == FindWindow(NULL,DEBUGGER_PROFILE_DIALOG_NAME) ) return;

		MBuffer& ProfileBuffer = *m_pProfileBuffer;
		MBuffer& ProfileNameBuffer = *m_pProfileBuffer;
		COPYDATASTRUCT SendData;

		ProfileBuffer.Reset();
		m_pTempBuffer1->Reset();
		m_pTempBuffer2->Reset();

		GetProfileManager().MakeBinary(&ProfileBuffer,m_pTempBuffer1,m_pTempBuffer2);
		SendData.dwData = DI_CM_PROFILEINFO;
		SendData.cbData = ProfileBuffer.GetDataSize();
		SendData.lpData = ProfileBuffer.GetBufferPointer();
		SendMessage( DestHwnd,WM_COPYDATA,(WPARAM)SourceHwnd,(LPARAM)(LPVOID)&SendData);

		ProfileNameBuffer.Reset();
		GetProfileManager().MakeNameBinary(&ProfileNameBuffer);
		SendData.dwData = DI_CM_PROFILESTRING;
		SendData.cbData = ProfileNameBuffer.GetDataSize();
		SendData.lpData = ProfileNameBuffer.GetBufferPointer();
		SendMessage( DestHwnd,WM_COPYDATA,(WPARAM)SourceHwnd,(LPARAM)(LPVOID)&SendData);
	}



	//------------------------------------------------------------------------------------------------------------------------------------------------



	RPerformanceDebuggingInfo::RPerformanceDebuggingInfo( void )
	{
		m_Buffer.AllocateBuffer(1024*1024); //1024 kb·Î ³Ë³ËÈ÷ Àâ¾ÆµÐ´Ù
	}

	RPerformanceDebuggingInfo::~RPerformanceDebuggingInfo( void )
	{
	}

	void RPerformanceDebuggingInfo::SendOut( HWND DestHwnd,HWND SourceHwnd )
	{
		if( NULL == FindWindow(NULL,DEBUGGER_RESOURCE_DIALOG_NAME) ) return;
		
		MBuffer& ProfileBuffer = m_Buffer;
		COPYDATASTRUCT SendData;

		ProfileBuffer.Reset();
		REngine::GetResourceProfiler().GetMeshProfileInfoBinary(&ProfileBuffer);
		SendData.dwData = DI_CM_PERFORMANCEINFO;
		SendData.cbData = ProfileBuffer.GetDataSize();
		SendData.lpData = ProfileBuffer.GetBufferPointer();
		SendMessage( DestHwnd,WM_COPYDATA,(WPARAM)SourceHwnd,(LPARAM)(LPVOID)&SendData);
	}



	//------------------------------------------------------------------------------------------------------------------------------------------------



	RDebuggingInfo::RDebuggingInfo( void )
	{
	}	

	RDebuggingInfo::~RDebuggingInfo( void )
	{
	}

	void RDebuggingInfo::SendOut(HWND SourceHwnd)
	{
		HWND DebuggerHwnd = FindWindow(NULL,DEBUGGER_DIALOG_NAME);
		if( NULL == DebuggerHwnd )	return;

		m_ProfileDebuggingInfo.SendOut(DebuggerHwnd,SourceHwnd);
		
		m_PerformanceDebuggingInfo.SendOut(DebuggerHwnd,SourceHwnd);
	}



}