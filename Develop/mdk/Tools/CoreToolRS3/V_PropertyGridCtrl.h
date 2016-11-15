// V_PropertyGridCtrl.h

#pragma once

#include "afxpropertygridctrl.h"

class CORETOOL_API CCoreToolPropertyGridCtrl : public CMFCPropertyGridCtrl
{
public:
	CCoreToolPropertyGridCtrl();
	virtual ~CCoreToolPropertyGridCtrl();

	virtual void OnPropertyChanged( CMFCPropertyGridProperty* pProp ) const;

	CMFCPropertyGridProperty* AddPropertyGroupItem(CMFCPropertyGridProperty* parent, const char* name, bool bEditable);
	CMFCPropertyGridProperty* AddPropertyItem(CMFCPropertyGridProperty* parent, const char* name, _variant_t& value, bool bEditable);
	CMFCPropertyGridColorProperty* AddPropertyColorItem(CMFCPropertyGridProperty* parent, const char* name, _variant_t& value, bool bEditable);
	CMFCPropertyGridFileProperty* AddPropertyFileItem(CMFCPropertyGridProperty* parent, const char* name, CString& value, bool bEditable);
protected:
	virtual void Init();
};
