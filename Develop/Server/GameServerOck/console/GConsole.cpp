#include "stdafx.h"
#include "GConsole.h"
#include "GSystem.h"
#include "GField.h"
#include "GFieldInfo.h"
#include "GSpawnManager.h"
#include "GBirdMonitor.h"
#include "GReporter.h"
#include "GEntityNPC.h"
#include "GSharedFieldMaster.h"
#include "GConst.h"
#include "GFieldMgr.h"
#include "GConfig.h"
#include "GCommandLogger.h"
#include "GValidatorSilo.h"
#include "SVNRevision.h"
#include "version.h"
#include "CAppHelper.h"
#include "GFieldInfoMgr.h"
#include "GRuntimeValidator.h"
#include "CSBaseValidator.h"
#include "GValidateLogger.h"
#include "GMasterServerFacade.h"
#include "GProxyField.h"
#include "GCmdHandler_App.h"
#include "SCommandTable_App.h"
#include "GCommandCenter.h"
#include "SProxyPlayerManager.h"
#include "SAppHelper.h"
#include "GCommandCenter.h"
#include "GMonitorHelper.h"
#include "SVNRevision.h"
#include "MLocale.h"
#include "MemPoolObjCounter.h"

class GAppCommandLogListener : public ICommandLogListener
{
public:
	virtual void OnLog(const wchar_t* szLog) 
	{
		if (GConsole::GetInstancePtr())
		{
			GConsole::GetInstancePtr()->LogCommand(szLog);
		}
	}
};




GConsole::GConsole(bool bMakeTrayIcon) : 
GBaseApplication(),
m_pRuntimeValidator(NULL),
m_pAppCmdHandler(NULL)
{

}

GConsole::~GConsole()
{
	Destroy();
}


void GConsole::OutputLogProc(const char* szText, MLogLevel nLevel)
{
	DWORD dwCurrentThreadID = GetCurrentThreadId();
	if (GConsole::GetInstance().IsMainThread(dwCurrentThreadID))
	{
		GConsole::GetInstance().OutputLog(MLocale::ConvAnsiToUCS2(szText).c_str(), nLevel);
	}
	else
	{
		if (NULL == gsys.pCommandCenter) return;
		if (NULL == gsys.pCommandCenter->GetNetServer()) return;

		MCommand* pNewCmd = gsys.pCommandCenter->NewCommand(MAPP_LOG_OUT_PUT);
		pNewCmd->AddParameter(NEW_WSTR(MLocale::ConvAnsiToUCS2(szText).c_str()));
		pNewCmd->AddParameter(NEW_INT(nLevel));

		pNewCmd->SetReceiverUID(gsys.pServer->GetUID());
		gsys.pCommandCenter->PostCommand(pNewCmd);

	}
}

void GConsole::OutputLog( const wchar_t* szText, MLogLevel nLevel )
{
	wstring strDateTime = SAppHelper::MakeCurrentDateTimeString();
	wstring strText = strDateTime + L" ";
	strText += szText;

	SAppHelper::OutputLogToConsole(nLevel, strText.c_str());
}

bool GConsole::OnCreate()
{
	AddAppCommandTable();

	MGetDefaultLogger()->SetCustomOutputCallback(GConsole::OutputLogProc);

	mlog("revision: %s, %s\n", SVN_REVISION, SVN_DATE);

	if (GBaseApplication::OnCreate() == false)
	{
		return false;
	}

	if (m_CommandLine.HasSwitch("exit"))
		return false;

	BirdM().SetListener(&m_BirdMonitorListener);

	m_pAppCmdHandler = new GCmdHandler_App(gsys.pCommandCenter->GetNetServer());
	gsys.pCommandCenter->SetLogger(new GCommandLogger(new GAppCommandLogListener()));

	if (GConst::DEBUG_CONSOLE_SHOW_GAMETICK && GConst::DEBUG_CONSOLE_GAMETICK_TICK > 0)
	{
		m_RegPrintGameTick.SetTickTime(float(GConst::DEBUG_CONSOLE_GAMETICK_TICK));
		m_RegPrintGameTick.Start();
	}

	return true;
}

