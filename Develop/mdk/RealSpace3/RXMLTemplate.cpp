#include "stdafx.h"
#include "RXMLTemplate.h"
#include <tchar.h>

namespace TXML
{
	// 값을 저장하는 ...
	void _SetElementValue(MXmlElement *pParent, const TCHAR * const pcName, const TCHAR * const pcType, int nValue)
	{
		MXmlElement *pElement = new MXmlElement(pcName);
		pParent->LinkEndChild(pElement);
		_SetAttribute(pElement,"type",pcType);

		MXmlElement *pChild = new MXmlElement("int");
		_SetContents(pChild,(int)nValue);
		pElement->LinkEndChild(pChild);

		return;
	}

	void _SetElementValue(MXmlElement *pParent, const TCHAR * const pcName, const TCHAR * const pcType, float nValue)
	{
		MXmlElement *pElement = new MXmlElement(pcName);
		pParent->LinkEndChild(pElement);
		_SetAttribute(pElement,"type",pcType);

		MXmlElement *pChild = new MXmlElement("float");
		_SetContents(pChild,(float)nValue);
		pElement->LinkEndChild(pChild);

		return;
	}

	void _SetElementValue(MXmlElement *pParent, const TCHAR * const pcName, const TCHAR * const pcType, bool bValue)
	{
		MXmlElement *pElement = new MXmlElement(pcName);
		pParent->LinkEndChild(pElement);
		_SetAttribute(pElement,"type",pcType);

		MXmlElement *pChild = new MXmlElement("bool");
		_SetContents(pChild,bValue);
		pElement->LinkEndChild(pChild);

		return;
	}

	void _SetElementValue(MXmlElement *pParent, const TCHAR * const pcName, const TCHAR * const pcType, const TCHAR * const nValue)
	{
		MXmlElement *pElement = new MXmlElement(pcName);
		pParent->LinkEndChild(pElement);
		_SetContents(pElement, nValue);
		return;
	}

	void _SetElementValue(MXmlElement *pParent, const TCHAR * const pcName, const TCHAR * const pcType, const rs3::RVector2 &nValue)
	{
		MXmlElement *pElement = new MXmlElement(pcName);
		pParent->LinkEndChild(pElement);
		_SetAttribute(pElement,"type",pcType);

		MXmlElement *pChild = new MXmlElement("vec2");
		TCHAR tStr[128];
		memset(tStr, 0, sizeof(tStr));
		_stprintf(tStr,_T("%f %f "), nValue.x, nValue.y);
		_SetContents(pChild, tStr);

		pElement->LinkEndChild(pChild);

		return;
	}

	void _SetElementValue(MXmlElement *pParent, const TCHAR * const pcName, const TCHAR * const pcType, const rs3::RVector &nValue)
	{
		MXmlElement *pElement = new MXmlElement(pcName);
		pParent->LinkEndChild(pElement);
		_SetAttribute(pElement,"type",pcType);

		MXmlElement *pChild = new MXmlElement("vec3");
		TCHAR tStr[128];
		memset(tStr, 0, sizeof(tStr));
		_stprintf(tStr,_T("%f %f %f"), nValue.x, nValue.y, nValue.z);
		_SetContents(pChild, tStr);

		pElement->LinkEndChild(pChild);

		return;
	}

	void _SetElementValue(MXmlElement *pParent, const TCHAR * const pcName, const TCHAR * const pcType, const rs3::RVector4 &nValue)
	{
		setlocale(LC_ALL, "kor");

		MXmlElement *pElement = new MXmlElement(pcName);
		pParent->LinkEndChild(pElement);
		_SetAttribute(pElement,"type",pcType);

		MXmlElement *pChild = new MXmlElement("vec4");
		TCHAR tStr[128];
		memset(tStr, 0, sizeof(tStr));
		_stprintf(tStr,_T("%f %f %f %f"), nValue.x, nValue.y, nValue.z, nValue.w);
		_SetContents(pChild, tStr);

		pElement->LinkEndChild(pChild);

		return;
	}


	bool _GetElementValue(rs3::RVector2 *pOut, MXmlElement *pParent, const TCHAR * const pcName, const TCHAR * const pcType)
	{
		setlocale(LC_ALL, "kor");

		TCHAR pcStr[128];
		memset(pcStr, 0, sizeof(pcStr));
		MXmlElement *pElement = pParent->FirstChildElement(pcName);
		if ( pElement == NULL )
			return false;
		try
		{
			_Contents(pcStr, pElement->FirstChildElement(pcType));
		}
		catch ( ... )
		{
			return false;
		}
		_stscanf(pcStr,"%f %f ",&pOut->x,&pOut->y);
		return true;
	}

	bool _GetElementValue(rs3::RVector3 *pOut, MXmlElement *pParent, const TCHAR * const pcName, const TCHAR * const pcType)
	{
		setlocale(LC_ALL, "kor");

		TCHAR pcStr[128];
		memset(pcStr, 0, sizeof(pcStr));
		MXmlElement *pElement = pParent->FirstChildElement(pcName);
		if ( pElement == NULL )
			return false;
		try
		{
			_Contents(pcStr, pElement->FirstChildElement(pcType));
		}
		catch ( ... )
		{
			return false;
		}
		_stscanf(pcStr,"%f %f %f",&pOut->x,&pOut->y,&pOut->z);
		return true;
	}

	bool _GetElementValue(rs3::RVector4 *pOut, MXmlElement *pParent, const TCHAR * const pcName, const TCHAR * const pcType)
	{
		setlocale(LC_ALL, "kor");

		TCHAR pcStr[128];
		memset(pcStr, 0, sizeof(pcStr));
		MXmlElement *pElement = pParent->FirstChildElement(pcName);
		if ( pElement == NULL )
			return false;

		try
		{
			_Contents(pcStr, pElement->FirstChildElement(pcType));
		}
		catch ( ... )
		{
			return false;
		}

		_stscanf(pcStr,"%f %f %f %f",&pOut->x,&pOut->y,&pOut->z,&pOut->w);
		return true;
	}

};