#pragma once

#include "GValidatorLib.h"

class CSBaseValidator;
class GServerResourceValidators;

class GVALIDATOR_API GValidatorSilo
{
public:
	GValidatorSilo();
	virtual ~GValidatorSilo();

	CSBaseValidator* GetAllCheckValidator(void);

private:
	GServerResourceValidators*	m_pAllCheckValidator;
	vector<CSBaseValidator*>	m_vecValidator;
};

bool GValidateResouces();

