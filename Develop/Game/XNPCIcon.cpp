#include "StdAfx.h"
#include "XNPCIcon.h"
#include "XModuleEffect.h"

//static wstring _IconStateStr[] = 
//{
//	L"NIS_NONE",
//	L"NIS_ITEM_INTERACTION",
//	L"NIS_ONLY_INTERACTION",
//	L"NIS_QUEST_INTERACT",
//	L"NIS_LOOT",
//	L"NIS_TALK",
//	L"NIS_HELPER",
//	L"NIS_SHOP",
//	L"NIS_CRAFT",
//	L"NIS_QUEST_BEGIN",
//	L"NIS_QUEST_INTERACTION",
//	L"NIS_QUEST_ACCEPT",
//	L"NIS_QUEST_END",
//	L"NIS_QUEST_CONTINUE",
//	L"NIS_QUEST_REWARD",
//	L"NIS_COMBAT",
//	L"NIS_MAX"
//};

XNPCIcon::XNPCIcon(XModuleEffect* pModuleEffect)
: m_pModuleEffect(pModuleEffect)
{
	m_NIT		= NIT_NONE;
	m_NIS		= NIS_NONE;
	m_PrevNIS	= NIS_NONE;

	m_Regulator.SetTickTime(0.1f);
}

XNPCIcon::~XNPCIcon(void)
{
	m_pModuleEffect = NULL;
}

void XNPCIcon::OnUpdate(float fDelta)
{
	if(m_Regulator.IsReady(fDelta))
	{
		if(!m_NISBuffer.empty())
		{
			m_PrevNIS = m_NIS;
			m_NIS = *(m_NISBuffer.begin());
			m_NISBuffer.pop_front();

			CheckException();

			//mlog("current NIS : %s \n", MLocale::ConvUTF16ToAnsi(_IconStateStr[m_NIS]).c_str());
			SetNPCIconEffect(m_NIS);
		}
	}
}

void XNPCIcon::CheckException()
{
	if (!m_NISBuffer.empty())
	{
		if (m_PrevNIS == NIS_QUEST_INTERACTION &&
			m_NIS == NIS_QUEST_BEGIN &&
			*(m_NISBuffer.begin()) == NIS_NONE )
		{
			m_NIS = NIS_NONE;
			return;
		}

		if (m_PrevNIS == NIS_QUEST_END &&
			m_NIS == NIS_QUEST_BEGIN &&
			*(m_NISBuffer.begin()) == NIS_QUEST_REWARD)
		{
			m_PrevNIS = m_NIS;
			m_NIS = *(m_NISBuffer.begin());
			m_NISBuffer.pop_front();
			return;
		}

		if (m_PrevNIS == NIS_QUEST_REWARD &&
			m_NIS == NIS_QUEST_END )
		{
			m_PrevNIS = m_NIS;
			m_NIS = *(m_NISBuffer.begin());
			m_NISBuffer.pop_front();
			return;
		}
		
		if (m_PrevNIS == NIS_QUEST_INTERACTION &&
			m_NIS == NIS_QUEST_BEGIN &&
			*(m_NISBuffer.begin()) == NIS_QUEST_INTERACT)
		{
			m_PrevNIS = m_NIS;
			m_NIS = *(m_NISBuffer.begin());
			m_NISBuffer.pop_front();
			return;
		}
	}
}

void XNPCIcon::SetNPCIConTypeToIconState( NPC_ICON_TYPE NIT )
{
	m_NIT = NIT;

	NPC_ICON_STATE NIS = NIS_NONE;
	switch (NIT)
	{
	case NIT_NONE:				NIS = NIS_NONE;				break;
	case NIT_ITEM_UNUSABLE:		NIS = NIS_ITEM_UNUSABLE;	break;
	case NIT_ITEM_USABLE:		NIS = NIS_ITEM_USABLE;		break;
	case NIT_ONLYINTERACTION:	NIS = NIS_ONLY_INTERACTION;	break;
	case NIT_TRIGGER:			NIS = NIS_TRIGGER;			break;
	case NIT_QUEST_INTERACT:	NIS = NIS_QUEST_INTERACT;	break;
	case NIT_LOOT:				NIS = NIS_LOOT;				break;
	case NIT_TALK:				NIS = NIS_TALK;				break;
	case NIT_HELPER:			NIS = NIS_HELPER;			break;
	case NIT_SHOP:				NIS = NIS_SHOP;				break;
	case NIT_GUILD_CREATE:		NIS = NIS_GUILD_CREATE;		break;
	case NIT_CRAFT:				NIS = NIS_CRAFT;			break;
	case NIT_QUEST_NOT_BEGIN_ONLYICON:	NIS = NIS_QUEST_NOT_BEGIN_ONLYICON;	break;
	case NIT_QUEST_NOT_BEGIN:			NIS = NIS_QUEST_NOT_BEGIN;	break;
	case NIT_QUEST_CONTINUE_ONLYICON:
		NIS = NIS_QUEST_CONTINUE_ONLYICON;
		if (m_PrevNIS == NIS_QUEST_BEGIN)
		{
			NIS = NIS_QUEST_ACCEPT;
		}
		break;
	case NIT_QUEST_CONTINUE:
		NIS = NIS_QUEST_CONTINUE;
		if (m_PrevNIS == NIS_QUEST_BEGIN)
		{
			NIS = NIS_QUEST_ACCEPT;
		}
		break;
	case NIT_QUEST_BEGIN:		NIS = NIS_QUEST_BEGIN;		break;
	case NIT_QUEST_END:			NIS = NIS_QUEST_END;		break;
	case NIT_COMBAT:			NIS = NIS_COMBAT;			break;
	default:					_ASSERT(0);					break;
	}

	//mlog("NIT to NIS : %s \n", MLocale::ConvUTF16ToAnsi(_IconStateStr[NIS]).c_str());

	SetNPCIConState(NIS);
}

void XNPCIcon::SetNPCIConState( NPC_ICON_STATE NIS )
{
	if (!m_NISBuffer.empty() && NIS == m_NISBuffer.back())
	{
		// 중복이 있을리 없나...
		int i= 0;
	}
	
	m_NISBuffer.push_back(NIS);
}

void XNPCIcon::SetNPCIconEffect( NPC_ICON_STATE NIS )
{
	if(m_pModuleEffect == NULL) return;

	m_pModuleEffect->OnEffectEvent(XEFTEVT_EFFECT_NPC_ICON, NIS);

	if (m_PrevNIS == NIS_LOOT || m_PrevNIS == NIS_QUEST_INTERACT)
		m_pModuleEffect->OnEffectEvent(XEFTEVT_EFFECT_LOOTABLE_DEL);
}

void XNPCIcon::SetPreviousNPCIcon()
{
	//mlog("Previous NPCIcon : %s \n", MLocale::ConvUTF16ToAnsi(_IconStateStr[m_PrevNIS]).c_str());

	SetNPCIConTypeToIconState(m_NIT);
}