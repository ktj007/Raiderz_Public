#pragma once

class CReflectiveFloatProperty : public CMFCPropertyGridProperty
{
public:
	CReflectiveFloatProperty(const CString& strName, const COleVariant& varValue, LPCTSTR lpszDescr , DWORD_PTR dwData);
	~CReflectiveFloatProperty(void);

	void AdjustInPlaceEditRect(CRect& rectEdit, CRect& rectSpin);

	BOOL OnEdit(LPPOINT lptClick) override;

	void SetValue(const COleVariant& varValue) override;

};
