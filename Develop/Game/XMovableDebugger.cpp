#include "stdafx.h"
#include "XMovableDebugger.h"
#include "XScreenDebugger.h"
#include "XNonPlayer.h"

#define TOLER_MOVABLE_PROC_DELTA_POS		100.0f
#define TOLER_SERVER_POS_SYNC				100.0f
#define RED_ALERT_TIME						5000

#define __NEXTLINE_MOVABLE_DEBUGGER			{y+=20;}

//////////////////////////////////////////////////////////////////////////
// XMovableDebugData_Proc

void XMovableDebugData_Proc::Update()
{
	if (XGetNowTime() - nLastUpdate > RED_ALERT_TIME)
	{
		fTimeDelta = 0.0f;
		fPosDelta = 0.0f;
		nLastUpdate = 0;
	}
}

bool XMovableDebugData_Proc::IsRedAlert()
{
	if (fTimeDelta == 0.0f) return false;

	if (GetPosDeltaPerSecond() > TOLER_MOVABLE_PROC_DELTA_POS)
	{
		uint32 nNowTime = XGetNowTime();
		if (nNowTime - nLastUpdate < RED_ALERT_TIME)
		{
			return true;
		}
	}
	return false;
}

float XMovableDebugData_Proc::GetPosDeltaPerSecond()
{
	if (fTimeDelta == 0.0f) return 0.0f;
	return fPosDelta / fTimeDelta;
}

//////////////////////////////////////////////////////////////////////////
// XMovableDebugData_Object

XMovableDebugData_Object::XMovableDebugData_Object(MUID uid)
: UID(uid)
, nLastUpdateTime(0)
, vStartPos(vec3::ZERO)
, vEndPos(vec3::ZERO)
, vLocalPos(vec3::ZERO)
, vServerPos(vec3::ZERO)
, vPostPos(vec3::ZERO)
, nLastPostTime(0)
, bDeleteMe(false)
{
	if (gg.omgr)
	{
		XObject* pObject = gg.omgr->FindObject(uid);
		if (pObject)
		{
			strName = pObject->GetName();
		}
	}
}

XMovableDebugData_Object::~XMovableDebugData_Object()
{
	for (XMovableDebugProcMap::iterator itor = mapProcs.begin();
		itor != mapProcs.end(); ++itor)
	{
		delete (*itor).second;
	}
	mapProcs.clear();
}

void XMovableDebugData_Object::Update()
{
	uint32 nNowTime = XGetNowTime();
	if (nNowTime - nLastUpdateTime > 10000)
	{
		bDeleteMe = true;
	}

	if (gg.omgr)
	{
		XObject* pObject = gg.omgr->FindObject(UID);
		if (pObject)
		{
			vLocalPos = pObject->GetPosition();
		}
		else
		{
			bDeleteMe = true;
		}
	}
}

XMovableDebugData_Proc* XMovableDebugData_Object::GetProc( const wchar_t* szProcName )
{
	XMovableDebugProcMap::iterator itor = mapProcs.find(wstring(szProcName));
	if (itor == mapProcs.end()) 
	{
		XMovableDebugData_Proc* pNewProc = new XMovableDebugData_Proc();
		mapProcs.insert(XMovableDebugProcMap::value_type(wstring(szProcName), pNewProc));
		return pNewProc;
	}

	return (*itor).second;
}

//////////////////////////////////////////////////////////////////////////
// XMovableDebugData

XMovableDebugData::~XMovableDebugData()
{
	for (XMovableDebugObjectMap::iterator itor = mapObjects.begin();
		itor != mapObjects.end(); ++itor)
	{
		delete (*itor).second;
	}
	mapObjects.clear();
}

XMovableDebugData_Object* XMovableDebugData::GetObject( MUID uidObject )
{
	XMovableDebugObjectMap::iterator itor = mapObjects.find(uidObject);
	if (itor == mapObjects.end())
	{
		XMovableDebugData_Object* pNewObject = new XMovableDebugData_Object(uidObject);
		mapObjects.insert(XMovableDebugObjectMap::value_type(uidObject, pNewObject));
		return pNewObject;
	}

	return (*itor).second;
}


