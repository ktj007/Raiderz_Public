#ifndef _G_CONDITIONS_INFO_VALIDATOR_H_
#define _G_CONDITIONS_INFO_VALIDATOR_H_

#include "GValidatorLib.h"
#include "GDef.h"
#include "CSBaseValidator.h"

class GConditionsInfo;
class GCondition_Leaf;
class GCondition_Composite;

class GVALIDATOR_API GConditionsInfoValidator : public CSBaseValidator
{
private:
	void CheckConditions(GConditionsInfo* pConditionsInfo);
	void CheckLeaf(GCondition_Leaf* pConditionLeaf);
	void CheckComposite(GCondition_Composite* pConditionComposite);

public:
	virtual bool Check();
};

#endif//_G_CONDITIONS_INFO_VALIDATOR_H_
