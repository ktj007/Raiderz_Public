#pragma once

#include "MSingleton.h"
#include "Mint4Game.h"
#include "MCommand.h"

//////////////////////////////////////////////////////////////////////////
// XMovableDebugData_Proc

struct XMovableDebugData_Proc
{
	float			fTimeDelta;
	float			fPosDelta;
	vec3			vPosDelta;
	uint32			nLastUpdate;

	XMovableDebugData_Proc() : fTimeDelta(0.0f), fPosDelta(0.0f), nLastUpdate(0), vPosDelta(vec3::ZERO) {}

	bool IsRedAlert();
	float GetPosDeltaPerSecond();
	void Update();
};

typedef map<wstring, XMovableDebugData_Proc*>	XMovableDebugProcMap;


//////////////////////////////////////////////////////////////////////////
// XMovableDebugData_Object

struct XMovableDebugData_Object
{
	MUID	UID;
	wstring	strName;
	uint32	nLastUpdateTime;
	vec3	vStartPos;
	vec3	vEndPos;

	// server pos
	vec3	vServerPos;
	vec3	vLocalPos;

	// post
	wstring	strPostMessageMsg;
	vec3	vPostPos;
	uint32	nLastPostTime;

	bool	bDeleteMe;

	XMovableDebugProcMap mapProcs;

	XMovableDebugData_Object(MUID uid);
	~XMovableDebugData_Object();

	XMovableDebugData_Proc* GetProc(const wchar_t* szProcName);
	void Update();
};

typedef map<MUID, XMovableDebugData_Object*>	XMovableDebugObjectMap;


//////////////////////////////////////////////////////////////////////////
// XMovableDebugData

class XMovableDebugData
{
public:
	XMovableDebugObjectMap		mapObjects;

	XMovableDebugData() {}
	~XMovableDebugData();
	XMovableDebugData_Object* GetObject(MUID uidObject);
};

//////////////////////////////////////////////////////////////////////////
// XMovableDebugReporter

class XMovableDebugReporter : public MSingleton<XMovableDebugReporter>
{
private:
	uint32					m_nLastRequestTime;
	bool					m_bCheckReportTime;
	XMovableDebugData*		m_pData;
	bool IsReport();
public:
	XMovableDebugReporter(XMovableDebugData* pData) : m_pData(pData), m_bCheckReportTime(false), m_nLastRequestTime(0) {}
	~XMovableDebugReporter() {}
	void CheckProc(MUID uidObject, const wchar_t* szProcName, float fDelta, vec3 vCurrentPos, vec3 vTo);
	void PostMessage(const wchar_t* szMsg, vec3 vPos);
	void Update();
	bool CheckReportTime() { return m_bCheckReportTime; }
};

//////////////////////////////////////////////////////////////////////////
// XMovableDebugReporter_Server

class XMovableDebugReporter_Server : public MSingleton<XMovableDebugReporter_Server>
{
private:
	XMovableDebugData*		m_pData;
	bool					m_bRequestReceived;
	uint32					m_nLastRequestTime;
public:
	XMovableDebugReporter_Server(XMovableDebugData* pData) : m_pData(pData), m_bRequestReceived(true), m_nLastRequestTime(0) {}
	~XMovableDebugReporter_Server() {}
	void Update();
	void OnRecvPosSync(TD_DEBUG_POS* pPos, int nSize);
};

//////////////////////////////////////////////////////////////////////////
// XMovableDebugger
//////////////////////////////////////////////////////////////////////////

class XMovableDebugger : public MSingleton<XMovableDebugger>
{
	friend class XMovableDebugReporter;
private:
	XMovableDebugReporter*	m_pReporter;
	XMovableDebugReporter_Server*	m_pServerReporter;
	XMovableDebugData		m_Data;
	bool IsVisible();
	void _PrintProcs(MDrawContext *pDC, int& x, int& y, XMovableDebugData_Object* pObjectProc);
	void _PrintObjectInfo(MDrawContext *pDC, int& x, int& y, XMovableDebugData_Object* pObjectProc);
	void PrintObjectProc(MDrawContext *pDC);

	void _DrawBackgroundBox( MDrawContext * pDC, int x, int y );
	void _UpdateData();
	void Update();
	void SetFont(MDrawContext *pDC);
public:
	XMovableDebugger();
	~XMovableDebugger();

	void Draw(MDrawContext *pDC);
	void DrawFrm( MDrawContext *pDC );
};




inline void __Report_Movable_Debug_PostMessage(const wchar_t* szMsg, vec3 vPos)
{
#ifndef _PUBLISH
	if (XMovableDebugReporter::GetInstancePtr())
	{
		XMovableDebugReporter::GetInstancePtr()->PostMessage(szMsg, vPos);
	}
#endif
}