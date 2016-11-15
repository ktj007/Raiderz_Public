#include "StdAfx.h"
#include "MMXml.h"

using namespace std;

namespace mmonitor {

MMXml::MMXml(void)
{
}

MMXml::~MMXml(void)
{
}


bool MMXml::LoadFile(const char* szFileName)
{
	return m_Document.LoadFile(szFileName);
}


bool _Attribute(char* pOutValue, TiXmlElement* pElement, const char* name)
{
	const char* szAttr = pElement->Attribute(name);
	if (szAttr)
	{
		strcpy(pOutValue, szAttr);
	}
	else
	{
		return false;
	}

	return true;
}

bool _Attribute(unsigned int* pOutValue, TiXmlElement* pElement, const char* name)
{
	const char* szAttr = pElement->Attribute(name);
	if (szAttr)
	{
		try
		{
			*pOutValue = atoi(szAttr);
		}
		catch(...)
		{
			return false;
		}
	}
	else
	{
		return false;
	}

	return true;
}


bool _Attribute(std::string& strOutValue, TiXmlElement* pElement, const char* name)
{
	const char* szAttr = pElement->Attribute(name);
	if (szAttr)
	{
		strOutValue = string(szAttr);
	}
	else
	{
		return false;
	}

	return true;
}

bool _Contents(std::string& strOutValue, TiXmlElement* pElement)
{
	if(!pElement)
		return false;
	const char* szText = pElement->GetText();
	if (szText)
	{
		strOutValue = string(szText);
	}
	else
	{
		return false;
	}

	return true;
}

}