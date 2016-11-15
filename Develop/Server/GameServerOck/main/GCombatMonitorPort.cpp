#include "StdAfx.h"
#include "GCombatMonitorPort.h"
#include "GEntityActor.h"
#include "GField.h"
#include "GEntityNPC.h"
#include "GEntityPlayer.h"
#include "GItemHolder.h"
#include "AStlUtil.h"
#include "GGlobal.h"
#include "GPlayerObjectManager.h"
#include "GFieldMgr.h"
#include "GSharedField.h"

GCombatMonitorPort::GCombatMonitorPort(void)
: m_bRunning(false)
, m_bWillStop(false)
{
}

GCombatMonitorPort::~GCombatMonitorPort(void)
{
}

GCombatMonitorPort& GCombatMonitorPort::Inst()
{
	static GCombatMonitorPort inst;
	return inst;
}

void GCombatMonitorPort::SetField( GField* pField )
{
	VALID(pField);
	m_pField = pField;
}

void GCombatMonitorPort::AttachSubject( GEntityActor* pActor )
{
	VALID(pActor);
	pActor->AttachObserver(this);
	m_setSubjects.insert(pActor->GetUID());

	Send(ExtractFromActor(pActor));
}

void GCombatMonitorPort::DettachSubject( GEntityActor* pActor )
{
	VALID(pActor);
	m_setSubjects.erase(pActor->GetUID());
	pActor->DetachObserver(this);
}

void GCombatMonitorPort::Prepare()
{
	m_bRunning = true;
}

void GCombatMonitorPort::Start()
{
	SendCommand("start");
}

void GCombatMonitorPort::Stop()
{
	m_bWillStop = true;
}

void GCombatMonitorPort::StopImpl()
{
	m_bWillStop = false;
	m_bRunning = false;
	if (m_pField)
	{
		m_pField->DetachCombatMonitor();
	}

	SendCommand("stop");
}

void GCombatMonitorPort::Reset()
{
	VALID(m_pField);

	set<MUID> setSubject = m_setSubjects;
	for each(MUID each in setSubject)
	{
		GEntityActor* pActor = m_pField->GetEntityMgr()->FindActor(each);
		if (!pActor)
			continue;

		DettachSubject(pActor);
	}

	m_pField = NULL;
}

bool GCombatMonitorPort::IsEnable()
{
	return true;
}
string GCombatMonitorPort::ExtractFromActor( GEntityActor* pActor )
{
	MXml xml;
	MXmlDocument* root = xml.Doc();
	MXmlElement* pElement = new MXmlElement(pActor->IsPlayer()?"Player":"NPC");
	root->LinkEndChild(pElement);

	AddXMLElement("Name", MLocale::ConvUTF16ToAnsi(pActor->GetName()), pElement);
	AddXMLElement("UIID", pActor->GetUIID(), pElement);
	AddXMLElement("Level", pActor->GetLevel(), pElement);

	if (pActor->IsNPC())
	{
		GEntityNPC* pNPC = ToEntityNPC(pActor);
		AddXMLElement("ID", pNPC->GetID(), pElement);
	}

	if (pActor->IsPlayer())
	{
		GEntityPlayer* pPlayer = ToEntityPlayer(pActor);
		vector<GItem*> vecItem;
		pPlayer->GetItemHolder()->CollectAllItem(SLOTTYPE_EQUIP, vecItem);
		for each (GItem* each in vecItem)
		{
			if (!each)
				continue;

			if (!each->m_pItemData)
				continue;

			int nItemSlot = each->m_pItemData->m_nItemSlot;
			if (nItemSlot < ITEMSLOT_DEFAULT_BEGIN)
				continue;

			if (nItemSlot > ITEMSLOT_DEFAULT_MAX)
				continue;

			string strSlotName = MLocale::ConvTCHARToAnsi(CSItemSlotStr[nItemSlot]);
			AddXMLElement("Equip_"+strSlotName, each->GetID(), pElement);
		}

		// TODO Stat
	}

	return xml.GetBuffer();
}

