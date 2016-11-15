#include "stdafx.h"
#include "XDummyUI.h"
#include "XDummyMaster.h"
#include "XDummyReporter.h"
#include "MTabCtrl.h"
#include "XWidgetNameDef.h"

XDummyUI::XDummyUI() : m_pAgentStatusPaint(NULL), m_pMainPaint(NULL), m_pStatisticsPaint(NULL), m_pData(NULL)
{
	XBirdClient::m_bPrintCommandLog = true;
	m_pTabCtrlListener = new MTabCtrlListener();
}

XDummyUI::~XDummyUI()
{
	SAFE_DELETE(m_pTabCtrlListener);
}

void XDummyUI::Create(XDummyData* pData)
{
	m_pData = pData;

	XMintLoader loader;
	loader.LoadFromFile("dev/DummyClient/DummyClientUI.xml");

	global.ui->ShowSheet(L"DummyClient");

	InitUI();
}

void XDummyUI::Destroy()
{

}

void XDummyUI::Reload()
{
	global.ui->Reload(PATH_UI L"DummyClient.xml");
	InitUI();
}

void XDummyUI::InitUI()
{
	MWidget* pMainPanelWidget = global.mint->FindWidget(WIDGET_NAME_MAIN_PAGE_PANEL);
	MWidget* pCommandLogPanelWidget = global.mint->FindWidget(WIDGET_NAME_COMMAND_LOG_PAGE_PANEL);
	MWidget* pAgentStatusPanelWidget = global.mint->FindWidget(WIDGET_NAME_AGENT_STATUS_PAGE_PANEL);
	MWidget* pStatisticsPanelWidget = global.mint->FindWidget(WIDGET_NAME_STATISTICS_PAGE_PANEL);

	if (global.ui == NULL || 
		pMainPanelWidget == NULL || 
		pCommandLogPanelWidget == NULL || 
		pAgentStatusPanelWidget == NULL ||
		pStatisticsPanelWidget == NULL)
	{
		_ASSERT(0);
		return;
	}

	if (global.ui->GetCommandLog())
	{
		global.ui->GetCommandLog()->SetParent(pCommandLogPanelWidget);
		global.ui->GetCommandLog()->SetRect(10, 10, pCommandLogPanelWidget->GetRect().w - 20, pCommandLogPanelWidget->GetRect().h - 20);
		global.ui->GetCommandLog()->SetMovable(false);
		global.ui->GetCommandLog()->Show(true);
	}

	m_pMainPaint = new XDummyMainPaint(m_pData, "MainPaint", pMainPanelWidget);
	m_pMainPaint->SetRect(0, 0, pMainPanelWidget->GetRect().w, pMainPanelWidget->GetRect().h);
	m_pMainPaint->Show(true);

	m_pAgentStatusPaint = new XDummyAgentStatusPaint(m_pData, "AgentStatusPaint", pAgentStatusPanelWidget);
	m_pAgentStatusPaint->SetRect(10, 10, pAgentStatusPanelWidget->GetRect().w - 20, pAgentStatusPanelWidget->GetRect().h - 20);
	m_pAgentStatusPaint->Show(true);

	m_pStatisticsPaint = new XDummyStatisticsPaint(m_pData, "StatisticsPaint", pStatisticsPanelWidget);
	m_pStatisticsPaint->SetRect(10, 10, pStatisticsPanelWidget->GetRect().w - 20, pStatisticsPanelWidget->GetRect().h - 20);
	m_pStatisticsPaint->Show(true);


	MWidget* pCommandTabPage = global.mint->FindWidget(WIDGET_NAME_DUMMY_TABCTRL);
	if (pCommandTabPage)
	{
		pCommandTabPage->SetListener(m_pTabCtrlListener);
	}

}

void XDummyUI::Render()
{
	MWidget* pAgentStatusPanelWidget = global.mint->FindWidget(WIDGET_NAME_AGENT_STATUS_PAGE_PANEL);

}


void XDummyUI::XDummyAgentStatusPaint::OnDraw( MDrawContext* pDC )
{
	DrawAgentName(pDC);

	DrawAgentStatus(pDC);
}

XDummyUI::XDummyAgentStatusPaint::XDummyAgentStatusPaint( XDummyData* pData, const char* szName/*=NULL*/, MWidget* pParent/*=NULL*/, MListener* pListener/*=NULL*/ ) 
: MPaint(szName, pParent, pListener), m_pData(pData)
{
	int y = 10;
	int box_height = 7;

	for (size_t i = 0; i < m_pData->m_AgentInfos.size(); i++)
	{
		wstring strAgentName = m_pData->m_AgentInfos[i].strName;
		int nAgentCount = m_pData->m_AgentInfos[i].nCount;

		m_mapY.insert(map<wstring, int>::value_type(strAgentName, y));
		y += max((((nAgentCount / 100) + 1) * box_height), 50);
	}
}

