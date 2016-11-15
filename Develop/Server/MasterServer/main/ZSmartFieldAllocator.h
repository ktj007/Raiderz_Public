#pragma once

class ZFieldManager;
class ZSharedField;
class ZSharedFieldMaster;
class ZGameServerObject;
enum DYNAMIC_FIELD_TYPE;

/// 필드를 어느 서버에 할당할지 결정해주는 클래스
class ZSmartFieldAllocator
{
private:
protected:
	ZFieldManager*		m_pFieldManager;

	ZGameServerObject*	GetRecommendedGameServer_ToCreateSharedField(ZSharedFieldMaster* pFieldMaster);
	ZGameServerObject*	GetRecommendedGameServer_ToCreateFieldGroup(MUID uidRequestPlayer);

public:
	ZSmartFieldAllocator(ZFieldManager* pFieldManager) : m_pFieldManager(pFieldManager) {}
	~ZSmartFieldAllocator() {}

	ZSharedField*		AllocNewChannel(ZSharedFieldMaster* pFieldMaster);
	ZGameServerObject*	AllocNewDynamicFieldGroup(MUID uidRequestPlayer, int nFieldGroupID, DYNAMIC_FIELD_TYPE eType, MUID* outuidCreatedFieldGroup);

};