void GCombatMonitorPort::OnMsg( PCOPYDATASTRUCT pcds )
{
	if (!IsEnable())
		return;

	string strText = (const char*)pcds->lpData;
	deque<string> vecTokens = Tokenize(strText, " ");
	if (vecTokens.empty())
		return;

	string strCommand = vecTokens.front();
	vecTokens.pop_front();
	if (strCommand == "start")
	{
		string strType = vecTokens.front();
		vecTokens.pop_front();
		if (strType == "field")
		{
			string strPlayerName = vecTokens.front();
			vecTokens.pop_front();
			
			GEntityPlayer* pPlayer =
				gmgr.pPlayerObjectManager->GetEntityFromPlayerID(MLocale::ConvAnsiToTCHAR(strPlayerName.c_str()));

			GField* pField = NULL;
			if (pPlayer)
			{
				pField =
					pPlayer->GetField();
				if (!pField)
					return;
			}
			else
			{
				int nFieldID = atoi(strPlayerName.c_str());
				pField =
					gmgr.pFieldMgr->GetSharedField(nFieldID);
				if (!pField)
					return;
			}

			pField->AttachCombatMonitor(this);
		}
	}
	else if (strCommand == "stop")
	{
		GCombatMonitorPort::Inst().Stop();
	}
	else
	{
		mlog3("%s - unknown command: %s\n", __FUNCTION__, strCommand.c_str());
	}
}

void GCombatMonitorPort::SendCommand( string strType )
{
	MXml xml;
	MXmlDocument* root = xml.Doc();
	MXmlElement* pElement = new MXmlElement("Command");
	root->LinkEndChild(pElement);

	AddXMLElement("Type", strType.c_str(), pElement);

	Send(xml.GetBuffer());
}

void GCombatMonitorPort::OnSendFailed()
{
	Stop();
}

void GCombatMonitorPort::Send( string strMessage )
{
	if (!m_bRunning)
		return;

	__super::Send(strMessage);
}

int GCombatMonitorPort::GetActorID(GEntityActor* pActor)
{
	VALID_RET(pActor, -1);
	if (pActor->IsPlayer())
	{
		return pActor->GetUIID();
	}
	else if (pActor->IsNPC())
	{
		GEntityNPC* pNPC = ToEntityNPC(pActor);
		GNPCInfo* pNPCInfo = pNPC->GetNPCInfo();
		if (!pNPCInfo)
			return -1;
		return pNPCInfo->nID;
	}

	return -1;
}

void GCombatMonitorPort::SendStats( const char* szType, GEntityActor* pUser, GEntityActor* pAttacker, const char* szSourceType, int nSourceID, int nDamage)
{
	MXml xml;
	MXmlDocument* root = xml.Doc();
	MXmlElement* pElement = new MXmlElement("Stats");
	root->LinkEndChild(pElement);

	AddXMLElement("Type", szType, pElement);
	AddXMLElement("Owner", pUser?GetActorID(pUser):0, pElement);
	AddXMLElement("Attacker", pAttacker?GetActorID(pAttacker):0, pElement);
	AddXMLElement("SourceType", szSourceType, pElement);
	AddXMLElement("SourceID", nSourceID, pElement);
	AddXMLElement("Damage", nDamage, pElement);

	Send(xml.GetBuffer());
}

void GCombatMonitorPort::OnCombatBegin(GEntityActor* pOwner)
{
	SendStats("Start", pOwner, NULL, "combat_time", 0l);
}

void GCombatMonitorPort::OnCombatEnd(GEntityActor* pOwner)
{
	SendStats("End", pOwner, NULL, "combat_time", 0);
}

void GCombatMonitorPort::OnUseTalent( GEntityActor* pUser, GTalentInfo* pTalentInfo )
{
	VALID(pTalentInfo);

	SendStats("UseTalent", pUser, NULL, "talent", pTalentInfo->m_nID);
}

void GCombatMonitorPort::OnActTalent( GEntityActor* pUser, GTalentInfo* pTalentInfo )
{
	VALID(pTalentInfo);

	SendStats("ActTalent", pUser, NULL, "talent", pTalentInfo->m_nID);
}