void XDummyUI::XDummyAgentStatusPaint::DrawAgentName( MDrawContext* pDC )
{
	USES_CONVERSION_EX;

	int x = 10;
	int y = 10;

	pDC->SetColor(MCOLOR(0xFFFFFFFF));
	for (size_t i = 0; i < m_pData->m_AgentInfos.size(); i++)
	{
		wstring strAgentName = m_pData->m_AgentInfos[i].strName;

		map<wstring, int>::iterator itorY = m_mapY.find(strAgentName);
		if (itorY != m_mapY.end())
		{
			y = (*itorY).second;
			pDC->Text(x, y, W2A_EX(strAgentName.c_str(), 0));
		}
	}
}

void XDummyUI::XDummyAgentStatusPaint::DrawAgentStatus( MDrawContext* pDC )
{
	const int box_width = 5;
	const int box_height = 5;
	const int box_one_line_count = 100;

	const MCOLOR cNotConnected = 0xFFEA0101;
	const MCOLOR cRunning = 0xFF1DA745;
	const MCOLOR cError = 0xFFFF9614;
	const MCOLOR cComplete = 0xFF333333;

	map<wstring, int> mapIndex;
	mapIndex = m_mapY;

	for (map<wstring,int>::iterator itor = mapIndex.begin(); itor != mapIndex.end(); itor++)
	{
		(*itor).second = 0;
	}

	for (size_t i = 0; i < m_pData->m_AgentMgr.GetSize(); i++)
	{
		XBirdDummyAgent* pAgent = m_pData->m_AgentMgr.GetAgent(i);

		wstring strAgentName = pAgent->GetName();
		map<wstring, int>::iterator itorY = m_mapY.find(strAgentName);
		if (itorY == m_mapY.end()) continue;

		int y = (*itorY).second;

		int index = mapIndex[strAgentName];
		y += (index / box_one_line_count) * (box_height+2);

		int x = 150 + (index % box_one_line_count) * (box_width+1);

		MCOLOR clr;
		switch(pAgent->GetStatus())
		{
		case XBirdDummyAgent::DAS_RUNNING:		clr = cRunning;	break;
		case XBirdDummyAgent::DAS_NOTCONNECTED:	clr = cNotConnected;	break;
		case XBirdDummyAgent::DAS_ERROR:		clr = cError;	break;
		case XBirdDummyAgent::DAS_COMPLETE:		clr = cComplete; break;
		}
		pDC->SetColor(clr);	
		pDC->FillRectangle(MRECT(x, y, box_width, box_height));

		mapIndex[strAgentName]++;
	}
}


