#pragma once


class CReflectiveSlideFloatProperty : public CMFCPropertyGridProperty
{

public:
	CReflectiveSlideFloatProperty(const CString& strName, const COleVariant& varValue, LPCTSTR lpszDescr, DWORD_PTR dwData,float min,float max);
	virtual ~CReflectiveSlideFloatProperty(void);
public:
	virtual CWnd*	CreateInPlaceEdit(CRect rectEdit,BOOL& bDefaultFormat);
	virtual BOOL	OnUpdateValue();
	
public:
	void	SetValueDirectly(const COleVariant& varValue);
	float	PosToFloat(int pos);
	int		FloatToPos(float f);

private:
	float	m_min;
	float	m_max;
	static	float	FLOAT_TO_POS_MULTIPLIER;


};
