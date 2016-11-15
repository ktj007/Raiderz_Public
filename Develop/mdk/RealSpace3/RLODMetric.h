#pragma once

namespace rs3
{

class RLODMetricInfo;

class RS_API RLODMetric
{
private:
	RLODMetricInfo* m_pMetricInfo;

public:
	RLODMetric();
	~RLODMetric();

	bool CreateMetricInfoFromXML(const char* fname);
	RLODMetricInfo* GetLODMetricInfo()	{ return m_pMetricInfo; }
private:
	bool CreateMetricInfo();
	bool DeleteMetricInfo();
	bool ClearMetricInfo();
};

}