void GCombatMonitorPort::OnHit( GEntityActor* pOwner, GEntityActor* pAttacker, GTalentInfo* pTalentInfo )
{
	VALID(pTalentInfo);
	SendStats("Hit", pOwner, pAttacker, "talent", pTalentInfo->m_nID);
}

void GCombatMonitorPort::OnHitSuccess( GEntityActor* pOwner, GTalentInfo* pTalentInfo )
{
	VALID(pTalentInfo);
	SendStats("HitSuccess", NULL, pOwner, "talent", pTalentInfo->m_nID);
}

void GCombatMonitorPort::OnHitAll( GEntityActor* pOwner, GTalentInfo* pTalentInfo )
{
	VALID(pTalentInfo);
	SendStats("HitAll", NULL, pOwner, "talent", pTalentInfo->m_nID);
}

void GCombatMonitorPort::OnCriHit( GEntityActor* pOwner, GEntityActor* pAttacker, GTalentInfo* pTalentInfo )
{
	VALID(pTalentInfo);
	SendStats("CriHit", pOwner, pAttacker, "talent", pTalentInfo->m_nID);
}

void GCombatMonitorPort::OnMiss( GEntityActor* pOwner, uint32 nCombatResultFalg, GEntityActor* pAttacker, GTalentInfo* pAttackTalentInfo )
{
	VALID(pAttackTalentInfo);
	SendStats("Avoid", pOwner, pAttacker, "talent", pAttackTalentInfo->m_nID);
}

void GCombatMonitorPort::OnGuard( GEntityActor* pOwner, GEntityActor* pAttacker, GTalentInfo* pAttackTalentInfo )
{
	VALID(pAttackTalentInfo);
	SendStats("Guard", pOwner, pAttacker, "talent", pAttackTalentInfo->m_nID);
}

void GCombatMonitorPort::OnPerfectGuard( GEntityActor* pOwner, GEntityActor* pAttacker, GTalentInfo* pAttackTalentInfo )
{
	VALID(pAttackTalentInfo);
	SendStats("PerfectGuard", pOwner, pAttacker, "talent", pAttackTalentInfo->m_nID);
}

void GCombatMonitorPort::OnPartialGuard( GEntityActor* pOwner, GEntityActor* pAttacker, GTalentInfo* pAttackTalentInfo )
{
	VALID(pAttackTalentInfo);
	SendStats("PartialGuard", pOwner, pAttacker, "talent", pAttackTalentInfo->m_nID);
}

void GCombatMonitorPort::OnAbsoluteGuard( GEntityActor* pOwner, GEntityActor* pAttacker, GTalentInfo* pAttackTalentInfo )
{
	VALID(pAttackTalentInfo);
	SendStats("AbsoluteGuard", pOwner, pAttacker, "talent", pAttackTalentInfo->m_nID);
}

void GCombatMonitorPort::OnTalentImmuned( GEntityActor* pOwner, GEntityActor* pAttacker,  GTalentInfo* pTalentInfo )
{
	VALID(pTalentInfo);
	SendStats("Immuned", pOwner, pAttacker, "talent", pTalentInfo->m_nID);
}

void GCombatMonitorPort::OnDamage( GEntityActor* pOwner, MUID uidAttacker, int nDamage, EFFECT_SOURCE_TYPE nEffectSourceType, int nEffectSourceID )
{
	VALID(pOwner);
	VALID(m_pField);

	string strSourceType = "none";
	{
		if (nEffectSourceType == EST_TALENT)
			strSourceType = "talent";
		else if (nEffectSourceType == EST_BUFF)
			strSourceType = "buff";
	}

	SendStats("Damage", pOwner, m_pField->FindActor(uidAttacker), strSourceType.c_str(), nEffectSourceID, nDamage);
}

void GCombatMonitorPort::Update( float fDelta )
{
	if (m_bWillStop)
	{
		StopImpl();
	}
}





