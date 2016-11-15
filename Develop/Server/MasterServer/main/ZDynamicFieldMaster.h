#pragma once

#include "MReferenceCounted.h"

struct FIELD_GROUP_INFO;
class ZDynamicField;
enum DYNAMIC_FIELD_TYPE;

class ZDynamicFieldMaster : public MReferenceCounted
{
public:
	typedef vector<ZDynamicField*> VEC_DYNAMICFIELD;

private:
	MUID				m_UID;
	FIELD_GROUP_INFO*	m_pInfo;
	int					m_nGameServerID;
	DYNAMIC_FIELD_TYPE	m_eDynamiicFieldType;
	MUID				m_uidCreator;
	VEC_DYNAMICFIELD	m_vecDynamicField;

	void				Clear();

public:
	ZDynamicFieldMaster(MUID uidNew, FIELD_GROUP_INFO* pInfo, int nGameServerID, DYNAMIC_FIELD_TYPE eDynamiicFieldType, MUID uidCreator);
	~ZDynamicFieldMaster();

	void				Insert(ZDynamicField* pField);

	MUID				GetUID()			{ return m_UID; }
	int					GetGameServerID()	{ return m_nGameServerID; }
	FIELD_GROUP_INFO*	GetInfo()			{ return m_pInfo; }
	DYNAMIC_FIELD_TYPE	GetType()			{ return m_eDynamiicFieldType; }
	MUID				GetCreatorUID()		{ return m_uidCreator; }
	int					GetPlayerCount();

	VEC_DYNAMICFIELD&	GetFieldVector()	{ return m_vecDynamicField; }
	ZDynamicField*		FindField(int nDynamicFieldID);
};
