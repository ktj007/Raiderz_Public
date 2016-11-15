//
// BaseObject.h
//

#ifndef _REFLECTIVE_UI_H
#define _REFLECTIVE_UI_H

#include <list>
#include <map>

class CBaseObject;
class CObjectWrapper;
class CReflectivePropertyGridCtrl;
class CReflectivePropertyGroup;
template <class T> class CTypedProperty;

typedef std::list<CBaseObject*> OBJECT_LIST;

class AFX_EXT_CLASS CReflectiveUI
{

public :
	CReflectiveUI();
	CReflectivePropertyGridCtrl*	createPropGridCtrl(CWnd *pCWnd);

	void							BuildUIForObject( CReflectivePropertyGridCtrl* pPropCtrl, CBaseObject* pObject);
	void							BuildUIForObjects( CReflectivePropertyGridCtrl* pPropCtrl, OBJECT_LIST& objects);

protected:
	void							BuildUIForObject( CReflectivePropertyGridCtrl* pPropCtrl, CObjectWrapper* pWrapper);

	CReflectivePropertyGroup*		createGridGroup( CReflectivePropertyGridCtrl* pPropCtrl, const char *szGroupName );

	typedef std::map<std::string,CReflectivePropertyGroup*> PROPERTY_GROUP_MAP;
	PROPERTY_GROUP_MAP				m_mapGroup;
};





#endif	// _REFLECTIVE_UI_H