#pragma once

#include "MTypes.h"
#include "MUID.h"
#include "MReferenceCounted.h"
#include "ZFieldInfo.h"


class ZField : public MReferenceCounted
{
protected:
	MUID			m_UID;
	int				m_nGameServerID;
	ZFieldInfo*		m_pFieldInfo;
	int				m_nPlayerCount;
	
public:
	ZField(MUID uid, ZFieldInfo* pFieldInfo, int m_nGameServerID);
	virtual ~ZField();

	MUID			GetUID()				{ return m_UID; }
	int				GetID()					{ return m_pFieldInfo->m_nFieldID;	};
	int				GetGameServerID() const	{ return m_nGameServerID; }
	ZFieldType		GetType();
	ZFieldInfo*		GetFieldInfo()			{ return m_pFieldInfo; }

	void			IncreasePlayerCount();
	void			DecreasePlayerCount();
	virtual int		GetPlayerCount()		{ return m_nPlayerCount; }
	virtual int		GetPlayerLimit();

};
