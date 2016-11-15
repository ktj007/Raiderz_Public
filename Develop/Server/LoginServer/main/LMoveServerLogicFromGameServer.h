#pragma once

class LMoveServerLogicFromGameServer
{
public:
	LMoveServerLogicFromGameServer(void);
	~LMoveServerLogicFromGameServer(void);

	bool MoveLoginServer(MUID uidPlayer, MUID uidConnectionKey, bool bOnPmang=false, wstring strPmangStatIndex=L"", int nPCCafeID = 0);

private:
	bool Check(MUID uidPlayer, MUID uidConnectionKey);
	void Apply(MUID uidPlayer, MUID uidConnectionKey);

	void AddPmangStatIndex(MUID uidPlayer, wstring strStatIndex, int nPCCafeID);
};
