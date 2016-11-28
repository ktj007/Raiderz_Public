#include "stdafx.h"
#include "CSEffectInfo.h"

CSEffectInfo::CSEffectInfo()
: m_nPoint(POINT_TARGET)
, m_fRadius(0.0f)
, m_nRelation(RELATION_NONE)
, m_nLimit(INT_MAX)
, m_fDelay(0.f)
{

}

CSEffectInfo::CSEffectInfo(POINT nPoint, float fRadius, RELATION nRelation, int nLimit, float fDelay)
: m_nPoint(nPoint)
, m_fRadius(fRadius)
, m_nRelation(nRelation)
, m_nLimit(nLimit)
, m_fDelay(fDelay)
{
	
}