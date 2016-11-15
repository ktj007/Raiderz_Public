#pragma once

#include "GValidatorLib.h"
#include "CSBaseValidator.h"

class CSCondition_Component;
class GFieldInfo;

class GVALIDATOR_API GTrialFieldValidator : public CSBaseValidator
{
public:
	virtual bool Check();

private:
	bool IsCheckPossible(void);
	void CheckQuestInfo(void);	
	void CheckTrialType(int nSingleQuestID, int nFieldGroupID);
	void CheckWarpSensor(void);
	void CheckWarpQuest(int nQuestID, int nFieldGropuID);
	void CheckSensorWarpInfos(GFieldInfo* pFieldInfo);
};
