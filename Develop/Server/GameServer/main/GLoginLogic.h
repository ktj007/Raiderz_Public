#pragma once

class GPlayerObject;
class GPlayerAcceptor;

class GLoginLogic
{
public:
	bool CheckCommandVersion(MUID uidPlayer, int nCommandVersion);

	bool LoginGameServer(MUID uidPlayer, MUID uidConnectionKey, bool bOnPmang=false, wstring strPmangStatIndex=L"", int nPCCafeID = 0);
	bool MoveGameServer(MUID uidPlayerSession, MUID uidConnectionKey, bool bOnPmang=false, wstring strPmangStatIndex=L"", int nPCCafeID = 0);
	void AddPmangStatIndex(MUID uidPlayer, wstring strStatIndex, int nPCCafeID);

	bool DuplicatedLogin(MUID uidPlayer, MUID uidConnectionKey);

private:
	GPlayerObject* NewPlayerObjectAndInitAccount( MUID uidPlayer, GPlayerAcceptor* pAcceptor );
	
};
