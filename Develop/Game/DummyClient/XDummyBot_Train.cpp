#include "StdAfx.h"
#include "XDummyBot_Train.h"
#include "XBirdDummyAgent.h"
#include "XDummyLazyCommandPoster.h"
#include "XDummyMaster.h"
#include "MCsvParser.h"


XDummyBot_Train::XDummyBot_Train(XBirdDummyAgent* pAgent, XDummyAgentInfo* pAgentInfo)
: XDummyBot(pAgent, pAgentInfo)
, m_nListCursor(0)
{
	InitParam();

	const float TIME_TIMEOUT_SEC = 180.0;
	m_rgltTimeout.SetTickTime(TIME_TIMEOUT_SEC);
}

XDummyBot_Train::~XDummyBot_Train(void)
{	
}

bool XDummyBot_Train::InitParam()
{
	wstring strFieldList = m_pAgentInfo->GetValue(L"talent_list");
	if (strFieldList.empty())
	{
		mlog3("Error! XDummyBot_Train::InitParam(), talent_list is empty\n");
		return false;
	}

	MCsvParser csv_parser;

	USES_CONVERSION_EX;
	csv_parser.LoadFromStream(W2A_EX(strFieldList.c_str(), 0));
	int nColCount = csv_parser.GetColCount(0);

	char szBuff[1024];
	for (int nCol = 0; nCol < nColCount; nCol++)
	{
		if (csv_parser.GetData(nCol, 0, szBuff, 1024))
		{
			m_vecTalentList.push_back(atoi(szBuff));
		}
	}

	if (m_vecTalentList.empty())
	{
		mlog3("Error! XDummyBot_Train::InitParam(), Not Exist Valid FieldList\n");
		return false;
	}

	return true;
}

void XDummyBot_Train::OnRun( float fDelta )
{
	if (m_pAgent->GetStatus() == XBirdDummyAgent::DAS_ERROR) return;

	if (m_rgltTimeout.IsReady(fDelta))
	{
		m_pAgent->SetStatusError();

		char szErrorLog[1024] = {0};
		sprintf_s(szErrorLog, 1024, "(%S)(%d) XDummyBot_Train::OnRun(), Timeout!!\n", m_pAgent->GetLoginID(), m_pAgent->GetID());
		mlog(szErrorLog);
	}
}

minet::MCommandResult XDummyBot_Train::OnCommand( XBirdDummyAgent* pAgent, MCommand* pCommand )
{
	__super::OnCommand(pAgent, pCommand);

	XBirdClient* pClient = pAgent->GetClient();
	switch(pCommand->GetID())
	{
	case MC_CHAR_MYINFO:
		{
			if (pCommand->GetBlob(m_vecRecvTalentList, 4)==false) return CR_ERROR;
		}
		break;
	case MC_FIELD_CHANGE_FIELD:
		{
			XBIRDPOSTCMD2(pClient, MC_GM_SET_ME_REQ, MCommandParameterWideString(L"grade"), MCommandParameterWideString(L"3"));
			XBIRDPOSTCMD2(pClient, MC_GM_SET_ME_REQ, MCommandParameterWideString(L"tp"), MCommandParameterWideString(L"100"));
			XBIRDPOSTCMD0(pClient, MC_GM_GOD_REQ);
		}
		break;
	case MC_FIELD_START_GAME:
		{
			m_rgltTimeout.Start();

			if (m_vecRecvTalentList.empty())
			{
				PostTrain(pClient);
			}
			else
			{
				XBIRDPOSTCMD2(pClient, MC_GM_ITEM_GIVE_REQ, MCommandParameterInt(9900020), MCommandParameterInt(1));				
			}
		}
		break;
	case MC_TRAINING_TRAIN:
		{
			m_rgltTimeout.Reset();

			if (m_nListCursor >= (int)m_vecTalentList.size())
			{
				XBIRDPOSTCMD2(pClient, MC_GM_ITEM_GIVE_REQ, MCommandParameterInt(9900020), MCommandParameterInt(1));				
			}
			else
			{
				PostTrain(pClient);
			}
		}
		break;
	case MC_ITEM_ADD:
		{
			vector<TD_ITEM_INSERT> vecTDItemInsert;
			if (pCommand->GetBlob(vecTDItemInsert, 0)==false) return CR_ERROR;

			for each(const TD_ITEM_INSERT& tdItemInsert in vecTDItemInsert)
			{
				XBIRDPOSTCMD1(pClient, MC_ITEM_USE_REQ, MCommandParameterInt(tdItemInsert.m_nSlotID));
			}
		}
		break;
	case MC_TRAINING_UNTRAIN_ALL:
		{
			m_rgltTimeout.Reset();
			PostTrain(pClient);
		}
		break;
	}

	return CR_TRUE;
}

void XDummyBot_Train::PostTrain(XBirdClient* pClient)
{
	if (m_vecTalentList.size() == 0) return;
	if (m_nListCursor >= (int)m_vecTalentList.size())
		m_nListCursor = 0;

	int nTalentID = m_vecTalentList[m_nListCursor];
	XBIRDPOSTCMD1(pClient, MC_TRAINING_TRAIN_REQ, MCommandParameterInt(nTalentID));

	m_nListCursor++;
}