#pragma once

#include "CSDef.h"

#pragma pack(push, old)
#pragma pack(1)


struct TDAG_CHAT_CHANNEL_INFO
{
	MUID		uidChannel;
	wchar_t		strChannelName[MAX_CHATTING_CHANNEL_NAME_LEN + 1];
};

struct TDAG_CHAT_CHANNEL_PLAYER_INFO
{
	MUID		uidChannel;
	int			nCID;
	int			nGameServer;
};


#pragma pack(pop, old)