bool GConsole::OnUpdate( float fDelta )
{
	if (!GBaseApplication::OnUpdate(fDelta))
		return false;

	UpdatePrintGameTick(fDelta);
	
	return true;
}

void GConsole::OnDestroy()
{
	if (IsRunForTest())
	{
		if (m_pRuntimeValidator)
		{
			m_pRuntimeValidator->Stop();
			SAFE_DELETE(m_pRuntimeValidator);
		}	
	}

	SAFE_DELETE(m_pAppCmdHandler);

	GBaseApplication::OnDestroy();
}

void GConsole::_LogCommand( const wchar_t* szMsg )
{
	wprintf_s(szMsg);
}

void GConsole::_Quit(int nRetCode)
{
	CloseServer();
}

void GAppBirdMonitorListener::OnOutput( int nLine, const wchar_t* szMsg )
{
	if (GConsole::GetInstancePtr())
	{

	}
}

bool GConsole::_ValidateResouces()
{
	return GValidateResouces();
}


void GConsole::_RunRuntimeValidator()
{
	m_pRuntimeValidator = new GRuntimeValidator();
	m_pRuntimeValidator->Start();
}

void GConsole::UpdatePrintGameTick( float fDelta )
{
	if (gsys.pServer == NULL) return;

	if (GConst::DEBUG_CONSOLE_SHOW_GAMETICK && GConst::DEBUG_CONSOLE_GAMETICK_TICK > 0)
	{
		if (m_RegPrintGameTick.IsReady(fDelta))
		{
			GServerInfo& serverInfo = gsys.pServer->GetServerInfo();

			// 해당 필드 NPC 수
			uint32 nAllNPCCount = GMonitorHelper::GetTotalNPCCount();

			wchar_t sMemory[256] = L"";
			if (IsRunForTest())
			{
				float fMemMB = SAppHelper::CalcMemorySize(::GetCurrentProcessId()) / 1024.0f / 1024.0f;

				swprintf_s(sMemory, L", Memory = %.1fMB", fMemMB);
			}

			wchar_t sBuf[1024];
			swprintf_s(sBuf, L"FPS = %d, Link = %d, Player = %d, NPC = %d, Recv BPS = %u, Send BPS = %u, TotalPacketBytes(R:%I64d , S:%I64d), DisConn(A:%d , Dpl:%d , Cht:%d, Packet:%d , HB:%d) TotalDBTask = %u%s\n", 
				serverInfo.nFPS, 
				serverInfo.nLinkCount,
				serverInfo.nNowPlayer,
				nAllNPCCount,
				serverInfo.nRecvBPS, 
				serverInfo.nSendBPS,
				serverInfo.nTotalRecvBytes,
				serverInfo.nTotalSendBytes,
				serverInfo.nDisconnCountAll,
				serverInfo.nDisconnCountByDup,
				serverInfo.nDisconnCountByCheat,
				gsys.pServer->GetNetServer()->GetDisconnCountByWrongPacket(),
				gsys.pServer->GetNetServer()->GetDisconnCountByHearBeat(),
				serverInfo.nAsyncDBTotalTask,
				sMemory
			);

			gsys.pServer->GetServerInfo().ResetDisconnCount();
			gsys.pServer->GetNetServer()->ResetDisconnCount();

			mlog(sBuf);
			::SetConsoleTitle(sBuf);

			if (true == GConfig::m_bLogPoolCount)
			{
				class DumpPoolCnts : public MemPoolObjCounter::IVisitor
				{
				public:
					DumpPoolCnts() : m_totalSize(0)
					{
						mlog("pool cnts... --------------------------------------------------\n");
					}

					~DumpPoolCnts()
					{
						mlog("checked memory total size : %d\n", m_totalSize);
						mlog("---------------------------------------------------------------\n");
					}

				private:
					void OnVisit(const std::string& objName,const int& objSize,const int& objCnt)
					{
						mlog("%5d\t""TotalSize : %10d\t""%s\n", objCnt, objCnt * objSize, objName.c_str());
						m_totalSize += objCnt * objSize;
					}

				private:
					int m_totalSize;
				};

				MemPoolObjCounter::Instance().Visit(DumpPoolCnts());
			}
		}
	}
}