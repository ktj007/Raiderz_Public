#ifndef MINMAX_H
#define MINMAX_H

//#include <CommonFuncs.h>

template <class T> class CMinMax
{
public:
	CMinMax() { m_Min = T(); m_Max = T(); }
	CMinMax(T tMin, T tMax) { m_Min = tMin; m_Max = tMax; }
	~CMinMax() { }

	T m_Min;
	T m_Max;

	T GetRandomNumInRange(void) const { return(RandomNumber(m_Min, m_Max)); }
	T GetRange(void) const { return(abs(m_Max-m_Min)); }	
};

#endif