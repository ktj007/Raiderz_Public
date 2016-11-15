#ifndef _CS_BASE_VALIDATOR_H_
#define _CS_BASE_VALIDATOR_H_

#include "CSCommonLib.h"

class CSCOMMON_API CSBaseValidator
{
private:
	bool m_bReturnValue;
protected:
	void SetFailed()		{ m_bReturnValue; }
	bool GetReturnValue()	{ return m_bReturnValue; }
public:
	CSBaseValidator();
	virtual ~CSBaseValidator();

	virtual bool Check() = 0;
};

#endif//_G_BASE_VALIDATOR_H_