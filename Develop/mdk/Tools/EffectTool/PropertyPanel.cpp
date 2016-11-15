#include "wx/wxprec.h"

#include "PropertyPanel.h"

namespace EffectTool
{

wxReflectivePropertyGrid::wxReflectivePropertyGrid(wxWindow* parent, wxWindowID id) :
 wxPropertyGrid(parent, id)
,m_pBaseObject(NULL)
{

}

wxReflectivePropertyGrid::~wxReflectivePropertyGrid()
{

}

void wxReflectivePropertyGrid::InitReflectionProperties(CBaseObject* pBaseObject)
{
	Clear();

	_ASSERT(pBaseObject);
	m_pBaseObject = pBaseObject;

	vector<CAbstractProperty*> properties;
	pBaseObject->GetRTTI()->EnumProperties( properties );

	for(size_t i = 0; i < properties.size(); ++i)
	{
		CAbstractProperty* pProperty = properties[i];
		ePropertyType propertyType = pProperty->GetType();

		if(eptBOOL==propertyType)
		{
			CTypedProperty<bool>* pTypedProperty = (CTypedProperty<bool>*)pProperty;
			AppendReflectionProperty(pTypedProperty);

			wxString categoryName = pTypedProperty->GetCategory();
			Append(new wxPropertyCategory(categoryName));

		//	Append(new wxBoolProperty() )
		}
		else
		if(eptINT==propertyType)
		{
			CTypedProperty<int>* pTypedProperty = (CTypedProperty<int>*)pProperty;
			AppendReflectionProperty(pTypedProperty);
		}
		else
		if(eptFLOAT==propertyType)
		{
			CTypedProperty<float>* pTypedProperty = (CTypedProperty<float>*)pProperty;
			AppendReflectionProperty(pTypedProperty);
		}
		else
		if(eptSTRING==propertyType)
		{
			CTypedProperty<char*>* pTypedProperty = (CTypedProperty<char*>*)pProperty;
			AppendReflectionProperty(pTypedProperty);
		}
		else
		if(eptCOLOR==propertyType)
		{
			CTypedProperty<DWORD>* pTypedProperty = (CTypedProperty<DWORD>*)pProperty;
			AppendReflectionProperty(pTypedProperty);
		}
	}
}

template<typename T>
void wxReflectivePropertyGrid::AppendReflectionProperty(CTypedProperty<T>* pTypedProperty)
{
	_ASSERT(m_pBaseObject);
	_ASSERT(pTypedProperty);


}

}