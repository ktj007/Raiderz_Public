//
// TypedProperty.h
//

#ifndef _TYPED_PROPERTY_H
#define _TYPED_PROPERTY_H

#include "AbstractProperty.h"
#include "EnumDescription.h"

class CBaseObject;

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// CTypedProperty
//
// This intermediate class defines a property that is typed, but not bound as a member of a particular
// class.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

enum PROPERTY_EDITOR
{
	PE_NONE,
	PE_FILENAME,
	PE_COLOR,
};

template <class T> class CTypedProperty: public CAbstractProperty
{

public:

	//----------------------------------------------------------------------------------------------
	// Constructor. Takes in property name.
	inline CTypedProperty( const char* szName, const char* szDescription, const char* szCategory, PROPERTY_EDITOR ePropertyEditor );

	//----------------------------------------------------------------------------------------------
	// Returns the type of this property.
	virtual ePropertyType	GetType() const;

	//----------------------------------------------------------------------------------------------
	// Returns the Description of this property.
	inline const char*              GetDescription() const;
	
	//----------------------------------------------------------------------------------------------
	// Returns the Category of this property.
	inline const char*              GetCategory() const;


	//----------------------------------------------------------------------------------------------
	// Returns the Editor of this property.
	inline const PROPERTY_EDITOR	GetPropertyEditor() const;

	/// return EnumDescription
	inline const EnumDescription*	GetEnumDescription() const;

	//----------------------------------------------------------------------------------------------
	// Determines the value of this property.
	virtual T		GetValue( CBaseObject* pObject ) = 0;
	virtual T		GetMinValue( CBaseObject* pObject ) = 0;
	virtual T		GetMaxValue( CBaseObject* pObject ) = 0;
	virtual void	SetValue( CBaseObject* pObject, T Value ) = 0;
	virtual bool	UseMinMax(void) = 0;

	void			SetUserType(ePropertyType type) { m_bUserType=true; m_UserType=type; }

protected:
	const char*				m_szDescription;	// Property Description
	const char*				m_szCategory;	// Property Category
	EnumDescription			m_enumDescription;

	PROPERTY_EDITOR			m_ePropertyEditor;

	bool					m_bUserType;
	ePropertyType			m_UserType;
};

template <class T>
inline CTypedProperty<T>::CTypedProperty( const char* szName, const char* szDescription, const char* szCategory, PROPERTY_EDITOR ePropertyEditor ) :
	CAbstractProperty( szName ),
	m_szDescription(szDescription),
	m_szCategory(szCategory),
	m_ePropertyEditor(ePropertyEditor),
	m_bUserType(false),
	m_UserType(eptPTR)
{
	EnumDescription::CreateEnumDescription(szDescription, &m_enumDescription, &m_szDescription );
}

template <class T>
ePropertyType CTypedProperty<T>::GetType() const
{
	if(m_bUserType)
		return m_UserType;
	else
		return CPropertyType<T>::GetTypeID();
}

template <class T>
inline const char* CTypedProperty<T>::GetDescription() const
{
	return m_szDescription;
}
template <class T>
inline const char* CTypedProperty<T>::GetCategory() const
{
	return m_szCategory;
}


template <class T>
inline const PROPERTY_EDITOR CTypedProperty<T>::GetPropertyEditor() const
{
	return m_ePropertyEditor;
}


template <class T>
inline const EnumDescription* CTypedProperty<T>::GetEnumDescription() const
{
	return &m_enumDescription;
}
#endif	// _TYPED_PROPERTY_H