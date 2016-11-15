#pragma once

#include <list>
#include "../reflection/BaseObject.h"

class AFX_EXT_CLASS CObjectWrapper
{
public:
	void			Create( CBaseObject* pBaseObject );
	void			Create( list<CBaseObject*>& objects );

	int				GetPropertyCount();

	void			EnumProperties( vector<CAbstractProperty*>& o_Result );


	float			GetMinValueFloat( CTypedProperty<float>* );
	float			GetMaxValueFloat( CTypedProperty<float>* );
	int				GetMinValueInt( CTypedProperty<int>* );
	int				GetMaxValueInt( CTypedProperty<int>* );

	bool			IsEqual( int _a, int _b) { return _a == _b; }
	bool			IsEqual( float _a, float _b) { return _a == _b; }
	bool			IsEqual( bool _a, bool _b) { return _a == _b; }
	bool			IsEqual( char* _a, char* _b) { return strcmp(_a,_b)==0; }
	bool			IsEqual( wchar_t* _a, wchar_t* _b) { return wcscmp(_a,_b)==0; }
	bool			IsEqual( std::vector<int>& _a, std::vector<int>& _b)  { return _a == _b; }
	bool			IsEqual( DWORD _a, DWORD _b) { return _a == _b; }

	// 모두 같은 값을 가지고 있는지 확인
	template < typename T >
	bool			IsSameValue( CTypedProperty<T>* pProperty )
	{
		OBJECT_LIST::iterator itr = m_objects.begin();
		T firstValue = pProperty->GetValue( *itr );

		++itr;
		for( ; itr!= m_objects.end(); ++itr )
		{
			T thisValue = pProperty->GetValue( *itr );
			if( IsEqual( firstValue , thisValue ) == false )
				return false;
		}

		return true;
	}

	template < typename T >
	T				GetValue( CTypedProperty<T>* pProperty )
	{
		if( IsSameValue<T>(pProperty) )
			return pProperty->GetValue( m_objects.front() );

		return T(0);
	}

	template < typename T >
	void			SetValue( CTypedProperty<T>* pProperty, const T& _value )
	{
		for each (CBaseObject* pObject in m_objects)
		{
			pProperty->SetValue( pObject, _value);
		}
	}

protected:
	list<CBaseObject*> m_objects;
};