#include "StdAfx.h"
#include "XDummyBot_Gather.h"
#include "XDummyMaster.h"
#include "MCsvParser.h"
#include "CCommandResultTable.h"

const int GATHER_TOOL_PICK_AXE = 49000;
const int GATHER_TOOL_TROWEL = 49001;
const int GATHER_TOOL_KEY = 49002;

const int LICENSE_GATHER_1 = 90000;


XDummyBot_Gather::XDummyBot_Gather(XBirdDummyAgent* pAgent, XDummyAgentInfo* pAgentInfo)
: XDummyBot(pAgent, pAgentInfo)
, m_Phase(PHASE_INIT)
, m_nCurGatherNPCIndex(0)
{
	if (!InitParam())
	{
		::PostQuitMessage(0);
	}
}

XDummyBot_Gather::~XDummyBot_Gather()
{
}

bool XDummyBot_Gather::InitParam()
{
	wstring strFieldList = m_pAgentInfo->GetValue(L"gather_npc");
	if (strFieldList.empty())
	{
		mlog3("Error! XDummyBot_Gather::InitParam(), talent_list is empty\n");
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
			m_vecGatherNPCIDList.push_back(atoi(szBuff));
		}
	}

	if (m_vecGatherNPCIDList.empty())
	{
		mlog3("Error! XDummyBot_Gather::InitParam(), Not Exist Valid FieldList\n");
		return false;
	}

	return true;
}

