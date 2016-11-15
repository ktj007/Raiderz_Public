#include "StdAfx.h"
#include "ReflectivePropertyGridCtrl.h"
#include "..\reflection\BaseObject.h"
#include "ObjectWrapper.h"
#include "ReflectivePropertyGroup.h"

#include <vector>
#include <string>


void GetSplitString(const string& str, std::vector<int>& vecInt)
{
	int nStart = 0;
	while ( true )
	{
		int nComma = str.find(',', nStart);
		if (nComma == std::string::npos)
		{
			vecInt.push_back(atoi(str.substr(nStart).c_str()));
			break;
		}
		vecInt.push_back(atoi(str.substr(nStart, nComma-nStart).c_str()));
		nStart = nComma + 1;
		if (nStart >= (int)str.length())
			break;
	}
}

extern UINT WM_XCOLORPICKER_SELCHANGE;

BEGIN_MESSAGE_MAP(CReflectivePropertyGridCtrl, CMFCPropertyGridCtrl)
	ON_REGISTERED_MESSAGE(WM_XCOLORPICKER_SELCHANGE, OnColorPickerSelChange)
END_MESSAGE_MAP()

LRESULT CReflectivePropertyGridCtrl::OnColorPickerSelChange(WPARAM wParam, LPARAM)
{
	// 컬러피커에서 변경 통지
	CMFCPropertyGridColorProperty* pColorProp = DYNAMIC_DOWNCAST(CMFCPropertyGridColorProperty, GetCurSel() );
	if (pColorProp == NULL)
	{
		ASSERT(FALSE);
		return 0;
	}

	pColorProp->SetColor( wParam );
	OnPropertyChanged( pColorProp );
	return 0;
}


CReflectivePropertyGridCtrl::CReflectivePropertyGridCtrl(void)
: m_pTargetWrapper( NULL )
{
}

CReflectivePropertyGridCtrl::~CReflectivePropertyGridCtrl(void)
{
	Clear();
}

void CReflectivePropertyGridCtrl::Clear()
{
	if(m_pTargetWrapper)
	{
		delete m_pTargetWrapper;
		m_pTargetWrapper = NULL;
	}
}

void CReflectivePropertyGridCtrl::SetTargetObject( CObjectWrapper* pObject )
{ 
	Clear();
	m_pTargetWrapper = pObject; 
}

void CReflectivePropertyGridCtrl::OnPropertyChanged( CMFCPropertyGridProperty* pProp ) const
{
	CObjectWrapper* pWrapper = (CObjectWrapper*) pProp->GetData();
	_ASSERT( NULL != pWrapper );

	vector<CAbstractProperty*> Properties;
	pWrapper->EnumProperties( Properties );

	for ( DWORD i = 0; i < Properties.size(); i++ )
	{
		if ( strcmp( Properties[i]->GetName(), pProp->GetName() ) == 0 )
		{
			switch ( Properties[i]->GetType() )
			{
				case eptFLOAT :
				{
					CTypedProperty<float>* pTypedProperty = (CTypedProperty<float>*)Properties[i];
					variant_t variant = pProp->GetValue();
					pWrapper->SetValue<float>( pTypedProperty, variant );
				}break;

				case eptINT:
				{
					CTypedProperty<int>* pTypedProperty = (CTypedProperty<int>*)Properties[i];
					variant_t variant = pProp->GetValue();
					pWrapper->SetValue<int>( pTypedProperty, variant );
				}break;

				case eptBOOL:
				{
					CTypedProperty<bool>* pTypedProperty = (CTypedProperty<bool>*)Properties[i];
					variant_t variant = pProp->GetValue();
					pWrapper->SetValue<bool>( pTypedProperty, variant );
				}break;

				case eptSTRING:
				{
					CTypedProperty<char*>* pTypedProperty = (CTypedProperty<char*>*)Properties[i];
					variant_t variant(pProp->GetValue());
					bstr_t bstr = variant;
					pWrapper->SetValue<char*>( pTypedProperty, bstr );
				}break;

				case eptWSTRING:
				{
					CTypedProperty<wchar_t*>* pTypedProperty = (CTypedProperty<wchar_t*>*)Properties[i];
					variant_t variant(pProp->GetValue());
					bstr_t bstr = variant;
					pWrapper->SetValue<wchar_t*>( pTypedProperty, bstr );
				}break;
				case eptINTVECTOR:
				{
					CTypedProperty<std::vector<int>>* pTypedProperty = (CTypedProperty<std::vector<int>>*)Properties[i];
					variant_t variant(pProp->GetValue());
					bstr_t bstr = variant;
					std::vector<int> vecInt;
					GetSplitString((const char*)bstr, vecInt);
					pWrapper->SetValue< std::vector<int> >( pTypedProperty, vecInt );
				}break;
				case eptCOLOR:
				{
					CTypedProperty<DWORD>* pTypedProperty = (CTypedProperty<DWORD>*)Properties[i];
					variant_t variant = pProp->GetValue();
					pWrapper->SetValue<DWORD>( pTypedProperty, variant );
				}break;
			}
		}
	}

	if(m_fnCallback)
		m_fnCallback();
}