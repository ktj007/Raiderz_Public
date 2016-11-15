#pragma once

#include "GValidatorLib.h"
#include "CSBaseValidator.h"

class GQPEventInfo;
class GQPEBeginInfo;
class GQPEEndInfo;
class GQPESpawnInfo;
class GQPEItemInfo;

class GVALIDATOR_API GQPEventInfoValidator : public CSBaseValidator
{
public:
	void CheckEvent(GQPEventInfo* pInfo);
	void CheckBegin(const GQPEBeginInfo &beginInfo);
	void CheckEnd(GQPEEndInfo* pEndInfo);

	void CheckNotice(wstring strNotice);
	void CheckSpawn(GQPESpawnInfo* pSpawnInfo);
	void CheckDespawn(const vector<int>& vecDespawnNPCID);
	void CheckReward(GQPEEndInfo* pEndInfo);
	void CheckItem(GQPEItemInfo* pItemInfo);	

public:
	virtual bool Check();
};
