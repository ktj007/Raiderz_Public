#pragma once

#include "XResourceManager.h"
#include "XCharacterSerializeStrategy.h"

class XCharacter;
class XModuleEntity;

class XCharacterSerializer
{
private:
	XCharacterSerializeStrategy*		m_pSerializeStrategy;

	XModuleEntity*			m_pModuleEntity;
	XCharacter*				m_pActor;
	bool					m_bLoadingCompleted;

	void CheckLoadingComplete();
	void OnLoadingComplete();
	void PreSerialize();

	XCharacterSerializeStrategy* GetStrategy(XCharacterSerializeStrategyType nStrategyType);
public:
	XCharacterSerializer(XModuleEntity* pModuleEntity, XCharacter* pActor = NULL );
	~XCharacterSerializer();

	void SerializeMyPlayer(PLAYER_INFO& playerInfo, XMyEquipmentSlot* pEquipmentSlot);
	void SerializeNetPlayer(XNetPlayer* pNetPlayer, XPlayerInfoFeature& feature, XResourceLoadingPolicy policy);
	void SerializeNPC(XNPCInfo* pNPCInfo, float fScale, XResourceLoadingPolicy policy);
	void SerializeBPartNPC(XNPCInfo* pNPCInfo, std::wstring strMeshNodeName, std::wstring strActorNodeName, XResourceLoadingPolicy policy);

	void Update();
	wstring GetMeshName();
};
