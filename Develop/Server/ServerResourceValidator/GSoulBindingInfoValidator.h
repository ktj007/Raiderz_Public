#ifndef _G_SOULBINDING_INFO_VALIDATOR_H_
#define _G_SOULBINDING_INFO_VALIDATOR_H_

#include "GValidatorLib.h"
#include "CSBaseValidator.h"

class GSoulBindingInfo;

class GVALIDATOR_API GSoulBindingInfoValidator : public CSBaseValidator
{
public:
	void CheckSoulBinding(GSoulBindingInfo* pSoulBindingInfo);

public:
	virtual bool Check();
};

#endif//_G_SOULBINDING_INFO_VALIDATOR_H_
