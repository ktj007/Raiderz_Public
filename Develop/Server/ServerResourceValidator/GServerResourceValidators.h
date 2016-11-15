#pragma once

#include "CSBaseValidator.h"
#include "GValidatorLib.h"

class GVALIDATOR_API GServerResourceValidators: public CSBaseValidator
{
public:
	GServerResourceValidators();
	virtual ~GServerResourceValidators();

	virtual bool Check(void);

public:	
	void AddValidator(CSBaseValidator* pValidator);

private:
	vector<CSBaseValidator*>	m_vecValidators;
};
