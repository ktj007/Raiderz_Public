#include "stdafx.h"
#include "GServerResourceValidators.h"
#include "CSBaseValidator.h"

GServerResourceValidators::GServerResourceValidators()
{
	// do nothing
}

GServerResourceValidators::~GServerResourceValidators()
{
	// do nothing
}

bool GServerResourceValidators::Check(void)
{
	bool result = true;

	for each (CSBaseValidator* validator in m_vecValidators)
	{
		if (!validator->Check())
		{
			result = false;
		}
	}

	return result;
}

void GServerResourceValidators::AddValidator(CSBaseValidator* pValidator)
{
	_ASSERT(pValidator != NULL);
	m_vecValidators.push_back(pValidator);
}
