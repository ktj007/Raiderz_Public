#pragma once

class GEntitySync;
class GBuffInfo;

#include "GBuffObserver.h"


//////////////////////////////////////////////////////////////////////////
//
//	GBuffScript
//
//////////////////////////////////////////////////////////////////////////

class GBuffScript : public GBuffObserver, public MTestMemPool<GBuffScript>
{
public:
	virtual void OnGain(MUID uidUser, GEntitySync* pTarget, GBuffInfo* pBuffInfo)		override;
	virtual void OnDuplicated(MUID uidUser, GEntitySync* pTarget, GBuffInfo* pBuffInfo)		override;

	virtual void OnLost(MUID uidUser, GEntitySync* pTarget, GBuffInfo* pBuffInfo, bool bRoute)	override;
	virtual void OnExpired(MUID uidUser, GEntitySync* pTarget, GBuffInfo* pBuffInfo)	override;

	static void MakeTable(int nBuffID);
	static void RunScript();

private:
	void CallEventScript(MUID uidUser, GEntitySync* pTarget, GBuffInfo* pBuffInfo, const wstring& strFuncName);
};
