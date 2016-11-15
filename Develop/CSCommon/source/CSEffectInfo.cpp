#include "stdafx.h"
#include "CSEffectInfo.h"

CSEffectInfo::CSEffectInfo()
: m_nPoint(POINT_TARGET)
, m_fRadius(0.0f)
, m_nRelation(RELATION_NONE)
, m_nLimit(INT_MAX)
{

}

CSEffectInfo::CSEffectInfo(POINT nPoint, float fRadius, RELATION nRelation, int nLimit)
: m_nPoint(nPoint)
, m_fRadius(fRadius)
, m_nRelation(nRelation)
, m_nLimit(nLimit)
{
	
}