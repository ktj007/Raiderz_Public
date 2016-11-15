#pragma once

class GGMSetNPCHandler
{
public:
	bool Handle(GEntityPlayer* pPlayer, wstring strType, wstring strValue);	

private:
	void HP(GEntityPlayer* pPlayer, int nPercent);
};
