#pragma once

typedef map<MUID, vector<int>> MAP_RECEIEVE_GAMESERVER_UID;

class ZReceiveGameServerUIDSelector
{
public:
	static void Select(const vector<int>& vecCID, MAP_RECEIEVE_GAMESERVER_UID& outmapReceiveServerUID);
	static void Select(int nCID, MAP_RECEIEVE_GAMESERVER_UID& outmapReceiveServerUID);
};