MCommandResult XDummyBot_Gather::OnCommand(XBirdDummyAgent* pAgent, MCommand* pCommand)
{
	__super::OnCommand(pAgent, pCommand);

	XBirdClient* pClient = pAgent->GetClient();
	switch(pCommand->GetID())
	{
	case MC_FIELD_IN_NPC:
		{
			MCommandParameter* pParam = pCommand->GetParameter(0);
			if(pParam->GetType()!=MPT_SINGLE_BLOB) return CR_ERROR;
			TD_UPDATE_CACHE_NPC* pNPCInfo = (TD_UPDATE_CACHE_NPC*)pParam->GetPointer();

			for (size_t i = 0; i < m_vecGatherNPCIDList.size(); ++i)
			{
				if (m_vecGatherNPCIDList[i] == pNPCInfo->nNPCID)
				{
					float fDistance = m_pAgent->GetCenter().XYDistanceTo(pNPCInfo->vPos);
					if (fDistance >= 100)
						continue;

					ST_GATHER_NPC_INFO infoGatherNPC;
					infoGatherNPC.nUID = pNPCInfo->uid;
					infoGatherNPC.nUIID = pNPCInfo->nUIID;
					infoGatherNPC.vPos = pNPCInfo->vPos;

					m_vecGatherNPC.push_back(infoGatherNPC);
				}
			}
		}
		break;
	case MC_FIELD_SECTOR_ENTITY_INFO:
		{
			MCommandParameter* pParam = pCommand->GetParameter(1);
			if(pParam->GetType() != MPT_BLOB) return CR_ERROR;

			TD_UPDATE_CACHE_NPC* pNpcNode = (TD_UPDATE_CACHE_NPC*)pParam->GetPointer();
			int nNpcCount = pParam->GetBlobCount();

			for (int i = 0; i < nNpcCount; i++)
			{
				float fDistance = m_pAgent->GetCenter().XYDistanceTo(pNpcNode[i].vPos);
				if (fDistance >= 100)
					continue;

				for (size_t j = 0; j < m_vecGatherNPCIDList.size(); ++j)
				{
					if (m_vecGatherNPCIDList[j] == pNpcNode[i].nNPCID)
					{
						ST_GATHER_NPC_INFO infoGatherNPC;
						infoGatherNPC.nUID = pNpcNode[i].uid;
						infoGatherNPC.nUIID = pNpcNode[i].nUIID;
						infoGatherNPC.vPos = pNpcNode[i].vPos;

						m_vecGatherNPC.push_back(infoGatherNPC);
					}
				}
			}
		}
		break;
	case MC_ACTION_DIE:
		{
			UIID uiidNPC;

			if (pCommand->GetParameter(&uiidNPC,0, MPT_USHORT)==false)	return CR_ERROR;

			GATHER_NPC_LIST::iterator itr = m_vecGatherNPC.begin();
			for (; itr != m_vecGatherNPC.end(); ++itr)
			{
				if (uiidNPC == (*itr).nUIID)
				{
					m_vecGatherNPC.erase(itr);
					break;
				}
			}
		}
		break;
	case MC_ITEM_ADD:
		{
			vector<TD_ITEM_INSERT> vecTDItemInsert;
			if (pCommand->GetBlob(vecTDItemInsert, 0)==false) return CR_ERROR;

			for each(const TD_ITEM_INSERT& tdItemInsert in vecTDItemInsert)
			{
				if (tdItemInsert.m_nID == GATHER_TOOL_PICK_AXE)		///< °î±ªÀÌ
					m_InitTools.bTakePickAxe = true;
				else if (tdItemInsert.m_nID == GATHER_TOOL_TROWEL)	///< ¸ðÁ¾»ð
					m_InitTools.bTakeTrowel = true;
				else if (tdItemInsert.m_nID == GATHER_TOOL_KEY)	///< ¶ôÇÈ
					m_InitTools.bTakeKey = true;
				else
				{
					XBIRDPOSTCMD2(pClient, MC_ITEM_DROP_REQ, MCommandParameterInt(tdItemInsert.m_nSlotID), MCommandParameterInt(tdItemInsert.m_nAmount));
				}
			}
		}
		break;

		//////////////////////////////////////////////////////////////////////////
	case MC_NPCINTERACTION_INTERACTION:
		{
			MUID uidNPC;
			vector<TD_INTERACTION_ELEMENT> vecTDIElement;

			if (pCommand->GetParameter(&uidNPC,0, MPT_UID)==false)	return CR_ERROR;
			if (pCommand->GetBlob(vecTDIElement, 2)==false) return CR_ERROR;

			m_uidCurGatherNPC = uidNPC;

			XBIRDPOSTCMD2(pClient, MC_NPCINTERACTION_IELEMENT_REQ, MCommandParameterUID(uidNPC), MCommandParameterInt(vecTDIElement[0].nIElementID));
		}
		break;
	case MC_MSG_RES:
		{
			int nType;
			int nAdditional;

			if (!pCommand->GetParameter(&nType, 0, MPT_INT))		return CR_ERROR;
			if (!pCommand->GetParameter(&nAdditional, 4, MPT_INT))	return CR_ERROR;

			if (MT_SYSTEM != nType) break;
			
			if (nAdditional == CR_FAIL_LOOT_ALREADY_OTHER_GATHERING ||
				nAdditional == CR_FAIL_LOOT_ALREADY_OTHER_INTERACT_LOOTING ||
				nAdditional == CR_FAIL_LOOT_NOT_EXIST_ITEM)
			{
				m_vecGatherNPC.erase(m_vecGatherNPC.begin());

				ChangePhase(PHASE_FAIL);
			}
		}
		break;
	case MC_NPCINTERACTION_END:
		{
			if (m_Phase == PHASE_FAIL)
				ChangePhase(PHASE_START);
		}
		break;
	case MC_LOOT_BEGIN:
		break;
	case MC_LOOT_SEE_ITEM:
		{
			vector<TD_ITEM_DROP> vecTDDropItem;
			if (pCommand->GetBlob(vecTDDropItem,	0)==false)	return CR_ERROR;

			vector<MUID> vecLootItemUID;
			for (size_t i = 0; i < vecTDDropItem.size(); ++i)
			{
				vecLootItemUID.push_back(vecTDDropItem[i].m_nDropItemUID);
			}

			XBIRDPOSTCMD2(pClient, MC_LOOT_LOOT_ITEM_REQ, MCommandParameterUID(m_uidCurGatherNPC), MCommandParameterBlob(vecLootItemUID));

			ChangePhase(PHASE_GATHERING);
		}
		break;
	case MC_LOOT_END:
		{
			MUID uidGetter;
			if (pCommand->GetParameter(&uidGetter,0, MPT_UID)==false)	return CR_ERROR;

			if (uidGetter == m_pAgent->GetUID())
				ChangePhase(PHASE_FIN);
		}
		break;
	}

	return CR_TRUE;
}

