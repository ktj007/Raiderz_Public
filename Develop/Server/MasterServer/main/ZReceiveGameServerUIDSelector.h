#pragma once

typedef map<MUID, vector<CID>> MAP_RECEIEVE_GAMESERVER_UID;

class ZReceiveGameServerUIDSelector
{
public:
	static void Select(const vector<CID>& vecCID, MAP_RECEIEVE_GAMESERVER_UID& outmapReceiveServerUID);
	static void Select(CID nCID, MAP_RECEIEVE_GAMESERVER_UID& outmapReceiveServerUID);
};
