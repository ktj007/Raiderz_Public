#include "StdAfx.h"
#include "V_CCoreToolPropertyGridCtrl.h"

V_CCoreToolPropertyGridCtrl::V_CCoreToolPropertyGridCtrl(void)
{
}

V_CCoreToolPropertyGridCtrl::~V_CCoreToolPropertyGridCtrl(void)
{
}

CMFCPropertyGridProperty* V_CCoreToolPropertyGridCtrl::AddPropertyBGMItem(const SOUNDFILELIST &mapBgmList
	, CMFCPropertyGridProperty* parent, const char* name, _variant_t& value, bool bEditable)
{
	CMFCPropertyGridProperty* item = new CMFCPropertyGridProperty( _T(name),  value,  "", (DWORD_PTR)&value);
	for ( SOUNDFILELIST::const_iterator it = mapBgmList.begin(); it != mapBgmList.end(); ++it )
	{
		item->AddOption(it->first.c_str());
	}

	parent->AddSubItem( item );
	item->Enable( bEditable );
	return item;
}
