#include "stdafx.h"
#include "ReflectiveUI.h"
#include "ReflectivePropertyGridCtrl.h"
#include "ReflectiveIntProperty.h"
#include "ReflectiveSlideFloatProperty.h"
#include "ReflectiveEnumProperty.h"
#include "ReflectivePropertyGroup.h"
#include "ReflectiveColorPropertyCtrl.h"
#include "ReflectiveFloatProperty.h"
#include "ObjectWrapper.h"

CReflectiveUI::CReflectiveUI()
{
	
}

CReflectivePropertyGridCtrl* CReflectiveUI::createPropGridCtrl(CWnd *pCWnd)
{
	CRect rect;
	pCWnd->GetClientRect( rect );
	pCWnd->MapWindowPoints( pCWnd, &rect );

	CReflectivePropertyGridCtrl *pPropCtrl = new CReflectivePropertyGridCtrl;

	pPropCtrl->Create( WS_CHILD | WS_BORDER | WS_VISIBLE | WS_TABSTOP, rect, pCWnd, 1231 );
	pPropCtrl->EnableHeaderCtrl( TRUE, _T( "Property" ), _T( "Value" ));
	pPropCtrl->SetVSDotNetLook( TRUE );
	pPropCtrl->EnableDescriptionArea( TRUE );	

	return pPropCtrl;
}

void CReflectiveUI::BuildUIForObject(CReflectivePropertyGridCtrl* pPropCtrl,CBaseObject* pObject)
{
	CObjectWrapper* pWrapper = new CObjectWrapper;
	pWrapper->Create( pObject );
	BuildUIForObject( pPropCtrl, pWrapper );
}

void CReflectiveUI::BuildUIForObjects(CReflectivePropertyGridCtrl* pPropCtrl, std::list<CBaseObject*>& objects)
{
	CObjectWrapper* pWrapper = new CObjectWrapper;
	pWrapper->Create( objects );
	BuildUIForObject( pPropCtrl, pWrapper );
}

