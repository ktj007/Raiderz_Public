#pragma once

#include "MMath.h"

class XUIDGenerator
{
private:
	MUID	m_UID;
public:
	XUIDGenerator() 
	{
		m_UID.High = UINT_MAX - RandomNumber(0, 10000);
		m_UID.Low = 0;
	}
	MUID Make()
	{
		return (++m_UID);
	}
};