#include "StdAfx.h"
#include "ReflectiveIntProperty.h"



CReflectiveIntProperty::CReflectiveIntProperty( const CString& strName, const COleVariant& varValue, LPCTSTR lpszDescr, DWORD_PTR dwData, int min , int max )
:CMFCPropertyGridProperty(strName,varValue,lpszDescr,dwData)
{
	m_min = min;
	m_max = max;
	EnableSpinControl(TRUE,min,max);
}

CReflectiveIntProperty::~CReflectiveIntProperty( void )
{
}

BOOL CReflectiveIntProperty::TextToVar( const CString& strText )
{
	CMFCPropertyGridProperty::TextToVar(strText);

	variant_t variant = GetValue();
	int i = variant;
	if( i >m_max )
	{
		i = m_max;
	}
	if( i < m_min )
	{
		i = m_min;
	}
	m_varValue = (variant_t)i;


	return TRUE; 
}





