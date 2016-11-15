#include "stdafx.h"
#include "MMonitorNetInfo.h"

#include <Winsock2.h.>


namespace mmonitor {

void MMonitorNetInfo::SetIPStr( const string& strIP )	
{ 
	m_strIP = strIP; 
	m_dwIP	= inet_addr( strIP.c_str() ); 
}


void MMonitorNetInfo::SetPortNtoH( const WORD nNetPort )
{ 
	m_nPort		= ntohs( nNetPort );
	m_nPortHtoN = nNetPort;
}


void MMonitorNetInfo::SetPort( const WORD nPort )
{
	m_nPort		= nPort;
	m_nPortHtoN = htons( nPort );
}


const string MMonitorNetInfo::GetPortStr() const
{
	char szPort[ 10 ] = {0,};
    sprintf( szPort, "%u", m_nPort );

	return string( szPort );
}

}