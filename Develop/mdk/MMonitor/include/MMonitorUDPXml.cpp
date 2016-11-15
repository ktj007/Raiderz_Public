#include "stdafx.h"
#include "MMonitorUDPXml.h"

#include <winsock2.h>


namespace mmonitor {

void MMonitorUDPXml::SetXml( const char* pXml, const DWORD dwSize )
{
	m_strXml.reserve( dwSize );
	m_strXml.insert( m_strXml.begin(), pXml, pXml + dwSize );
}


/////////////////////////////////////////////////////////////////////////////////////////////


MMonitorUDPXmlManager::MMonitorUDPXmlManager()
{
}


MMonitorUDPXmlManager::~MMonitorUDPXmlManager()
{
}


bool MMonitorUDPXmlManager::Init()
{
	InitializeCriticalSection( &m_CriSection );

	return true;
}



void MMonitorUDPXmlManager::Push( MMonitorUDPXml* pUDPXml  )
{
	if( 0 == pUDPXml  )
		return;

	m_UDPXmlList.push_back( pUDPXml  );
}


MMonitorUDPXml* MMonitorUDPXmlManager::Pop()
{
	if( m_UDPXmlList.empty() )
		return NULL;

	MMonitorUDPXml* pUDPXml  = m_UDPXmlList.front();
	m_UDPXmlList.pop_front();	

	return pUDPXml ;
}


void MMonitorUDPXmlManager::SafePush( MMonitorUDPXml* pUDPXml  )
{
	if( 0 == pUDPXml  )
		return;

	EnterCriticalSection( &m_CriSection );
	m_UDPXmlList.push_back( pUDPXml  );
	LeaveCriticalSection( &m_CriSection );
}


MMonitorUDPXml* MMonitorUDPXmlManager::SafePop()
{
	EnterCriticalSection( &m_CriSection );
	if( m_UDPXmlList.empty() )
	{
		LeaveCriticalSection( &m_CriSection );
		return NULL;
	}

	MMonitorUDPXml* pUDPXml  = m_UDPXmlList.front();
	m_UDPXmlList.pop_front();
	LeaveCriticalSection( &m_CriSection );

	return pUDPXml ;
}


void MMonitorUDPXmlManager::Release()
{
	Lock();
	const size_t nCount = m_UDPXmlList.size();
	for( size_t i = 0; i < nCount; ++i )
		delete m_UDPXmlList[ i ];
	m_UDPXmlList.clear();
	Unlock();

	DeleteCriticalSection( &m_CriSection );
}




}