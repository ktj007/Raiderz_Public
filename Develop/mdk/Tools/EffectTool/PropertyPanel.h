#pragma once
#include <wx/propgrid/propgrid.h>

#include "..\Reflection\BaseObject.h"

namespace EffectTool
{

class wxReflectivePropertyGrid : public wxPropertyGrid
{
public:
	wxReflectivePropertyGrid(wxWindow* parent, wxWindowID id);
	~wxReflectivePropertyGrid();

	//void SetBaseObject(CBaseObject* pBaseObject);
	//CBaseObject* GetBaseObject();

	void InitReflectionProperties(CBaseObject* pBaseObject);

private:
	template<typename T> void AppendReflectionProperty(CTypedProperty<T>* pTypedProperty);

	CBaseObject* m_pBaseObject;
//	DECLARE_EVENT_TABLE()
};

}