#pragma once

class CReflectiveIntProperty : public CMFCPropertyGridProperty
{
public:
	CReflectiveIntProperty(const CString& strName, const COleVariant& varValue, LPCTSTR lpszDescr , DWORD_PTR dwData , int min , int max);
	virtual ~CReflectiveIntProperty(void);

public://오버라이딩
	virtual BOOL TextToVar(const CString& strText);	

private:
	int		m_min;
	int		m_max;

};
