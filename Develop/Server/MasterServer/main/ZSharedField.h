#pragma once

#include "ZField.h"

class ZSharedField : public ZField
{
public:	static const int LIMIT_PLAYER_RATE_FOR_QUIET_CHANNEL = 25;
public:
	ZSharedField(MUID uid, ZFieldInfo* pFieldInfo, int nChannelID, bool bDefaultChannel, int m_nGameServerID);
	~ZSharedField();

	int		GetChannelID()			{ return m_nChannelID; }
	int		GetPlayerLimit() override;

	bool	IsDefaultChannel()		{ return m_bIsDefaultChannel; }
	bool	IsQuiet();
	bool	IsEnterable();

private:
	int		m_nChannelID;
	bool	m_bIsDefaultChannel;
};
