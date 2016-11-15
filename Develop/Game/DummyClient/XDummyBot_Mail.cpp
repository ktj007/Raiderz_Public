#include "StdAfx.h"
#include "XDummyBot_Mail.h"
#include "XDummyMaster.h"
#include "XMail.h"
#include "CCommandResultTable.h"

#define TEST_MONEY 1000000
#define APPENDING_MONEY 1

bool XDummyBot_Mail::m_bSpawnMailboxNPC = false;

XDummyBot_Mail::XDummyBot_Mail(XBirdDummyAgent* pAgent, XDummyAgentInfo* pAgentInfo)
: XDummyBot(pAgent, pAgentInfo)
, m_nPhase(PHASE_READY)
, m_nMyMoney(0)
, m_pTestMail(NULL)
, m_nPostCount(0)
{
	wstring strMailboxID = pAgentInfo->GetValue(L"mailbox_id");
	m_MailboxID = _wtoi(strMailboxID.c_str());
	if (m_MailboxID == 0)
	{
		mlog3("Error! XDummyBot_Mail, Invalid Mailbox ID!!\n");
		::PostQuitMessage(0);
	}
	m_nTestItemID = 1; ///< 1HS
}

XDummyBot_Mail::~XDummyBot_Mail(void)
{
}

minet::MCommandResult XDummyBot_Mail::OnCommand( XBirdDummyAgent* pAgent, MCommand* pCommand )
{
	__super::OnCommand(pAgent, pCommand);

	XBirdClient* pClient = pAgent->GetClient();
	switch(pCommand->GetID())
	{
	case MC_FIELD_START_GAME:
		{
			if (!XDummyBot_Mail::m_bSpawnMailboxNPC)
			{
				XDummyBot_Mail::m_bSpawnMailboxNPC = true;
				XBIRDPOSTCMD3(pClient, MC_GM_REQUEST_SPAWN, MCommandParameterInt(m_MailboxID), MCommandParameterInt(1), MCommandParameterVector(m_pAgent->GetCenter()));
			}
			Init();
		}
		break;
	case MC_FIELD_IN_NPC:
		{
			MCommandParameter* pParam = pCommand->GetParameter(0);
			if(pParam->GetType()!=MPT_SINGLE_BLOB) return CR_ERROR;
			TD_UPDATE_CACHE_NPC* pNPCInfo = (TD_UPDATE_CACHE_NPC*)pParam->GetPointer();

			if (m_MailboxID == pNPCInfo->nNPCID)
			{
				m_MailboxUID = pNPCInfo->uid;
				m_MailboxUIID = pNPCInfo->nUIID;
				m_MailboxPos = pNPCInfo->vPos;
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
				if (m_MailboxID == pNpcNode[i].nNPCID)
				{
					m_MailboxUID = pNpcNode[i].uid;
					m_MailboxUIID = pNpcNode[i].nUIID;
					m_MailboxPos = pNpcNode[i].vPos;
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
				XItem* pItem = m_Inventory.InsertItem(tdItemInsert);
			}
		}
		break;
	case MC_ITEM_REMOVE:
		{
			vector<TD_ITEM_DELETE> vecTDItemDelete;
			if (pCommand->GetBlob(vecTDItemDelete, 0)==false) return CR_ERROR;

			for each(const TD_ITEM_DELETE& tdItemDelete in vecTDItemDelete)
			{
				m_Inventory.PopItem(tdItemDelete.m_nSlotID);
			}
		}
		break;
	case MC_CHAR_MYINFO:
		{
			TD_MYINFO tdMyInfo;
			if (pCommand->GetSingleBlob(tdMyInfo, 0)==false) return CR_ERROR;

			m_nMyMoney = tdMyInfo.nMoney;
		}
		break;
	case MC_CHAR_UPDATE_MONEY:
		{
			int nMoney;
			if (pCommand->GetParameter(&nMoney, 0, MPT_INT)==false) return CR_ERROR;

			m_nMyMoney = nMoney;
		}
		break;
	case MC_NPCINTERACTION_INTERACTION:
		{
			MUID uidNPC;
			vector<TD_INTERACTION_ELEMENT> vecTDIElement;

			if (pCommand->GetParameter(&uidNPC,0, MPT_UID)==false)	return CR_ERROR;
			if (pCommand->GetBlob(vecTDIElement, 2)==false) return CR_ERROR;

			XBIRDPOSTCMD2(pClient, MC_NPCINTERACTION_IELEMENT_REQ, MCommandParameterUID(uidNPC), MCommandParameterInt(vecTDIElement[0].nIElementID));
		}
		break;
	case MC_MAIL_MAILBOX_IS_OPENED:
		{
			vector<TD_MAIL_MAILBOX_MAIL_INFO> vecMailList;
			if (pCommand->GetBlob(vecMailList, 0)==false) return CR_ERROR;

			for (size_t i = 0; i < vecMailList.size(); ++i)
			{
				m_Mailbox.AddNewMail(	vecMailList[i].uidMail,
					(MAIL_TYPE)vecMailList[i].nType,
					vecMailList[i].strSenderName,
					vecMailList[i].strMailTitle,
					vecMailList[i].nAppendedItemID,
					vecMailList[i].nAppendedMoney,
					vecMailList[i].bIsRead,
					vecMailList[i].nExpiringTimeSec
					);
			}

			if (m_Mailbox.GetMailCount() > 0)
				ChangePhase(PHASE_CLEAR_MAILBOX);
			if (m_Mailbox.GetMailCount() == 0)
				ChangePhase(PHASE_START);
		}
		break;
	case MC_MAIL_READ_RES:
		{
			TD_MAIL_CONTENT_INFO tdMailContentInfo;
			wstring strMailText;
			vector<TD_ITEM> vecItem;

			if (!pCommand->GetSingleBlob(tdMailContentInfo, 0)) return CR_ERROR;
			if (!pCommand->GetParameter(strMailText, 1, MPT_WSTR)) return CR_ERROR;
			if (!pCommand->GetBlob(vecItem, 2)) return CR_ERROR;
			
			if (tdMailContentInfo.nAppendedMoney > 0)
				XBIRDPOSTCMD1(pClient, MC_MAIL_TAKE_APPENDED_MONEY_REQ, MCommandParameterUID(tdMailContentInfo.uidMail));

			for (size_t i = 0; i < vecItem.size(); ++i)
			{
				XBIRDPOSTCMD2(pClient, MC_MAIL_TAKE_APPENDED_ITEM_REQ, MCommandParameterUID(tdMailContentInfo.uidMail), MCommandParameterUChar(vecItem[i].m_nSlotID));
			}
			
			ChangePhase(PHASE_GET_APPENDED_GOODS);
		}
		break;
	case MC_MAIL_TAKE_APPENDED_MONEY_RES:
		{
			MUID uidMail;
			if (!pCommand->GetParameter(&uidMail, 0, MPT_UID)) return CR_ERROR;

			XMail* pMail = m_Mailbox.GetMail(uidMail);
			pMail->GetContent().nAppendedMoney = 0;
		}
		break;
	case MC_MAIL_TAKE_APPENDED_ITEM_RES:
		{
			MUID uidMail;
			unsigned char nMailSlotID;

			if (!pCommand->GetParameter(&uidMail,		0, MPT_UID)) return CR_ERROR;
			if (!pCommand->GetParameter(&nMailSlotID,	1, MPT_UCHAR)) return CR_ERROR;

			XMail* pMail = m_Mailbox.GetMail(uidMail);
			pMail->GetContent().DelAppendedItem(nMailSlotID);
		}
		break;
	case MC_MAIL_DELETE_RES:
		{
			MUID uidMail;
			if (!pCommand->GetParameter(&uidMail,		0, MPT_UID)) return CR_ERROR;

			m_Mailbox.DeleteMail(uidMail);

			if (m_nPhase == PHASE_DELETING_MAIL)
			{
				m_pTestMail = NULL;
				ChangePhase(PHASE_WRITE_MAIL);
			}
		}
		break;
	case MC_MAIL_APPEND_PUTUP_ITEM_RES:
		{
			ChangePhase(PHASE_POST_MAIL);
		}
		break;
	case MC_MAIL_POST_RES:
		{
			m_nPostCount++;
			if (m_nPostCount > 10000)
			{
				m_nPostCount = 0;

				// 소지 금액 재설정.
				wchar_t szMoney[16];
				wsprintf(szMoney, L"%d", TEST_MONEY);
				XBIRDPOSTCMD2(m_pAgent->GetClient(), MC_GM_SET_ME_REQ, MCommandParameterWideString(L"silver"), MCommandParameterWideString(szMoney));
			}

			int nResult;
			if (!pCommand->GetParameter(&nResult,0, MPT_INT)) return CR_ERROR;

			if (nResult == CR_SUCCESS)
			{
				XBIRDPOSTCMD0(pClient, MC_MAIL_APPEND_RESET_ITEM_REQ);
				ChangePhase(PHASE_WAIT_RECV_MAIL);
			}
		}
		break;
	case MC_MAIL_NOTIFY_MAIL:
		{
			vector<TD_MAIL_MAILBOX_MAIL_INFO> vecInfo;
			if (!pCommand->GetBlob(vecInfo, 1)) return CR_ERROR;

			// 메일 추가
			for (size_t i = 0; i < vecInfo.size(); ++i)
			{
				m_Mailbox.AddNewMail(	vecInfo[i].uidMail, 
					(MAIL_TYPE)vecInfo[i].nType,
					vecInfo[i].strSenderName,
					vecInfo[i].strMailTitle,
					vecInfo[i].nAppendedItemID,
					vecInfo[i].nAppendedMoney,
					vecInfo[i].bIsRead,
					vecInfo[i].nExpiringTimeSec
					);
			}
			ReadFirstMail();
		}
		break;
	}

	return CR_TRUE;
}

void XDummyBot_Mail::OnRun( float fDelta )
{
	XBirdClient* pClient = m_pAgent->GetClient();

	switch (m_nPhase)
	{
	case PHASE_READY :
		{
			if (!CheckInitialized()) return;
			
			ChangePhase(PHASE_MOVE_TO_MAILBOX);
		}
		break;
	case PHASE_MOVE_TO_MAILBOX:
		{
			vec3 vPosCloseToMailbox;
			float fRadius = 100;

			vPosCloseToMailbox.x = XMath::RandomNumber(m_MailboxPos.x - fRadius * 0.5f, m_MailboxPos.x + fRadius * 0.5f);
			vPosCloseToMailbox.y = XMath::RandomNumber(m_MailboxPos.y - fRadius * 0.5f, m_MailboxPos.y + fRadius * 0.5f);
			vPosCloseToMailbox.z = m_MailboxPos.z;

			XBIRDPOSTCMD2(pClient, MC_GM_MOVE_REQ, MCommandParameterInt(m_pAgent->GetLoginFieldID()), MCommandParameterVector(vPosCloseToMailbox));
			m_pAgent->SetCenter(vPosCloseToMailbox);
			ChangePhase(PHASE_OPEN_MAILBOX);
		}
		break;
	case PHASE_OPEN_MAILBOX:
		{
			XBIRDPOSTCMD3(pClient, MC_INTERACTION_INTERACTION_REQ, MCommandParameterUID(m_MailboxUID), MCommandParameterVector(m_pAgent->GetCenter()), MCommandParameterVector(vec3::ZERO));
			ChangePhase(PHASE_WAIT_TO_OPEN_MAILBOX);
		}
		break;
	case PHASE_CLEAR_MAILBOX:
		{
			for (int i = 0; i < m_Mailbox.GetMailCount(); ++i)
			{
				XMail* pMail = m_Mailbox.GetMailIndex(i);
				XBIRDPOSTCMD1(pClient, MC_MAIL_DELETE_REQ, MCommandParameterUID(pMail->GetUID()));
			}
			ChangePhase(PHASE_WAIT_TO_CLEAR_MAILBOX);
		}
		break;
	case PHASE_WAIT_TO_CLEAR_MAILBOX:
		{
			if (m_Mailbox.GetMailCount() == 0)
				ChangePhase(PHASE_START);
		}
		break;
	case PHASE_START:
		{
			if (!IsEvenNumberAgent()) return;
			
			ChangePhase(PHASE_WRITE_MAIL);
		}
		break;
	case PHASE_GET_APPENDED_GOODS:
		{
			if (!CheckGetAllAppendedGoods()) return;

			// Delete!
			XBIRDPOSTCMD1(pClient, MC_MAIL_DELETE_REQ, MCommandParameterUID(m_pTestMail->GetUID()));
			ChangePhase(PHASE_DELETING_MAIL);
		}
		break;
	case PHASE_WRITE_MAIL:
		{
			vector<XItem* > vecInvenItem = m_Inventory.GetAllItem();
			if (vecInvenItem.size() == 0)
			{
				ChangePhase(PHASE_POST_MAIL);
				return;
			}

			TD_MAIL_RESERVE_ITEM tdMailReservingItem;
			tdMailReservingItem.nInventorySlotID = vecInvenItem[0]->m_nSlotID;
			tdMailReservingItem.nItemAmount = vecInvenItem[0]->m_nAmount;
			XBIRDPOSTCMD1(pClient, MC_MAIL_APPEND_PUTUP_ITEM_REQ, MCommandParameterSingleBlob(tdMailReservingItem));
			ChangePhase(PHASE_PUTUP_APPENDING_ITEM);
		}
		break;
	case PHASE_POST_MAIL:
		{
			TD_MAIL_POST_REQ_INFO tdMailPostReqInfo;
			tdMailPostReqInfo.nAppendedMoney = APPENDING_MONEY;
			wsprintf(tdMailPostReqInfo.strReceiverName, L"%s", m_pTargetBot->GetAgentName());	///< TODO : Name
			wsprintf(tdMailPostReqInfo.strMailTitle, L"Test Mail Title");
			XBIRDPOSTCMD2(pClient, MC_MAIL_POST_REQ, MCommandParameterSingleBlob(tdMailPostReqInfo), MCommandParameterWideString(L"Test Mail Message"));

			ChangePhase(PHASE_WAIT_POST_MAIL_RES);
		}
		break;
	}
}


void XDummyBot_Mail::Init()
{
	// 대상 봇 설정.
	m_pTargetBot = GetTraderBot();
	if (m_pTargetBot == NULL)
	{
		mlog3("Error! XDummyBot_Mail, Not Exist Target Bot... (name=%S, id=%d)\n", m_pAgent->GetName(), m_pAgent->GetID());
		ChangePhase(PHASE_NO_PARTNER);
		return;
	}

	XBIRDPOSTCMD2(m_pAgent->GetClient(), MC_GM_SET_ME_REQ, MCommandParameterWideString(L"grade"), MCommandParameterWideString(L"3"));
	XBIRDPOSTCMD0(m_pAgent->GetClient(), MC_GM_GOD_REQ);

	// 인벤토리 정리하고 첨부할 아이템 추가.
	XBIRDPOSTCMD0(m_pAgent->GetClient(), MC_GM_CLEAR_INVENTORY_REQ);
	XBIRDPOSTCMD2(m_pAgent->GetClient(), MC_GM_ITEM_GIVE_REQ, MCommandParameterInt(m_nTestItemID), MCommandParameterInt(1));

	// 초기 소지 금액 설정.
	wchar_t szMoney[16];
	wsprintf(szMoney, L"%d", TEST_MONEY);
	XBIRDPOSTCMD2(m_pAgent->GetClient(), MC_GM_SET_ME_REQ, MCommandParameterWideString(L"silver"), MCommandParameterWideString(szMoney));
}

bool XDummyBot_Mail::CheckInitialized()
{
	if (m_MailboxUID.IsInvalid()) return false;
	if (m_MailboxUIID == 0) return false;
	
	vector<XItem* > vecItem = m_Inventory.GetAllItem();
	if (vecItem.size() != 1) return false;
	if (vecItem[0]->m_pItemData->m_nID != m_nTestItemID) return false;

	if (m_nMyMoney != TEST_MONEY) return false;

	if (m_nPhase == PHASE_NO_PARTNER) return false;
	
	return true;
}

bool XDummyBot_Mail::CheckGetAllAppendedGoods()
{
	if (m_pTestMail == NULL) return false;
	if (m_pTestMail->GetContent().nAppendedMoney != 0) return false;
	if (m_pTestMail->GetContent().vAppendedItems.size() != 0) return false;

	return true;
}

void XDummyBot_Mail::ReadFirstMail()
{
	m_pTestMail = m_Mailbox.GetMailIndex(m_Mailbox.GetMailCount()-1);
	if (m_pTestMail == NULL)
	{
		m_Mailbox.Clear();
		ChangePhase(PHASE_WRITE_MAIL);
		return;
	}

	if (!m_pTestMail->IsRead())
	{
		m_pTestMail->SetRead();
		XBIRDPOSTCMD1(m_pAgent->GetClient(), MC_MAIL_READ_REQ, MCommandParameterUID(m_pTestMail->GetUID()));
	}
	else
	{
		// Delete!
		XBIRDPOSTCMD1(m_pAgent->GetClient(), MC_MAIL_DELETE_REQ, MCommandParameterUID(m_pTestMail->GetUID()));
		ChangePhase(PHASE_DELETING_MAIL);
	}
	ChangePhase(PHASE_READ_MAIL);
}

bool XDummyBot_Mail::IsEvenNumberAgent()
{
	return (0 == m_pAgent->GetID() % 2);
}

XDummyBot_Mail* XDummyBot_Mail::GetTraderBot()
{
	int nID = m_pAgent->GetID();
	int nTraderBotID = nID + 1;
	if (1 == nID % 2)
	{
		nTraderBotID = nID - 1;
	}

	XBirdDummyAgentMgr* pAgentMgr = m_pAgent->GetAgentMgr();
	if (NULL == pAgentMgr) return NULL;
	XBirdDummyAgent* pTraderAgent = pAgentMgr->GetAgentByID(nTraderBotID);
	if (NULL == pTraderAgent) return NULL;
	XDummyBot* pTraderBot = pTraderAgent->GetFrontBot();
	if (NULL == pTraderBot) return NULL;
	if (GetName() != pTraderBot->_GetName()) return NULL;

	return dynamic_cast<XDummyBot_Mail*>(pTraderBot);
}