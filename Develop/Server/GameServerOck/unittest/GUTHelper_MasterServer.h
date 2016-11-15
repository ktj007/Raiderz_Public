#pragma once

class GUTHelper_MasterServer
{
public:
	GUTHelper_MasterServer();
	~GUTHelper_MasterServer();

	void AddNewSharedField(MUID uidField, int nFieldID, int nChannelID, int nGameServerID=0);
};
