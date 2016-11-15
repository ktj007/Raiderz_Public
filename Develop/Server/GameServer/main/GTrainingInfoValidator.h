#ifndef _G_TRAINING_INFO_VALIDATOR_H_
#define _G_TRAINING_INFO_VALIDATOR_H_

#include "CSBaseValidator.h"

class GTrainingInfo;

class GTrainingInfoValidator : public CSBaseValidator
{
public:
	void CheckTraining(GTrainingInfo* pTrainingInfo);

public:
	virtual bool Check();
};

#endif//_G_TRAINING_INFO_VALIDATOR_H_
