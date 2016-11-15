#pragma once

#include "MXml.h"
#include "RTypes.h"

namespace TXML
{
	// 값을 저장하는 ...
	void _SetElementValue(MXmlElement *pParent, const TCHAR * const pcName, const TCHAR * const pcType, int nValue);
	void _SetElementValue(MXmlElement *pParent, const TCHAR * const pcName, const TCHAR * const pcType, float nValue);
	void _SetElementValue(MXmlElement *pParent, const TCHAR * const pcName, const TCHAR * const pcType, bool bValue);
	void _SetElementValue(MXmlElement *pParent, const TCHAR * const pcName, const TCHAR * const pcType, const TCHAR * const nValue);
	void _SetElementValue(MXmlElement *pParent, const TCHAR * const pcName, const TCHAR * const pcType, const rs3::RVector2 &nValue);
	void _SetElementValue(MXmlElement *pParent, const TCHAR * const pcName, const TCHAR * const pcType, const rs3::RVector &nValue);
	void _SetElementValue(MXmlElement *pParent, const TCHAR * const pcName, const TCHAR * const pcType, const rs3::RVector4 &nValue);

	//
	RS_API bool _GetElementValue(rs3::RVector2 *pOut, MXmlElement *pParent, const TCHAR * const pcName, const TCHAR * const pcType);
	RS_API bool _GetElementValue(rs3::RVector3 *pOut, MXmlElement *pParent, const TCHAR * const pcName, const TCHAR * const pcType);
	RS_API bool _GetElementValue(rs3::RVector4 *pOut, MXmlElement *pParent, const TCHAR * const pcName, const TCHAR * const pcType);

	template<typename T>
	class _GETElement
	{
	public:
		static T Value(MXmlElement *pParent, const TCHAR * const pcName, const TCHAR * const pcType)
		{
			T nValue;
			MXmlElement *pElement = pParent->FirstChildElement(pcName);
			if ( pElement == NULL )
				return (T)(0);

			try 
			{
				_Contents(&nValue, pElement->FirstChildElement(pcType));
			}
			catch ( ... )
			{
				return (T)(0);
			}

			return nValue;
		}
	};
};