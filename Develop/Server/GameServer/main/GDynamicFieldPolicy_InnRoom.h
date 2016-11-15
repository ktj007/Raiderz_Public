#ifndef _G_INN_ROOM_FIELD_GROUP_POLICY_H_
#define _G_INN_ROOM_FIELD_GROUP_POLICY_H_

#include "GDynamicFieldPolicy.h"

class GInnRoomInfo;

class GDynamicFieldPolicy_InnRoom : public GDynamicFieldPolicy, public MTestMemPool<GDynamicFieldPolicy_InnRoom>
{
private:
	MRegulator	m_expiredRegulator;

protected:
	virtual void OnCreate();	
	virtual void OnDestroy();
	virtual void OnUpdate(float fDelta);

	virtual DYNAMICFIELD_POLICY_TYPE GetType() override { return DPT_INN; }
public:
	GDynamicFieldPolicy_InnRoom(GDynamicFieldMaster* pOwner);
	virtual ~GDynamicFieldPolicy_InnRoom();
};

#endif