//////////////////////////////////////////////////////////////////////////
// XMovableDebugReporter

void XMovableDebugReporter::CheckProc( MUID uidObject, const wchar_t* szProcName, float fDelta, vec3 vCurrentPos, vec3 vTo )
{
#ifdef _PUBLISH
	return;
#endif

	if (gg.omgr && gg.omgr->GetCount(ETID_PLAYER) > 1)
	{
		XObject* pObj = gg.omgr->FindObject(uidObject);
		if (pObj && pObj->GetEntityType() != ETID_PLAYER)
		{
			return;
		}
	}

	XMovableDebugData_Object* pObjectProc = m_pData->GetObject(uidObject);
	XMovableDebugData_Proc* pProc = pObjectProc->GetProc(szProcName);

	pObjectProc->nLastUpdateTime = XGetNowTime();

	float fPosDelta = vTo.Length();

	if (pProc->IsRedAlert() && fDelta != 0.0f) 
	{
		float fPosDeltaPerSecond = fPosDelta / fDelta;
		if (fPosDeltaPerSecond < pProc->GetPosDeltaPerSecond())
		{
			return;
		}
	}

	pProc->fTimeDelta = fDelta;
	pProc->fPosDelta = fPosDelta;
	pProc->vPosDelta = vTo;
	if (fDelta != 0.0f)
	{
		pProc->vPosDelta.x = pProc->vPosDelta.x / fDelta;
		pProc->vPosDelta.y = pProc->vPosDelta.y / fDelta;
		pProc->vPosDelta.z = pProc->vPosDelta.z / fDelta;
	}
	pProc->nLastUpdate = XGetNowTime();
}

bool XMovableDebugReporter::IsReport()
{
	if (XMovableDebugger::GetInstancePtr())
	{
		return XMovableDebugger::GetInstancePtr()->IsVisible();
	}
	return false;
}

void XMovableDebugReporter::PostMessage( const wchar_t* szMsg, vec3 vPos )
{
	MUID uid = XGetMyUID();
	XMovableDebugData_Object* pObject = m_pData->GetObject(uid);
	if (pObject)
	{
		pObject->nLastPostTime = XGetNowTime();
		pObject->strPostMessageMsg = szMsg;
		pObject->vPostPos = vPos;
	}
}

void XMovableDebugReporter::Update()
{
	m_bCheckReportTime = false;
	uint32 nNowTime = XGetNowTime();
	if (nNowTime- m_nLastRequestTime > 1000)
	{
		m_nLastRequestTime = nNowTime;
		m_bCheckReportTime = true;
	}
}

//////////////////////////////////////////////////////////////////////////
// XMovableDebugReporter_Server

void XMovableDebugReporter_Server::Update()
{
#ifdef _PUBLISH
	return;
#endif

	uint32 nNowTime = XGetNowTime();
	if (nNowTime - m_nLastRequestTime > 5000)
	{
		m_nLastRequestTime = nNowTime;
		m_bRequestReceived = true;
	}

	if (m_bRequestReceived == true)
	{
		vector<MUID> vectorUID;

		for (XMovableDebugObjectMap::iterator itor = m_pData->mapObjects.begin();
			itor != m_pData->mapObjects.end(); ++itor)
		{
			MUID uid = (*itor).first;
			vectorUID.push_back(uid);
		}

		if (!vectorUID.empty())
		{
			XPOSTCMD4(MC_REQUEST_DEBUG_STRING, 
				MCommandParameterWideString(L"pos_sync"), 
				MCommandParameterInt(0), 
				MCommandParameterWideString(L""), 
				MCommandParameterBlob(&(vectorUID[0]), sizeof(MUID), vectorUID.size()));


			m_bRequestReceived = false;
		}
	}
}

void XMovableDebugReporter_Server::OnRecvPosSync(TD_DEBUG_POS* pPos, int nSize)
{
#ifdef _PUBLISH
	return;
#endif

	m_bRequestReceived = true;

	for (int i = 0; i < nSize; i++)
	{
		TD_DEBUG_POS* posInfo = &pPos[i];

		XMovableDebugData_Object* pObject = m_pData->GetObject(posInfo->uidTarget);
		pObject->vServerPos = posInfo->vPoint;
	}
}

