#include "stdafx.h"
#include "MMonitorCommand.h"
#include "MMonitorCommandDefine.h"
#include "MMonitorDefine.h"

#include <crtdbg.h>

#include <utility>

using std::pair;

namespace mmonitor {

MMonitorCommandElement::MMonitorCommandElement()
{
}


MMonitorCommandElement::~MMonitorCommandElement()
{
	Release();
}


void MMonitorCommandElement::SetTarget( const string& strTarget )		
{ 
	_ASSERT( 0 == GetParam(strTarget) ); 

	AddParam(MMCXML_TRANSMIT_TARGET, strTarget); 
}


const string& MMonitorCommandElement::GetTaskName() const
{
	const MMonitorCommandParam* pParam = GetParam( MMCXML_TRANSMIT_TASKNAME );
	if( NULL == pParam )
	{
		static string strEmpty;
		return strEmpty;
	}

	return pParam->GetData();
}


const string& MMonitorCommandElement::GetTarget() const
{
	const MMonitorCommandParam* pParam = GetParam( MMCXML_TRANSMIT_TARGET );
	if( NULL == pParam )
	{
		static string strEmpty;
		return strEmpty;
	}

	return pParam->GetData();
}


void MMonitorCommandElement::SetTaskName( const string& strTaskName )
{
	_ASSERT( 0 == GetParam(strTaskName) );

	AddParam(MMCXML_TRANSMIT_TASKNAME, strTaskName); 
}


const bool MMonitorCommandElement::IsRequest() const
{ 
	return  MMONITOR_REQUEST_COMMAND == m_strProtocolType; 
}


const bool MMonitorCommandElement::IsResponse() const
{ 
	return MMONITOR_RESPONSE_COMMAND == m_strProtocolType; 
}


void MMonitorCommandElement::ThisIsRequest()
{ 
	m_strProtocolType = MMONITOR_REQUEST_COMMAND; 
}


void MMonitorCommandElement::ThisisResponse()
{ 
	m_strProtocolType = MMONITOR_RESPONSE_COMMAND; 
}


bool MMonitorCommandElement::AddParam( const string& strName, const string& strValue )
{
	MMonitorCommandParam* pParam = new MMonitorCommandParam;
	if( 0 == pParam )
		return false;

	pParam->SetName( strName );
	pParam->SetData( strValue );

	m_ParamList.insert( pair<string, MMonitorCommandParam*>(strName, pParam) );
	m_ParamListVec.push_back( pParam );

	return true;
}


const bool MMonitorCommandElement::GetParamValue( const string& strName, string& strOutValue ) const
{
	map< string, MMonitorCommandParam* >::const_iterator itFind = m_ParamList.find( strName );
	if( m_ParamList.end() == itFind )
		return false;

	strOutValue = itFind->second->GetData();
	return true;
}


bool MMonitorCommandElement::SetParamDataType( const string& strName, const string& strType )
{
	map< string, MMonitorCommandParam* >::iterator itFind = m_ParamList.find( strName );
	if( m_ParamList.end() == itFind )
		return false;

	itFind->second->SetType( strType );

	return true;
}


const MMonitorCommandParam* MMonitorCommandElement::GetParam( const string& strName ) const 
{
	map< string, MMonitorCommandParam* >::const_iterator itFind = m_ParamList.find( strName );
	if( m_ParamList.end() == itFind )
		return NULL;

	return itFind->second;
}


MMonitorCommandParam* MMonitorCommandElement::GetParam( const string& strName )
{
	map< string, MMonitorCommandParam* >::iterator itFind = m_ParamList.find( strName );
	if( m_ParamList.end() == itFind )
		return NULL;

	return itFind->second;
}


void MMonitorCommandElement::Release()
{
	map< string, MMonitorCommandParam* >::iterator it, end;
	end = m_ParamList.end();
	for( it = m_ParamList.begin(); it != end; ++it )
		delete it->second;
	m_ParamList.clear();
}


const MMonitorCommandParam* MMonitorCommandElement::GetParam( const DWORD dwIndex ) const
{
	if( m_ParamListVec.size() > dwIndex )
		return m_ParamListVec[ dwIndex ];

	return NULL;
}


/////////////////////////////////////////////////////////////////////////////////////////


MMonitorCommand::MMonitorCommand()
{
}


MMonitorCommand::~MMonitorCommand()
{
	Release();
}


const bool MMonitorCommand::AddCommandElement( MMonitorCommandElement* pCommandElement )
{
	if( NULL == pCommandElement )
		return false;

	m_CommandElementList.push_back( pCommandElement );

	if( pCommandElement->IsRequest() )
	{
		m_RequestCommandElementList.push_back( pCommandElement );
	}
	else if( pCommandElement->IsResponse() )
	{
		m_ResponseCommandElementList.push_back( pCommandElement );
	}
#ifdef _DEBUG
	else
	{
		_ASSERT( 0 );
	}
#endif

	return true;
}


const MMonitorCommandElement* MMonitorCommand::GetElement( const DWORD dwIndex ) const 
{
	if( m_CommandElementList.size() > dwIndex )
		return m_CommandElementList[ dwIndex ];

	return NULL;
}


const MMonitorCommandElement* MMonitorCommand::GetRequestElement( const DWORD dwIndex ) const
{
	if( m_RequestCommandElementList.size() > dwIndex )
		return m_RequestCommandElementList[ dwIndex ];

	return NULL;
}


const MMonitorCommandElement* MMonitorCommand::GetResponseElement( const DWORD dwIndex ) const
{
	if( m_ResponseCommandElementList.size() > dwIndex )
		return m_ResponseCommandElementList[ dwIndex ];

	return NULL;
}


void MMonitorCommand::Release()
{
	if( m_CommandElementList.empty() )
		return;

	size_t Size = m_CommandElementList.size();
	for( size_t i = 0; i < Size; ++i )
		DeleteMonitorCommandElement( m_CommandElementList[i] );
		
	m_RequestCommandElementList.clear();
	m_ResponseCommandElementList.clear();
	m_CommandElementList.clear();
}


////////////////////////////////////////////////////////////////


void DeleteMonitorCommand( MMonitorCommand*& pCmd )
{
	if( NULL == pCmd )
		return;

	pCmd->Release();
	delete pCmd;
	pCmd = NULL;
}



void DeleteMonitorCommandElement( MMonitorCommandElement*& pCmdElement )
{
	if( NULL == pCmdElement )
		return;

	pCmdElement->Release();
	delete pCmdElement;
	pCmdElement = NULL;
}

}
