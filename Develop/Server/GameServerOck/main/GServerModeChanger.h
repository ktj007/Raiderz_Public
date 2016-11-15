#pragma once

class GServerModeChanger
{
public:
	GServerModeChanger();
	~GServerModeChanger();

	void ChangeServerMode(int nServerMode);

	void ChangeServerModeImpl(int nServerMode);
	void RequestToMasterServer(int nServerMode);
};
