#pragma once

#include "../Reflection/TypedProperty.h"

class CReflectiveEnumProperty : public CMFCPropertyGridProperty
{
public:
	CReflectiveEnumProperty(const CString& strName, const COleVariant& varValue, LPCTSTR lpszDescr,
							DWORD_PTR dwData , int min , int max, const EnumDescription* pEnumDescription);
	virtual ~CReflectiveEnumProperty(void);

protected:
	//오버라이딩
	virtual BOOL TextToVar(const CString& strText);	

	virtual CString FormatProperty();

private:
	int		m_min;
	int		m_max;
	const EnumDescription*	m_pEnumDescription;

};
