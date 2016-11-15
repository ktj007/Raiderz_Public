#pragma once

#include <vector>

namespace rs3
{

class RS_API RLODMetricInfo
{
private:

public:
	RLODMetricInfo();
	~RLODMetricInfo();

public:
	std::vector<float>	m_vMetricList;
	float m_fHysteresis;
	int m_iProjectionIndex;

	bool ClearMetricList();
	bool AddMetricFromXML( MXmlElement* pMetricNode);
	bool SetHysteresisFromXML( MXmlElement* pMetricNode);

	int GetMetricListSize();
	float GetMetric( int nIdx);

};


}