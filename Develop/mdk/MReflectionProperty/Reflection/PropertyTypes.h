//
// PropertyTypes.h
//

#ifndef _PROPERTY_TYPES_H
#define _PROPERTY_TYPES_H

#include <vector>

#ifndef DWORD
typedef unsigned long DWORD;
#endif

enum ePropertyType
{
	eptBOOL,
	eptINT,
	eptFLOAT,
	eptSTRING,
	eptWSTRING,
	eptPTR,
	eptCOLOR,
	eptINTVECTOR,
	eptMAX_PROPERTY_TYPES
};

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// CPropertyType
//
// This templatized class will associate compile-time types with unique enum members.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

template <class T> class CPropertyType
{

public :

	// Returns type ID associated with the templatized type.
	static	ePropertyType	GetTypeID();

private:

    static			ePropertyType    ms_TypeID;
};

template<class T>
ePropertyType CPropertyType<T>::GetTypeID()
{
	return ms_TypeID;
}

template<class T> ePropertyType CPropertyType<T>::ms_TypeID = eptPTR;
template<> ePropertyType CPropertyType<bool>::ms_TypeID		= eptBOOL;
template<> ePropertyType CPropertyType<int>::ms_TypeID		= eptINT; 
template<> ePropertyType CPropertyType<float>::ms_TypeID	= eptFLOAT; 
template<> ePropertyType CPropertyType<char*>::ms_TypeID	= eptSTRING;
template<> ePropertyType CPropertyType<wchar_t*>::ms_TypeID	= eptWSTRING;
template<> ePropertyType CPropertyType<const char*>::ms_TypeID	= eptSTRING;
template<> ePropertyType CPropertyType<std::vector<int>>::ms_TypeID	= eptINTVECTOR;


#endif	// _PROPERTY_TYPES_H