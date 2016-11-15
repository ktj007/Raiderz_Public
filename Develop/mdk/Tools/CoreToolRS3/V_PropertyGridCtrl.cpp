// PropertyGridCtrl.cpp

#include "stdafx.h"
#include "V_PropertyGridCtrl.h"

CCoreToolPropertyGridCtrl::CCoreToolPropertyGridCtrl()
{
}

CCoreToolPropertyGridCtrl::~CCoreToolPropertyGridCtrl()
{
}

void CCoreToolPropertyGridCtrl::OnPropertyChanged( CMFCPropertyGridProperty* pProp ) const
{
	DWORD_PTR pData = pProp->GetData();
	if( pData )
	{
		variant_t* pVar = (variant_t*)pData;
		*pVar = pProp->GetValue();
	}
}

CMFCPropertyGridProperty* CCoreToolPropertyGridCtrl::AddPropertyGroupItem(CMFCPropertyGridProperty* parent, const char* name, bool bEditable)
{
	CMFCPropertyGridProperty* item = new CMFCPropertyGridProperty( _T(name) );
	parent->AddSubItem( item );
	item->Enable( bEditable );
	return item;
}

CMFCPropertyGridProperty* CCoreToolPropertyGridCtrl::AddPropertyItem(CMFCPropertyGridProperty* parent, const char* name, _variant_t& value, bool bEditable)
{
	CMFCPropertyGridProperty* item = new CMFCPropertyGridProperty( _T(name), value, _T(""), (DWORD_PTR)&value );
	parent->AddSubItem( item );
	item->Enable( bEditable );
	return item;
}

CMFCPropertyGridColorProperty* CCoreToolPropertyGridCtrl::AddPropertyColorItem(CMFCPropertyGridProperty* parent, const char* name, _variant_t& value, bool bEditable)
{
	CMFCPropertyGridColorProperty* item = new CMFCPropertyGridColorProperty( _T(name), value, NULL, _T(""), (DWORD_PTR)&value );
	parent->AddSubItem( item );
	item->EnableOtherButton(_T("Color.."), FALSE);
	item->Enable( bEditable );
	return item;
}

CMFCPropertyGridFileProperty* CCoreToolPropertyGridCtrl::AddPropertyFileItem(CMFCPropertyGridProperty* parent, const char* name, CString& value, bool bEditable)
{
	CMFCPropertyGridFileProperty* item = new CMFCPropertyGridFileProperty( _T(name), true, value, 0, OFN_NOCHANGEDIR);
	parent->AddSubItem( item );
	item->Enable( bEditable );
	return item;
}

void CCoreToolPropertyGridCtrl::Init()
{
	CMFCPropertyGridCtrl::Init();

	EnableHeaderCtrl(FALSE);
	MarkModifiedProperties();
}
