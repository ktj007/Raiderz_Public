#include "stdafx.h"
#include "MTask.h"
#include "MMonitorProtocol.h"
#include "MMonitorCommandDefine.h"

#include <utility>

using std::pair;





////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace mmonitor {


MMonitorProtocol::MMonitorProtocol()
{
}


MMonitorProtocol::~MMonitorProtocol()
{	
	Release();
}

const bool MMonitorProtocol::IsRequsetType() const	
{ 
	return MONITOR_PROTOCOL_REQUEST == m_strType; 
}


const bool MMonitorProtocol::IsResponseType() const	
{ 
	return MONITOR_PROTOCOL_RESPONSE == m_strType; 
}


bool MMonitorProtocol::GetParamName( const DWORD dwIndex, string& strOutParamName ) const
{
	if( m_ParamList.size() > dwIndex )
	{
		strOutParamName = m_ParamList[ dwIndex ]->m_strData;
		return true;
	}

	return false;
}


const MMonitorProtocolParam* MMonitorProtocol::GetParam( const DWORD dwIndex ) const
{
	if( m_ParamList.size() > dwIndex )
		return m_ParamList[ dwIndex ];
	return NULL;
}


void MMonitorProtocol::Release()
{
	const DWORD dwParamCount = GetParamCount();
	for( DWORD i = 0; i < dwParamCount; ++i )
		delete m_ParamList[ i ];
	m_ParamList.clear();
}


bool MMonitorProtocol::AddParam( MMonitorProtocolParam* pParam )
{
	const size_t nParamCount = m_ParamList.size();
	for( size_t i = 0; i < nParamCount; ++i )
	{
		if( pParam->m_strData == m_ParamList[i]->m_strData )
			return false;
	}

	m_ParamList.push_back( pParam );

	return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void DeleteMonitorProtocol( MMonitorProtocol*& pProtocol )
{
	if( NULL == pProtocol )
		return;

	pProtocol->Release();
	delete pProtocol;
	pProtocol = NULL;
}


}