void XDummyBot_Gather::OnRun(float fDelta)
{
	XBirdClient* pClient = m_pAgent->GetClient();

	switch(m_Phase)
	{
	case PHASE_INIT:
		{
			XBIRDPOSTCMD2(pClient, MC_GM_SET_ME_REQ, MCommandParameterWideString(L"grade"), MCommandParameterWideString(L"3"));
			XBIRDPOSTCMD2(pClient, MC_GM_SET_ME_REQ, MCommandParameterWideString(L"level"), MCommandParameterWideString(L"50"));
			XBIRDPOSTCMD0(pClient, MC_GM_GOD_REQ);
			XBIRDPOSTCMD0(pClient, MC_GM_REBIRTH_REQ);

			XBIRDPOSTCMD0(pClient, MC_GM_CLEAR_INVENTORY_REQ);
			XBIRDPOSTCMD2(pClient, MC_GM_ITEM_GIVE_REQ, MCommandParameterInt(GATHER_TOOL_PICK_AXE), MCommandParameterInt(1));	///< °î±ªÀÌ
			XBIRDPOSTCMD2(pClient, MC_GM_ITEM_GIVE_REQ, MCommandParameterInt(GATHER_TOOL_TROWEL), MCommandParameterInt(1));	///< ¸ðÁ¾»ð
			XBIRDPOSTCMD2(pClient, MC_GM_ITEM_GIVE_REQ, MCommandParameterInt(GATHER_TOOL_KEY), MCommandParameterInt(1));	///< ¶ôÇÈ

			for (int i = 0; i < 10; ++i)
			{
				wchar_t szTalentID[32];
				_itow(LICENSE_GATHER_1 + i, szTalentID, 10);

				XBIRDPOSTCMD2(pClient, MC_GM_SET_ME_REQ, MCommandParameterWideString(L"talent"), MCommandParameterWideString(szTalentID));
			}

			XBIRDPOSTCMD3(pClient, MC_GM_REQUEST_SPAWN, MCommandParameterInt(m_vecGatherNPCIDList[m_nCurGatherNPCIndex]), MCommandParameterInt(1), MCommandParameterVector(m_pAgent->GetCenter()));

			ChangePhase(PHASE_READY);
		}
		break;
	case PHASE_READY:
		{
			if (m_InitTools.IsReady())
				ChangePhase(PHASE_START);
		}
		break;
	case PHASE_START:
		{
			if (0 == m_vecGatherNPC.size())
				return;

			ST_GATHER_NPC_INFO& refGatherNPC = m_vecGatherNPC[0];
			
			XBIRDPOSTCMD3(pClient, MC_INTERACTION_INTERACTION_REQ, MCommandParameterUID(refGatherNPC.nUID), MCommandParameterVector(m_pAgent->GetCenter()), MCommandParameterVector(vec3::ZERO));
			ChangePhase(PHASE_WAIT_TO_INTERACTION);
		}
		break;
	case PHASE_FIN:
		{
			m_nCurGatherNPCIndex++;
			if (m_nCurGatherNPCIndex >= (int)m_vecGatherNPCIDList.size() ||
				m_nCurGatherNPCIndex < 0)
				m_nCurGatherNPCIndex = 0;

			XBIRDPOSTCMD3(pClient, MC_GM_REQUEST_SPAWN, MCommandParameterInt(m_vecGatherNPCIDList[m_nCurGatherNPCIndex]), MCommandParameterInt(1), MCommandParameterVector(m_pAgent->GetCenter()));

			ChangePhase(PHASE_START);
		}
		break;
	}
}