//////////////////////////////////////////////////////////////////////////
// XMovableDebugger

XMovableDebugger::XMovableDebugger()
{
	m_pReporter = new XMovableDebugReporter(&m_Data);
	m_pServerReporter = new XMovableDebugReporter_Server(&m_Data);
}

XMovableDebugger::~XMovableDebugger()
{
	SAFE_DELETE(m_pReporter);
	SAFE_DELETE(m_pServerReporter);
}

bool XMovableDebugger::IsVisible()
{
	if (!XScreenDebugger::GetInstance()) return false;
	if (XScreenDebugger::GetInstance()->IsVisible() == false ||
		XScreenDebugger::GetInstance()->GetSelLayer() != XScreenDebugger::LAYER8)
	{
		return false;
	}

	return true;
}

void XMovableDebugger::Draw( MDrawContext *pDC )
{
#ifdef _PUBLISH
	return;
#endif

	if (IsVisible() == false) return;

	Update();

	SetFont(pDC);

	pDC->SetColor(MCOLOR(XCONST::SCREEN_DEBUG_FONT_COLOR));
	pDC->Text(10, 10, "MovableDebugger Test");

	PrintObjectProc(pDC);
}

void XMovableDebugger::DrawFrm( MDrawContext *pDC )
{
	Update();

	PrintObjectProc(pDC);
}

void XMovableDebugger::_PrintProcs( MDrawContext *pDC, int& x, int& y, XMovableDebugData_Object* pObject )
{
	wchar_t text[256];

	pDC->SetColor(MCOLOR(0xFF0000FF));
	swprintf_s(text, L"%s", pObject->strName.c_str());
	pDC->Text(x, y, MLocale::ConvUTF16ToAnsi(text).c_str());

	__NEXTLINE_MOVABLE_DEBUGGER
	
	for (XMovableDebugProcMap::iterator itor = pObject->mapProcs.begin();
		itor != pObject->mapProcs.end(); ++itor)
	{
		XMovableDebugData_Proc* pProc = (*itor).second;
		pProc->Update();

		if (pProc->IsRedAlert())
		{
			pDC->SetColor(MCOLOR(0xFFFF0000));
		}
		else
		{
			pDC->SetColor(MCOLOR(XCONST::SCREEN_DEBUG_FONT_COLOR));
		}

		pDC->Text(x, y, MLocale::ConvUTF16ToAnsi(((*itor).first).c_str()).c_str());

		swprintf_s(text, L"%.2f", pProc->GetPosDeltaPerSecond());
		pDC->Text(x + 120, y, MLocale::ConvUTF16ToAnsi(text).c_str());

		__NEXTLINE_MOVABLE_DEBUGGER

		if (pProc->IsRedAlert())
		{
			swprintf_s(text, L" x=%.1f", pProc->vPosDelta.x);
			pDC->Text(x + 120, y, MLocale::ConvUTF16ToAnsi(text).c_str());	__NEXTLINE_MOVABLE_DEBUGGER

			swprintf_s(text, L" y=%.1f", pProc->vPosDelta.y);
			pDC->Text(x + 120, y, MLocale::ConvUTF16ToAnsi(text).c_str());	__NEXTLINE_MOVABLE_DEBUGGER
			swprintf_s(text, L" z=%.1f", pProc->vPosDelta.z);
			pDC->Text(x + 120, y, MLocale::ConvUTF16ToAnsi(text).c_str());	__NEXTLINE_MOVABLE_DEBUGGER
		}
	}
}

