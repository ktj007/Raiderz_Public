//
// CRTTI.cpp
//


#include "RTTI.h"

//-------------------------------------------------------------------------------------------------
CRTTI::CRTTI(	int dwStub, ClassID CLID, const char* szClassName, CRTTI* pBaseClassRTTI, 
				ClassFactoryFunc pFactory, RegisterReflectionFunc pReflectionFunc ) : 
	m_CLID			( CLID				),
	m_pBaseRTTI		( pBaseClassRTTI	),
	m_pObjectFactory( pFactory			)
{
	_ASSERT( CLID != 0 );
	_ASSERT( strlen(szClassName) <= CLASSNAME_LENGTH );
	strcpy_s( m_szClassName, szClassName );

	if ( pReflectionFunc )
		pReflectionFunc();
}

void CRTTI::EnumProperties( vector<CAbstractProperty*>& o_Result )
{
	if ( m_pBaseRTTI )
		m_pBaseRTTI->EnumProperties( o_Result );
	for ( list<CAbstractProperty*>::iterator it = m_Properties.begin(); it != m_Properties.end(); ++it )
		o_Result.push_back( *it );
}

CAbstractProperty* CRTTI::FindProperty( const char* szPropertyName )
{
	for ( list<CAbstractProperty*>::iterator it = m_Properties.begin(); it != m_Properties.end(); ++it )
	{
		if( strcmp( (*it)->GetName() , szPropertyName ) == 0 )
			return (*it);
	}

	if ( m_pBaseRTTI )
		return m_pBaseRTTI->FindProperty( szPropertyName );

	return NULL;
}

