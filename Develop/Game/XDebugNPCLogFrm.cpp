#include "StdAfx.h"
#include "XDebugNPCLogFrm.h"
#include "XPost_GM.h"

XDebugNPCLogFrm::XDebugNPCLogFrm( const char* szName/*=NULL*/, MWidget* pParent/*=NULL*/, MListener* pListener/*=NULL*/ )
: mint3::MTextFrame(szName, pParent, pListener)
{
	AddControls();

	SetRect(10, 120, 430, 520);
//	SetOpacity(0.9f);
	SetText("NPC Log");

	unsigned long int nOutputFlags = MLOG_PROG;
	if (XCONST::DEBUG_LOG_COMMAND) 
	{
		nOutputFlags |= MLOG_FILE;
		mlog("Start logging\n");
	}
}

void XDebugNPCLogFrm::AddControls()
{
	m_pCommonBT = new MButton(NULL, this, this);
	m_pCommonBT->SetType(MBT_CHECKBTN);
	m_pCommonBT->SetText("common");
	m_pCommonBT->SetRect(5, 20, 60, 16);

	m_pUseTalentBT = new MButton(NULL, this, this);
	m_pUseTalentBT->SetType(MBT_CHECKBTN);
	m_pUseTalentBT->SetText("talent");
	m_pUseTalentBT->SetRect(70, 20, 60, 16);

	m_pMoveBT = new MButton(NULL, this, this);
	m_pMoveBT->SetType(MBT_CHECKBTN);
	m_pMoveBT->SetText("move");
	m_pMoveBT->SetRect(135, 20, 60, 16);

	m_pColtBT = new MButton(NULL, this, this);
	m_pColtBT->SetType(MBT_CHECKBTN);
	m_pColtBT->SetText("colt");
	m_pColtBT->SetRect(200, 20, 60, 16);
}

bool XDebugNPCLogFrm::OnCommand( MWidget* pWidget, MEventMsg nMsg, const char* szArgs/*=NULL*/ )
{
	if(MTextFrame::OnCommand(pWidget, nMsg, szArgs)) return true;

	if (pWidget == m_pCommonBT || pWidget == m_pUseTalentBT
		|| pWidget == m_pMoveBT || pWidget == m_pColtBT)
	{
		switch(nMsg)
		{
		case MEVENT_CLICK:
			{
				int nParam = 0;

 				((MButton*)pWidget)->SetCheck(!((MButton*)pWidget)->glueGetCheck());
 
 				if(m_pCommonBT->glueGetCheck())		SetBitSet(nParam, NPCLOG_COMMON);
 				if(m_pUseTalentBT->glueGetCheck())	SetBitSet(nParam, NPCLOG_USETALENT);
 				if(m_pMoveBT->glueGetCheck())		SetBitSet(nParam, NPCLOG_MOVE);
 				if(m_pColtBT->glueGetCheck())		SetBitSet(nParam, NPCLOG_COLT);
 
 				((MButton*)pWidget)->SetCheck(!((MButton*)pWidget)->glueGetCheck());
 
 				XPostDebugString(L"npc_monitor_show_type", nParam, L"");
				return true;
			}
		}
	}

	return false;
}

void XDebugNPCLogFrm::OnShow( bool bShow)
{
	{
		int nParam = 0;

		SetBitSet(nParam, NPCLOG_COMMON);
		SetBitSet(nParam, NPCLOG_USETALENT);
		SetBitSet(nParam, NPCLOG_MOVE);
		SetBitSet(nParam, NPCLOG_COLT);

		XPostDebugString(L"npc_monitor_show_type", nParam, L"");
	}

	MWidget::OnShow( bShow);

	if ( bShow == true)		BringToTop();
}

void XDebugNPCLogFrm::ClearMessage()
{
	ResetContent();
}

void XDebugNPCLogFrm::SetNPCLog(vector<TD_DEBUG_NPCLOG>& pVecTDDebugNPCLog)
{
	ClearMessage();

	m_VecNPCLog.clear();

	for each (TD_DEBUG_NPCLOG tdNPCLog in pVecTDDebugNPCLog)
	{
		AddLog(tdNPCLog.szText);
		m_VecNPCLog.push_back(tdNPCLog.szText);
	}
}

void XDebugNPCLogFrm::AddLog( const wchar_t* sLog )
{
	AddString(MLocale::ConvUTF16ToAnsi(sLog).c_str());
}

void XDebugNPCLogFrm::SaveLog()
{
	SYSTEMTIME stCurTime;
	GetLocalTime(&stCurTime);

	char temp[1024] = {0,};
	sprintf_s(temp, "%02d-%02d-%02d.%03d"
		, stCurTime.wHour
		, stCurTime.wMinute
		, stCurTime.wSecond
		, stCurTime.wMilliseconds);

	char szFullFileName[_MAX_PATH];
	sprintf_s(szFullFileName, "NPCLog %s.txt", temp);

	FILE* fp;
	errno_t err = fopen_s(&fp, szFullFileName, "wt");
	if ( 0 != err ) return;

	PrintLog(fp);

	fclose(fp);
}

void XDebugNPCLogFrm::PrintLog(FILE* fp)
{
	for each (wstring Log in m_VecNPCLog)
	{
		fputs(MLocale::ConvUTF16ToAnsi(Log.c_str()).c_str(), fp);
	}
}