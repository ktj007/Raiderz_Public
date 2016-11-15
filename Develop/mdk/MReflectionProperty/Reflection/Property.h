//
// Property.h
//

#ifndef _PROPERTY_H
#define _PROPERTY_H

#include "TypedProperty.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// CProperty
//
// Fully defined property class. Specifies property's name, type and the getter/setters are bound as
// members of a specific class type.
//
// Template parameter OwnerType is the class that the getter and setter are a member of and template
// parameter T is the property type.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#define DEFAULT_CATEGORY_NAME "Form"

template <class OwnerType, class T> class CProperty : public CTypedProperty<T>
{

public:

	//----------------------------------------------------------------------------------------------
	typedef T		(OwnerType::*GetterType)();				// Getter function
	typedef void	(OwnerType::*SetterType)( T Value );	// Setter function

	//----------------------------------------------------------------------------------------------
	// Constructor. Takes in property's name, getter and setter functions.
	inline CProperty( const char* szName, GetterType Getter,SetterType Setter, GetterType MinGetter = 0, GetterType MaxGetter = 0, 
		const char* szDescription = "", const char* szCategory = DEFAULT_CATEGORY_NAME, PROPERTY_EDITOR ePropertyEditor= PE_NONE);

	//----------------------------------------------------------------------------------------------
	// Determines the value of this property.
	virtual T		GetValue( CBaseObject* pObject );
	virtual T		GetMinValue( CBaseObject* pObject );
	virtual T		GetMaxValue( CBaseObject* pObject );
	virtual void	SetValue( CBaseObject* pObject, T Value );
	virtual bool	UseMinMax(void);

protected:
    GetterType		m_Getter;
    SetterType		m_Setter;
	GetterType		m_MinGetter;
	GetterType		m_MaxGetter;
};

template <class OwnerType, class T>
inline CProperty<OwnerType, T>::CProperty( const char* szName, GetterType Getter,SetterType Setter, GetterType MinGetter = 0, GetterType MaxGetter = 0, 
										  const char* szDescription ="", const char* szCategory="Form", PROPERTY_EDITOR ePropertyEditor = PE_NONE) :
	CTypedProperty<T>	( szName, szDescription, szCategory, ePropertyEditor ),
	m_Getter			( Getter		),
	m_Setter			( Setter		),
	m_MinGetter			( MinGetter		),
	m_MaxGetter			( MaxGetter		)
{
}


template <class OwnerType, class T> 
bool CProperty<OwnerType, T>::UseMinMax( void )
{
	if( m_MinGetter != 0 && m_MaxGetter != 0 )	return true;
	return false;
}


template <class OwnerType, class T> 
T CProperty<OwnerType, T>::GetValue( CBaseObject* pOwner )
{
	return (((OwnerType*)pOwner)->*m_Getter)();
}

template <class OwnerType, class T> 
T CProperty<OwnerType, T>::GetMinValue( CBaseObject* pOwner )
{
	_ASSERT( 0 != m_MinGetter);
	return (((OwnerType*)pOwner)->*m_MinGetter)();
}

template <class OwnerType, class T> 
T CProperty<OwnerType, T>::GetMaxValue( CBaseObject* pOwner )
{
	_ASSERT( 0 != m_MaxGetter);
	return (((OwnerType*)pOwner)->*m_MaxGetter)();
}

template <class OwnerType, class T> 
void CProperty<OwnerType, T>::SetValue(CBaseObject* pOwner, T Value )
{
	if ( !m_Setter)
	{
		_ASSERT( false ); // Cannot write to a read-only property
		return;
	}
	(((OwnerType*)pOwner)->*m_Setter)( Value );
}

#endif	// _PROPERTY_H