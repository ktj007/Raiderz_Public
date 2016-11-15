#pragma once

#include "GFieldInfo.h"

class GFieldInfoMgr;
class GFieldMgr;
class GTestSystem;
class GSharedField;
class GSharedFieldMaster;
class GServer;

class GTrialField;
class GInnRoomField;
class GDynamicFieldMaster;

class GFieldFactory : public MTestMemPool<GFieldFactory>
{
protected:
	GFieldMgr*			m_pFieldMgr;

public:
	GFieldFactory(GFieldMgr* pFieldMgr) : m_pFieldMgr(pFieldMgr) {}
	virtual ~GFieldFactory() {}


	/// 관리하고 있는 모든 공유 필드를 채널별로 생성한다.
	bool CreateAllSharedField(GFieldInfoMgr* pFieldInfoMgr, GTestSystem* pTestSystem, GServer* pServer);



	// 리팩터링 대상 --
	GSharedField* CreateSharedField( MUID uidNewField, int nFieldID, CHANNELID nNewChannelID );
	GSharedFieldMaster* CreateSharedFieldMaster( GFieldInfo* pFieldInfo );


	// New
	GTrialField* NewTrialField(MUID& uidNew, MUID uidGroup);
	GInnRoomField* NewInnRoomField(MUID& uidNew, MUID uidGroup);
	
	GDynamicFieldMaster* NewDynamicFieldMaster(const MUID& uidCreator, const DYNAMIC_FIELD_TYPE eType);
	GSharedFieldMaster* NewSharedFieldMaster(GFieldInfo* pFieldInfo);
};