#include "stdafx.h"
#include "RLODMetric.h"
#include "RLODMetricInfo.h"

namespace rs3
{

RLODMetric::RLODMetric()
: m_pMetricInfo(NULL)
{

}

RLODMetric::~RLODMetric()
{
	DeleteMetricInfo();
}

bool RLODMetric::CreateMetricInfo()
{
	if(!m_pMetricInfo)
		m_pMetricInfo = new RLODMetricInfo;

	return true;
}


bool RLODMetric::DeleteMetricInfo()
{
	SAFE_DELETE( m_pMetricInfo);
	return true;
}

bool RLODMetric::ClearMetricInfo()
{
	if( m_pMetricInfo)
		m_pMetricInfo->ClearMetricList();

	return true;
}

bool RLODMetric::CreateMetricInfoFromXML(const char* fname)
{
	char szDescName[256];

	strcpy(szDescName,fname);
	strcat(szDescName,".lod.xml");

	MXml aXml;
	if(!aXml.LoadFile(szDescName))
		return false;

	CreateMetricInfo();
	ClearMetricInfo();

	////////////////////////////////////////////////////

	MXmlElement*	pElement;
	MXmlHandle		docHandle = aXml.DocHandle();

	pElement = docHandle.FirstChild("PROJECTIONLIST").FirstChildElement().Element();

	if(pElement)
	{
		for(pElement; pElement != NULL; pElement=pElement->NextSiblingElement())
		{
			if (!_stricmp(pElement->Value(), "PROJECTION"))
			{
				int tempProj = 0;
				if (!_Attribute(tempProj, pElement, "index")) 
					continue;

				m_pMetricInfo->m_iProjectionIndex = tempProj;
			}
		}
	}
	else
	{
		MXmlElement* pMetricElement = docHandle.FirstChild("METRICLIST").FirstChildElement().Element();

		if(pMetricElement)
		{
			MXmlElement* pMetricNode;
			char szTagName[256];
			pMetricNode = pMetricElement->FirstChildElement();
			for(pMetricNode; pMetricNode != NULL; pMetricNode = pMetricNode->NextSiblingElement())
			{
				strcpy(szTagName, pMetricNode->Value());
				if(stricmp(szTagName,"METRIC")==0)
				{
					m_pMetricInfo->AddMetricFromXML( pMetricNode);
				}
			}
		}

	}

	/*MXmlElement* pProjectionEle = pChild->FirstChildElement("PROJECTIONLIST");
	if(pProjectionEle)
	{
		MXmlElement* pProjectionElement = pChild->NextSiblingElement("PROJECTION");
		if(pProjectionElement)
		{
			int tempProjIdx = 0;
			if(_Attribute(tempProjIdx, pProjectionElement, "index"))
			{
				m_pMetricInfo->m_iProjectionIndex = tempProjIdx;
				//More?
			}
		}


		MXmlElement* pMetricElement = pChild->NextSiblingElement("METRICLIST");
		if(pMetricElement)
		{
			MXmlElement* pMetricNode;
			char szTagName[256];
			pMetricNode = pMetricElement->FirstChildElement();
			for(pMetricNode; pMetricNode != NULL; pMetricNode = pMetricNode->NextSiblingElement())
			{
				strcpy(szTagName, pMetricNode->Value());
				if(stricmp(szTagName,"METRIC")==0)
				{
					m_pMetricInfo->AddMetricFromXML( pMetricNode);
				}
			}
		}

		MXmlElement* pHysteresisElement = pChild->NextSiblingElement("HYSTERESIS");
		if(pHysteresisElement)
		{
			m_pMetricInfo->SetHysteresisFromXML( pHysteresisElement);
		}
	}*/

	return true;
}



}