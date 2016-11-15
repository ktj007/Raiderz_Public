#pragma once

#include "XCharacterSerializeStrategy.h"

class XNPCSerializeStrategy : public XCharacterSerializeStrategy
{
private:
	wstring		m_strBPartNodeName;

protected:
	std::wstring GetMeshName( XNPCInfo* pNPCInfo );
	std::wstring GetItemMeshName( CSItemData* pItemData );
	void EquipNPCItem( XNPCPartsSlotType nLookSlot, CSItemData* pItemData );

	void SetNPCInfoSubMtrl( int NPCID );
	void SerializeParts( XNPCInfo* pNPCInfo );

public:
	XNPCSerializeStrategy(XCharacter* pActor, bool bResourceLoading);
	virtual ~XNPCSerializeStrategy() {}
	virtual void OnLoadingComplete();
	void Serialize( XNPCInfo* pNPCInfo, float fScale );
	void BPartSerialize(XNPCInfo* pNPCInfo, std::wstring strMeshNodeName, std::wstring strActorNodeName);

	virtual XCharacterSerializeStrategyType GetType() { return CSST_NPC; }
};

