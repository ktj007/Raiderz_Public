#pragma once

#include "afxpropertygridctrl.h"
#include "FastDelegate.h"

class CReflectivePropertyGroup;
class CObjectWrapper;

class CReflectivePropertyGridCtrl : public CMFCPropertyGridCtrl
{
public:
	CReflectivePropertyGridCtrl(void);
	virtual ~CReflectivePropertyGridCtrl(void);

	virtual void OnPropertyChanged( CMFCPropertyGridProperty* pProp ) const;
	LRESULT OnColorPickerSelChange(WPARAM, LPARAM);

	typedef fastdelegate::FastDelegate0< > PROPERTY_CHANGE_HANDLER;

	void		Clear();
	void		SetCallback( PROPERTY_CHANGE_HANDLER );
	void		SetTargetObject( CObjectWrapper* );

	DECLARE_MESSAGE_MAP()

protected:
	PROPERTY_CHANGE_HANDLER		m_fnCallback;
	CObjectWrapper*				m_pTargetWrapper;
};

// inline
inline void CReflectivePropertyGridCtrl::SetCallback( PROPERTY_CHANGE_HANDLER fnCallback) { m_fnCallback = fnCallback; }
