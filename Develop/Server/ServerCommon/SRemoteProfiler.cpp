#include "StdAfx.h"
#include "SRemoteProfiler.h"

SRemoteProfiler::SRemoteProfiler(void)
{
	m_pProfileBuffer = new MBuffer;
	m_pTempBuffer1 = new MBuffer;
	m_pTempBuffer2 = new MBuffer;

	m_pProfileBuffer->AllocateBuffer(1024*1024); //1024 kb∑Œ ≥À≥À»˜ ¿‚æ∆µ–¥Ÿ
	m_pTempBuffer1->AllocateBuffer(1024*1024); //1024 kb∑Œ ≥À≥À»˜ ¿‚æ∆µ–¥Ÿ
	m_pTempBuffer2->AllocateBuffer(1024*1024); //1024 kb∑Œ ≥À≥À»˜ ¿‚æ∆µ–¥Ÿ
}

SRemoteProfiler::~SRemoteProfiler(void)
{
	SAFE_DELETE(m_pProfileBuffer);
	SAFE_DELETE(m_pTempBuffer1);
	SAFE_DELETE(m_pTempBuffer2);
}

SRemoteProfiler& SRemoteProfiler::Instance()
{
	static SRemoteProfiler inst;
	return inst;
}

#define DEBUGGER_DIALOG_NAME				L"RS3Analyzer"
#define DEBUGGER_PROFILE_DIALOG_NAME		L"CPU Analyzer"
//debug info command 
#define DI_CM_PROFILEINFO	1
#define DI_CM_PROFILESTRING	2

void SRemoteProfiler::SendToProfiler( HWND SourceHwnd )
{
	HWND DestHwnd = FindWindow(NULL, DEBUGGER_DIALOG_NAME);
	if( NULL == DestHwnd )	return;
	if( NULL == FindWindow(NULL, DEBUGGER_PROFILE_DIALOG_NAME) ) return;

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

	// ±‚¡∏ «¡∑Œ∆ƒ¿œ∑Ø ¡§∫∏ √ ±‚»≠
	GetProfileManager().ResetCurrentInfo();
}