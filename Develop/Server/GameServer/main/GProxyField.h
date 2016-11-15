#pragma once

#include "MReferenceCounted.h"
#include "SChecksum.h"

class GFieldInfo;

class GProxyField : public MReferenceCounted, public MTestMemPool<GProxyField>
{
protected:
	MUID			m_UID;
	GFieldInfo*		m_pFieldInfo;
	int				m_nGameServerID;
	int				m_nPlayerCount;

	bool			m_bDynamic;
	int				m_nChannelID;		///< SharedField 에서만 사용
	MUID			m_uidFieldGroup;	///< DynamicField 에서만 사용

public:
	GProxyField(MUID uidField, GFieldInfo* pFieldInfo, int nGameServerID);
	virtual ~GProxyField();

	void InitSharedField(int nChannelID)		{ m_bDynamic = false;	m_nChannelID = nChannelID; }
	void InitDynamicField(MUID uidFieldGroup)	{ m_bDynamic = true;	m_uidFieldGroup = uidFieldGroup; }

	MUID GetUID() const						{ return m_UID; }
	GFieldInfo* GetFieldInfo() const		{ return m_pFieldInfo; }
	int GetGameServerID() const				{ return m_nGameServerID; }
	int GetPlayerCount() const				{ return m_nPlayerCount; }
	int GetCapacity() const;
	bool IsDynamic() const					{ return m_bDynamic; }

	int GetChannelID() const				{ return m_nChannelID; }
	MUID GetFieldGroupUID() const			{ return m_uidFieldGroup; }

	void SetPlayerCount(int nCount);
	bool IsEnterable();
};