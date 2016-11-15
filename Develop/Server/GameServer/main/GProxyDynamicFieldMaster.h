#pragma once

class GProxyField;
struct FIELD_GROUP_INFO;
struct TDMG_PROXY_DYNAMIC_FIELD_UPDATE_INFO;
enum DYNAMIC_FIELD_TYPE;

class GProxyDynamicFieldMaster : public MTestMemPool<GProxyDynamicFieldMaster>
{
public:
	typedef vector<GProxyField*>	GDynamicFieldVector;
private:
	MUID						m_UID;
	FIELD_GROUP_INFO*			m_pFieldGroupInfo;
	int							m_nGameServerID;
	DYNAMIC_FIELD_TYPE			m_eDynamicFieldType;
	GDynamicFieldVector			m_FieldVector;

	void Clear();

public:
	GProxyDynamicFieldMaster(MUID uidFieldGroup, FIELD_GROUP_INFO* pFieldGroupInfo, int nGameServerID, DYNAMIC_FIELD_TYPE eDynamicFieldType);
	virtual ~GProxyDynamicFieldMaster();

	MUID					GetUID() const					{ return m_UID; }
	int						GetGameServerID() const			{ return m_nGameServerID; }
	DYNAMIC_FIELD_TYPE		GetDynamicFieldType() const		{ return m_eDynamicFieldType; }
	FIELD_GROUP_INFO*		GetInfo()						{ return m_pFieldGroupInfo; }
	int						GetPlayerCount();

	GDynamicFieldVector&	GetDynamicFieldVector()			{ return m_FieldVector; }

	void					AddDynamicField(GProxyField* pProxyDynamicField);
};
