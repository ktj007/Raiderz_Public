#include "stdafx.h"
#include "XCharacterSerializer.h"
#include "XCharacter.h"
#include "XModuleEntity.h"
#include "RBackgroundWorker.h"
#include "XSystem.h"
#include "XGlobal.h"
#include "XPlayerSerializeStrategy.h"
#include "XNPCSerializeStrategy.h"


XCharacterSerializer::XCharacterSerializer(XModuleEntity* pModuleEntity, XCharacter* pActor )
: m_pModuleEntity(pModuleEntity), m_pSerializeStrategy(NULL), m_bLoadingCompleted(false)
{
	if( pActor )
		m_pActor		= pActor;
	else
		m_pActor = pModuleEntity->GetActor();
}

XCharacterSerializer::~XCharacterSerializer()
{
	SAFE_DELETE(m_pSerializeStrategy);
}

void XCharacterSerializer::SerializeMyPlayer(PLAYER_INFO& playerInfo, XMyEquipmentSlot* pEquipmentSlot)
{
	PreSerialize();

	XCharacterSerializeStrategy* pStrategy = GetStrategy(CSST_MY_PLAYER);
	pStrategy->SetLoadingPolicy(LOADING_SYNC);

	(dynamic_cast<XMyPlayerSerializeStrategy*>(pStrategy))->Serialize(playerInfo, pEquipmentSlot);
}

void XCharacterSerializer::SerializeNetPlayer( XNetPlayer* pNetPlayer, XPlayerInfoFeature& feature, XResourceLoadingPolicy policy )
{
	PreSerialize();

	XCharacterSerializeStrategy* pStrategy = GetStrategy(CSST_NET_PLAYER);
	pStrategy->SetLoadingPolicy(policy);

	(dynamic_cast<XNetPlayerSerializeStrategy*>(m_pSerializeStrategy))->Serialize(pNetPlayer, feature);
}


void XCharacterSerializer::SerializeNPC( XNPCInfo* pNPCInfo, float fScale, XResourceLoadingPolicy policy )
{
	PreSerialize();

	XCharacterSerializeStrategy* pStrategy = GetStrategy(CSST_NPC);
	pStrategy->SetLoadingPolicy(policy);

	(dynamic_cast<XNPCSerializeStrategy*>(m_pSerializeStrategy))->Serialize(pNPCInfo, fScale);

	if (global.system->IsResourceLoading() == false)
	{
		// 백그라운드 로딩을 하지 않을 경우
		OnLoadingComplete();
	}
	else
	{
		_ASSERT( m_pActor );
	}
}


void XCharacterSerializer::Update()
{
	CheckLoadingComplete();
}

void XCharacterSerializer::CheckLoadingComplete()
{
	if (m_bLoadingCompleted) return;

	if (m_pActor->IsLoadingCompleted())
	{
		OnLoadingComplete();
		m_bLoadingCompleted = true;
	}
}

void XCharacterSerializer::OnLoadingComplete()
{
	if (m_pSerializeStrategy) m_pSerializeStrategy->OnLoadingComplete();

	m_pModuleEntity->OnLoadingComplete();
}

wstring XCharacterSerializer::GetMeshName()
{
	if (m_pSerializeStrategy)
	{
		return m_pSerializeStrategy->GetMeshName();
	}
	return wstring();
}

XCharacterSerializeStrategy* XCharacterSerializer::GetStrategy(XCharacterSerializeStrategyType nStrategyType)
{
	if (m_pSerializeStrategy && m_pSerializeStrategy->GetType() != nStrategyType)
	{
		delete m_pSerializeStrategy;
		m_pSerializeStrategy = NULL;
	}

	if (m_pSerializeStrategy == NULL)
	{
		switch (nStrategyType)
		{
		case CSST_MY_PLAYER: m_pSerializeStrategy = new XMyPlayerSerializeStrategy(m_pActor, global.system->IsResourceLoading()); break;
		case CSST_NET_PLAYER: m_pSerializeStrategy = new XNetPlayerSerializeStrategy(m_pActor, global.system->IsResourceLoading()); break;
		case CSST_NPC: m_pSerializeStrategy = new XNPCSerializeStrategy(m_pActor, global.system->IsResourceLoading()); break;
		default: _ASSERT(0);
		}
	}
	return m_pSerializeStrategy;
}

void XCharacterSerializer::PreSerialize()
{
	m_bLoadingCompleted = false;
}

void XCharacterSerializer::SerializeBPartNPC( XNPCInfo* pNPCInfo, std::wstring strMeshNodeName, std::wstring strActorNodeName, XResourceLoadingPolicy policy )
{
	PreSerialize();

	XCharacterSerializeStrategy* pStrategy = GetStrategy(CSST_NPC);

	pStrategy->SetLoadingPolicy(policy);

	(dynamic_cast<XNPCSerializeStrategy*>(m_pSerializeStrategy))->BPartSerialize(pNPCInfo, strMeshNodeName, strActorNodeName);

	if (global.system->IsResourceLoading() == false)
	{
		// 백그라운드 로딩을 하지 않을 경우
		OnLoadingComplete();
	}
	else
	{
		_ASSERT( m_pActor );
	}
}