void XMovableDebugger::_PrintObjectInfo( MDrawContext *pDC, int& x, int& y, XMovableDebugData_Object* pObjectProc )
{
	char text[256];

	pDC->SetColor(MCOLOR(XCONST::SCREEN_DEBUG_FONT_COLOR));

	pDC->Text(x, y, "LocalPos");
	sprintf_s(text, "%4.1f %4.1f %4.1f", pObjectProc->vLocalPos.x, pObjectProc->vLocalPos.y, pObjectProc->vLocalPos.z);
	pDC->Text(x + 120, y, text);

	__NEXTLINE_MOVABLE_DEBUGGER

	pDC->Text(x, y, "ServerPos");
	sprintf_s(text, "%4.1f %4.1f %4.1f", pObjectProc->vServerPos.x, pObjectProc->vServerPos.y, pObjectProc->vServerPos.z);
	pDC->Text(x + 120, y, text);

	__NEXTLINE_MOVABLE_DEBUGGER

	float fPosDiff = pObjectProc->vLocalPos.DistanceTo(pObjectProc->vServerPos);

	if (fPosDiff > TOLER_SERVER_POS_SYNC)
	{
		pDC->SetColor(MCOLOR(0xFFFF8020));
	}

	pDC->Text(x, y, "PosDiff");
	sprintf_s(text, "%.1f", fPosDiff);
	pDC->Text(x + 120, y, text);

	__NEXTLINE_MOVABLE_DEBUGGER

	if (pObjectProc->nLastPostTime > 0)
	{
		pDC->SetColor(MCOLOR(XCONST::SCREEN_DEBUG_FONT_COLOR));

		pDC->Text(x, y, "PostPos");
		sprintf_s(text, "%4.1f %4.1f %4.1f", pObjectProc->vPostPos.x, pObjectProc->vPostPos.y, pObjectProc->vPostPos.z);
		pDC->Text(x + 120, y, text);

		__NEXTLINE_MOVABLE_DEBUGGER
		pDC->Text(x, y, "PostMsg");
		sprintf_s(text, "%s", MLocale::ConvUTF16ToAnsi(pObjectProc->strPostMessageMsg.c_str()).c_str());
		pDC->Text(x + 120, y, text);

		__NEXTLINE_MOVABLE_DEBUGGER
			pDC->Text(x, y, "PostTime");
		sprintf_s(text, "%.1fs ago", (XGetNowTime() - pObjectProc->nLastPostTime) / 1000.0f );
		pDC->Text(x + 120, y, text);
	}
}

void XMovableDebugger::PrintObjectProc( MDrawContext *pDC )
{
	const int MAX_OBJECT = 6;

	MPOINT pt[MAX_OBJECT];
	pt[0] = MPOINT(20, 30);		pt[1] = MPOINT(320, 30);	pt[2] = MPOINT(620, 30);
	pt[3] = MPOINT(20, 300);	pt[4] = MPOINT(320, 300);	pt[5] = MPOINT(620, 300);

	int i = 0;
	for (XMovableDebugObjectMap::iterator itor = m_Data.mapObjects.begin();
		itor != m_Data.mapObjects.end(); ++itor)
	{
		int x = pt[i].x;
		int y = pt[i].y;

		_DrawBackgroundBox(pDC, x, y);

		XMovableDebugData_Object* pObject = (*itor).second;
		_PrintProcs(pDC, x, y, pObject);

		_PrintObjectInfo(pDC, x, y, pObject);

		i++;

		if (i >= MAX_OBJECT) break;
	}
}

void XMovableDebugger::Update()
{
	m_pServerReporter->Update();
	m_pReporter->Update();

	_UpdateData();
}

void XMovableDebugger::_UpdateData()
{
	for (XMovableDebugObjectMap::iterator itor = m_Data.mapObjects.begin();
		itor != m_Data.mapObjects.end(); )
	{
		XMovableDebugData_Object* pObject = (*itor).second;
		pObject->Update();
		
		if (pObject->bDeleteMe)
		{
			delete pObject;
			itor = m_Data.mapObjects.erase(itor);
		}
		else
		{
			++itor;
		}
	}
}

void XMovableDebugger::SetFont(MDrawContext *pDC)
{
	pDC->SetFont(global.ui->GetDefFont());
	//if (pDC->SetFont("fnt_DefUI") == NULL)
	//{
	//	pDC->SetFont(global.ui->GetDefFont());
	//}
}

void XMovableDebugger::_DrawBackgroundBox( MDrawContext * pDC, int x, int y )
{
	pDC->SetColor(MCOLOR(0x99000000));
	MRECT rt = MRECT(x-5, y-5, 250, 200);
	pDC->FillRectangle(rt);
}