void CReflectiveUI::BuildUIForObject(CReflectivePropertyGridCtrl* pPropCtrl, CObjectWrapper* pWrapper)
{
	pPropCtrl->Clear();
	pPropCtrl->SetTargetObject( pWrapper );

	vector<CAbstractProperty*> Properties;
	pWrapper->EnumProperties( Properties );
	
	for ( DWORD i = 0; i < Properties.size(); i++ )
	{
		switch ( Properties[i]->GetType() )
		{
			case eptFLOAT :
			{
				CTypedProperty<float>* pTypedProperty = (CTypedProperty<float>*)Properties[i];
				CReflectivePropertyGroup* group = createGridGroup(pPropCtrl, pTypedProperty->GetCategory());

				variant_t variant;
				if( pWrapper->IsSameValue<float>(pTypedProperty) )
					variant = pWrapper->GetValue<float>( pTypedProperty );
				else
					variant = "";

				if( pTypedProperty->UseMinMax() )
				{
					CMFCPropertyGridProperty* item = new CReflectiveSlideFloatProperty(pTypedProperty->GetName(),variant,pTypedProperty->GetDescription(),(DWORD_PTR)pWrapper,
														pWrapper->GetMinValueFloat( pTypedProperty ), pWrapper->GetMaxValueFloat( pTypedProperty ) );
					group->AddSubItem(item);
				}
				else
				{
					CMFCPropertyGridProperty* item = new CReflectiveFloatProperty(pTypedProperty->GetName(),variant,pTypedProperty->GetDescription(),(DWORD_PTR)pWrapper);
					group->AddSubItem(item);				
				}

			}break;

			case eptINT:
			{
				CTypedProperty<int>* pTypedProperty = (CTypedProperty<int>*)Properties[i];
				CReflectivePropertyGroup* group = createGridGroup(pPropCtrl, pTypedProperty->GetCategory());

				variant_t variant;
				if( pWrapper->IsSameValue<int>(pTypedProperty) )
					variant = pWrapper->GetValue<int>( pTypedProperty );
				else
					variant = "";

				const EnumDescription* pEnumDescription = pTypedProperty->GetEnumDescription();
				if ( pEnumDescription->GetCount() )
				{
					CMFCPropertyGridProperty* item = new CReflectiveEnumProperty(pTypedProperty->GetName(),variant,pTypedProperty->GetDescription(),(DWORD_PTR)pWrapper,
						pWrapper->GetMinValueInt( pTypedProperty ), pWrapper->GetMaxValueInt( pTypedProperty ), pEnumDescription );
					group->AddSubItem(item);

				}else
				if( pTypedProperty->UseMinMax() )
				{
					CMFCPropertyGridProperty* item = new CReflectiveIntProperty(pTypedProperty->GetName(),variant,pTypedProperty->GetDescription(),(DWORD_PTR)pWrapper,
														pWrapper->GetMinValueInt( pTypedProperty ), pWrapper->GetMaxValueInt( pTypedProperty ) );
					group->AddSubItem(item);
				}
				else
				{
					CMFCPropertyGridProperty* item = new CMFCPropertyGridProperty(pTypedProperty->GetName(),variant,pTypedProperty->GetDescription(),(DWORD_PTR)pWrapper);
					group->AddSubItem(item);
				}
				
			}break;

			case eptBOOL:
			{
				CTypedProperty<bool>* pTypedProperty = (CTypedProperty<bool>*)Properties[i];
				CReflectivePropertyGroup* group = createGridGroup(pPropCtrl, pTypedProperty->GetCategory());

				variant_t variant;
				if( pWrapper->IsSameValue<bool>(pTypedProperty) )
					variant = pWrapper->GetValue<bool>(pTypedProperty);
				else
					variant = "";

				CMFCPropertyGridProperty* item = new CMFCPropertyGridProperty(pTypedProperty->GetName(),variant,pTypedProperty->GetDescription(),(DWORD_PTR)pWrapper );
				group->AddSubItem(item);
			}break;

			case eptSTRING:
			{
				CTypedProperty<char*>* pTypedProperty = (CTypedProperty<char*>*)Properties[i];
				CReflectivePropertyGroup* group = createGridGroup(pPropCtrl, pTypedProperty->GetCategory());

				variant_t variant = pWrapper->GetValue<char*>(pTypedProperty);

				CMFCPropertyGridProperty* item =NULL;
				if(pTypedProperty->GetPropertyEditor()==PE_FILENAME)
					item = new CMFCPropertyGridFileProperty(pTypedProperty->GetName(),true,variant,NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,NULL,pTypedProperty->GetDescription(),(DWORD_PTR)pWrapper );
				else
					item = new CMFCPropertyGridProperty(pTypedProperty->GetName(),variant,pTypedProperty->GetDescription(),(DWORD_PTR)pWrapper );

				group->AddSubItem(item);
			}break;

			case eptWSTRING:
			{
				CTypedProperty<wchar_t*>* pTypedProperty = (CTypedProperty<wchar_t*>*)Properties[i];
				CReflectivePropertyGroup* group = createGridGroup(pPropCtrl, pTypedProperty->GetCategory());

				variant_t variant = pWrapper->GetValue<wchar_t*>(pTypedProperty);
				CMFCPropertyGridProperty* item = new CMFCPropertyGridProperty(pTypedProperty->GetName(),variant,pTypedProperty->GetDescription(),(DWORD_PTR)pWrapper );
				group->AddSubItem(item);
			}break;
			case eptINTVECTOR:
			{
				CTypedProperty<std::vector<int>>* pTypedProperty = (CTypedProperty<std::vector<int>>*)Properties[i];
				CReflectivePropertyGroup* group = createGridGroup(pPropCtrl, pTypedProperty->GetCategory());

				std::vector<int> vecInt = pWrapper->GetValue< std::vector<int> >(pTypedProperty);

				std::string strItem;
				for each (int element in vecInt)
				{
					char buf[10] = {0, };
					_itoa_s(element, buf, 10, 10);
					strItem += buf;
					strItem += ",";
				}
				CMFCPropertyGridProperty* item = new CMFCPropertyGridProperty(pTypedProperty->GetName(),strItem.c_str(),pTypedProperty->GetDescription(),(DWORD_PTR)pWrapper );
				group->AddSubItem(item);
			}break;
			case eptCOLOR:
			{
				CTypedProperty<DWORD>* pTypedProperty = (CTypedProperty<DWORD>*)Properties[i];
				CReflectivePropertyGroup* group = createGridGroup(pPropCtrl, pTypedProperty->GetCategory());

				variant_t variant = pWrapper->GetValue<DWORD>(pTypedProperty);
#define _USE_CUSTOM_COLOR_PICKER
#ifdef _USE_CUSTOM_COLOR_PICKER
				CReflectiveColorPropertyCtrl* color = new CReflectiveColorPropertyCtrl(pTypedProperty->GetName(), variant, 0, pTypedProperty->GetDescription(), (DWORD_PTR)pWrapper);
				group->AddSubItem(color);
#else
				CMFCPropertyGridColorProperty* color = new CMFCPropertyGridColorProperty(pTypedProperty->GetName(), variant, 0, pTypedProperty->GetDescription(), (DWORD_PTR)pWrapper);
				group->AddSubItem(color);
				color->EnableOtherButton(_T("±âÅ¸..."), FALSE);
				color->Enable( true );
#endif
			}
			break;
		};
	}

	m_mapGroup.clear();
}


CReflectivePropertyGroup* CReflectiveUI::createGridGroup( CReflectivePropertyGridCtrl* pPropCtrl, const char *szGroupName )
{
	_ASSERT( NULL != szGroupName);

	PROPERTY_GROUP_MAP::iterator itr = m_mapGroup.find( szGroupName );
	if( itr == m_mapGroup.end() )
	{
		CReflectivePropertyGroup * pGroup = new CReflectivePropertyGroup(szGroupName);
		pPropCtrl->AddProperty( pGroup );
		m_mapGroup.insert( PROPERTY_GROUP_MAP::value_type( szGroupName, pGroup ) );
		return pGroup;
	}

	return itr->second;
}

