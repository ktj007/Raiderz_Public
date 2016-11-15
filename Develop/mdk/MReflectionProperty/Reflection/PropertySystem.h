//
// CPropertySystem.h
//

#ifndef _PROPERTY_SYSTEM_H
#define _PROPERTY_SYSTEM_H

#include <list>
#include "Property.h"

using namespace std;

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// CPropertySystem
//
// Global manager of all properties. Used to release properties.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

class CPropertySystem
{

public :

	//----------------------------------------------------------------------------------------------
	// Constructor
	CPropertySystem();
	//----------------------------------------------------------------------------------------------
	// Destructor
	virtual ~CPropertySystem();

	static void Release();

	//----------------------------------------------------------------------------------------------
	// Returns global list of properties.
	inline	static	list<CAbstractProperty*>*	GetProperties();
	

private :
	static	list<CAbstractProperty*>*	ms_pProperties;
	static	bool						ms_bReleased;

};



inline list<CAbstractProperty*>* CPropertySystem::GetProperties()
{
	if( ms_bReleased == true )
	{
		_ASSERT(false); // 해제된것을 다시 사용할 수 없다. 해제는 맨마지막에 해야함
	}

	if( 0 == ms_pProperties )
	{
		ms_pProperties = new list<CAbstractProperty*>;
	}
	return ms_pProperties;
}

#endif	// _PROPERTY_SYSTEM_H