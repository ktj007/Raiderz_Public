#pragma once

class MockLink;
class GEntityPlayer;

class GUTHelper_Network
{
public:
	// 货肺款 冈傅农 积己
	MockLink* NewLink(GEntityPlayer* pPlayer, bool bIgnoreAllSendCommandID = false);
	// 积己等 葛电 冈傅农甫 沥府
	void ClearLinks();
};
