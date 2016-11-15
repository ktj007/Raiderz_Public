#pragma once

#include "MReferenceCounted.h"
#include "SChecksum.h"

class GFieldInfo;
class GProxyField;
class GProxySharedFieldMaster;
class GProxyDynamicFieldMaster;
struct FIELD_GROUP_INFO;

class GProxyFieldManager : public MTestMemPool<GProxyFieldManager>
{
	typedef map<MUID, GProxyField*>					GProxyFieldMap;
	typedef map<int, GProxySharedFieldMaster*>		GProxySharedFieldMasterMap;
	typedef map<MUID, GProxyDynamicFieldMaster*>	GProxyDynamicFieldMasterMap;
public:
	GProxyFieldManager();
	virtual ~GProxyFieldManager();

	GProxyField*				Find(MUID uidProxyField);
	GProxyField*				FindSharedField(int nFieldID, int nChannelID);
	GProxySharedFieldMaster*	FindSharedFieldMaster(int nFieldID);
	GProxyDynamicFieldMaster*	FindFieldGroup(MUID uidFieldGroup);

	GProxyField*				AddSharedField(MUID uidField, GFieldInfo* pFieldInfo, int nGameServerID, int nChannelID=0);
	void						DeleteSharedField(MUID uidField);

	GProxyDynamicFieldMaster*	AddFieldGroup(MUID uidFieldGroup, FIELD_GROUP_INFO* pFieldGroupInfo, int nGameServerID, DYNAMIC_FIELD_TYPE eDynamicFieldType);
	bool						AddDynamicField(MUID uidField, GFieldInfo* pFieldInfo, MUID uidOwnerFieldGroup);
	void						DeleteFieldGroup(MUID uidFieldGroup);

	void						ClearProxySharedFields();
	void						ClearProxyFieldGroups();
	void						ClearProxyFields();

	int							GetFieldCount()			{ return (int)m_FieldMap.size(); }
	int							GetFieldGroupCount()	{ return (int)m_FieldGroupMap.size(); }

	uint32 CalcChecksumSharedFields();
	uint32 CalcChecksumFieldGroups();

	bool IsExistChannel(int nFieldID, CHANNELID nChannelID);

private:
	GProxyFieldMap				m_FieldMap;

	GProxySharedFieldMasterMap	m_SharedFieldMasterMap;
	SFieldChecksum				m_ChecksumSharedFields;

	GProxyDynamicFieldMasterMap	m_FieldGroupMap;
	SFieldChecksum				m_ChecksumFieldGroups;

	void						AddField(GProxyField* pProxyField);
	void						DeleteField(MUID uidProxyField);

	GProxySharedFieldMaster*	CreateSharedFIeldMaster(int nFieldID);
};