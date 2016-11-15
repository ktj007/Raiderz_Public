#include "stdafx.h"
#include "RLODMetricInfo.h"
#include "RXMLTemplate.h"



namespace rs3
{

using namespace TXML;

RLODMetricInfo::RLODMetricInfo()
: m_fHysteresis(0)
{

}

RLODMetricInfo::~RLODMetricInfo()
{
	ClearMetricList();
}


bool RLODMetricInfo::ClearMetricList()
{
	m_vMetricList.clear();
	return true;
}

int RLODMetricInfo::GetMetricListSize()
{
	return m_vMetricList.size();
}

float RLODMetricInfo::GetMetric( int nIdx)
{
	_ASSERT( nIdx < (int)m_vMetricList.size());
	return m_vMetricList[nIdx];
}


bool RLODMetricInfo::AddMetricFromXML( MXmlElement* pMetricNode)
{
	float fDistance = _GETElement<float>::Value( pMetricNode,"DISTANCE","float");
	m_vMetricList.push_back( fDistance);

	return true;
}

bool RLODMetricInfo::SetHysteresisFromXML( MXmlElement* pMetricNode)
{
	float fDistance = _GETElement<float>::Value( pMetricNode,"DISTANCE","float");
	m_fHysteresis = fDistance;

	return true;
}

}