////////////////////////
void XDummyUI::XDummyMainPaint::OnDraw( MDrawContext* pDC )
{
//////////////////////////////////////////////////////////////////////////
///	FPS			LoginServer		ID				Total SendBytes
///	실행시간	GameServer		StartingID		Send BPS
///								AgentCount		Total RecvBytes
///												Recv BPS
//////////////////////////////////////////////////////////////////////////

	USES_CONVERSION_EX;

	char text[512];
	int x[4] = {10, 200, 400, 600};
	int y[4] = {5, 25, 45, 65};
	XDummyReporter::XDummyTotalInfo& totalInfo = m_pData->m_pReporter->GetTotalInfo();


	pDC->SetColor(MCOLOR(0xFFFFFFFF));

	int fps = m_FrameCounter.Update();
	sprintf_s(text, "FPS : %d", fps);
	pDC->Text(x[0], y[0], text);

	uint32 runningTime = totalInfo.m_nTotalRunningTime;
	int hour = runningTime/(60*60*1000);
	int minute = (runningTime/(60*1000))%60;
	int second = (runningTime/1000)%60;

	sprintf_s(text, "실행시간 : [%02d:%2d:%2d]", hour, minute, second);
	pDC->Text(x[0], y[1], text);


	sprintf_s(text, "LoginServer : %s", W2A_EX(m_pData->m_Config.strLoginServer.c_str(), 0));
	pDC->Text(x[1], y[0], text);

	sprintf_s(text, "GameServer : %s", W2A_EX(m_pData->m_Config.strGameServer.c_str(), 0));
	pDC->Text(x[1], y[1], text);


	sprintf_s(text, "ID : %s", W2A_EX(m_pData->m_Config.strID.c_str(), 0));
	pDC->Text(x[2], y[0], text);

	sprintf_s(text, "StartingID : %d", m_pData->m_Config.nStartingID);
	pDC->Text(x[2], y[1], text);

	int agentCount = 0;
	for (size_t i = 0; i < m_pData->m_AgentInfos.size(); i++)
		agentCount += m_pData->m_AgentInfos[i].nCount;

	sprintf_s(text, "AgentCount : %d", agentCount);
	pDC->Text(x[2], y[2], text);

	vector<XDummyReporter::XDummyAgentInfo>& vecAgentInfo = m_pData->m_pReporter->GetAgentInfo();
	int runningAgentCount = 0;
	for (size_t i = 0; i < vecAgentInfo.size(); i++)
		runningAgentCount += vecAgentInfo[i].m_nRunningAgentCount;

	sprintf_s(text, "RunningAgentCount : %d", runningAgentCount);
	pDC->Text(x[2], y[3], text);


	sprintf_s(text, "Total SendBytes : %u KB", totalInfo.m_nTotalSendBytes / 1024);
	pDC->Text(x[3], y[0], text);

	sprintf_s(text, "Send BPS : %u KB", totalInfo.m_nSecSendBytes / 1024);
	pDC->Text(x[3], y[1], text);

	sprintf_s(text, "Total RecvBytes : %u KB", totalInfo.m_nTotalRecvBytes / 1024);
	pDC->Text(x[3], y[2], text);

	sprintf_s(text, "Recv BPS : %u KB", totalInfo.m_nSecRecvBytes / 1024);
	pDC->Text(x[3], y[3], text);
}

XDummyUI::XDummyMainPaint::XDummyMainPaint( XDummyData* pData, const char* szName/*=NULL*/, MWidget* pParent/*=NULL*/, MListener* pListener/*=NULL*/ )
: MPaint(szName, pParent, pListener), m_pData(pData)
{

}

////////////////////////
XDummyUI::XDummyStatisticsPaint::XDummyStatisticsPaint( XDummyData* pData, const char* szName/*=NULL*/, MWidget* pParent/*=NULL*/, MListener* pListener/*=NULL*/ )
: XDummyAgentStatusPaint(pData, szName, pParent, pListener)
{

}

void XDummyUI::XDummyStatisticsPaint::OnDraw( MDrawContext* pDC )
{
	DrawAgentName(pDC);

	DrawAgentInfo(pDC);

}

void XDummyUI::XDummyStatisticsPaint::DrawAgentInfo(MDrawContext* pDC)
{
	char text[512];
	int x[4] = {100, 250, 400, 550};

	int index = 0;
	wstring strLastAgentName;

	vector<XDummyReporter::XDummyAgentInfo>& vecAgentInfo = m_pData->m_pReporter->GetAgentInfo();

	for (size_t i = 0; i < vecAgentInfo.size(); i++)
	{
		XDummyReporter::XDummyAgentInfo& agentInfo = vecAgentInfo[i];

		int y;
		
		map<wstring, int>::iterator itor = m_mapY.find(agentInfo.strAgentName);
		if (itor == m_mapY.end()) continue;

		y = (*itor).second;

		sprintf_s(text, "Agent수 : %d", agentInfo.m_nAgentCount);
		pDC->Text(x[0], y, text);

		sprintf_s(text, "실행 횟수 : %d", agentInfo.m_nTotalRunningCount);
		pDC->Text(x[1], y, text);

		sprintf_s(text, "SendBytes : %u KB", agentInfo.m_nTotalSendBytes / 1024);
		pDC->Text(x[2], y, text);

		sprintf_s(text, "RecvBytes : %u KB", agentInfo.m_nTotalRecvBytes / 1024);
		pDC->Text(x[3], y, text);

	}
}

bool XDummyUI::MTabCtrlListener::OnCommand( MWidget* pWidget, MEventMsg nMsg, const char* szArgs/*=NULL*/ )
{
	switch (nMsg)
	{
	case MEVENT_SELCHANGE:
		{
			MTabCtrl* pTabCtrl = dynamic_cast<MTabCtrl*>(pWidget);
			if (pTabCtrl == NULL) return false;

			if (pTabCtrl->GetCurSel() == 2)	// 로그창
			{
				XBirdClient::m_bPrintCommandLog = true;
				global.ui->GetCommandLog()->ClearMessage();
			}
			else
			{
				//XBirdClient::m_bPrintCommandLog = false;
			}
		}
		break;
	}
	return false;
}