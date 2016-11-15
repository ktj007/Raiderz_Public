#include "stdafx.h"
#include "MMonitorProtocolManager.h"
#include "MMonitorProtocol.h"
#include "MMonitorXmlInterface.h"
#include "MMonitorCommandDefine.h"
//#include "MXml.h"
#include "MMxml.h"

#include <crtdbg.h>


#include <utility>

using std::pair;

namespace mmonitor {

MMonitorProtocolManager::MMonitorProtocolManager()
{
}


MMonitorProtocolManager::~MMonitorProtocolManager()
{
	Release();
}


bool MMonitorProtocolManager::LoadProtocolXmlTest()
{
	MMXml xml;
	if (xml.LoadFile(MONITOR_PROTOCOL_FILENAME) == false)
	{
		return false;
	}

	MXmlElement* child = xml.Doc()->FirstChildElement()->FirstChildElement();
	for( child; child != NULL; child = child->NextSiblingElement() )
	{
		char szTagName[MMONITOR_XML_BUFFER_MAX_LEN] = "";
		strcpy(szTagName, child->Value());
		if (szTagName[0] == '#')
		{
			continue;
		}

		if( (0 == _stricmp(MONITOR_PROTOCOL_REQUEST, szTagName)) ||	(0 == _stricmp(MONITOR_PROTOCOL_RESPONSE, szTagName)) )
		{
			LoadProtocolTest( child );
		}
	}
	return true;
}


bool MMonitorProtocolManager::LoadProtocolTest( MMonitorXmlElement* pProtocolElement )
{
	if( NULL == pProtocolElement  )
	{
		return false;
	}

	MMonitorProtocol* pProtocol = new MMonitorProtocol;
	if( NULL == pProtocol )
	{
		return false;
	}

	char szTagName[ MONITOR_PROTOCOLXML_MAX_BUFFER_LEN ] = "";
	strcpy(szTagName, pProtocolElement->Value());
	pProtocol->m_strType = szTagName;

	_Attribute(pProtocol->m_strID, pProtocolElement, MONITOR_PROTOCOL_ID);
	_Attribute(pProtocol->m_strDescription, pProtocolElement, MONITOR_PROTOCOL_DESCRIPTION);

	MMonitorXmlElement* pChrElement = NULL;
	for( pChrElement = pProtocolElement->FirstChildElement(); pChrElement !=NULL; pChrElement = pChrElement->NextSiblingElement() )
	{
		char szTargetTagName[ MONITOR_PROTOCOLXML_MAX_BUFFER_LEN ] = "";
		strcpy(szTargetTagName, pChrElement->Value());
		if (szTargetTagName[0] == '#')
		{
			continue;
		}

		if( 0 == _stricmp(MONITOR_PROTOCOLPARAM, szTargetTagName) )
		{
			MMonitorProtocolParam* pParam = LoadParamTest( pChrElement );
			if( NULL == pParam )
			{
				DeleteMonitorProtocol( pProtocol );
				return false;
			}

			pProtocol->AddParam( pParam );
		}
	}

	if( !AddProtocol(pProtocol) )
	{
		DeleteMonitorProtocol( pProtocol );
		return false;
	}

	return true;
}


MMonitorProtocolParam* MMonitorProtocolManager::LoadParamTest( MMonitorXmlElement* pParamElement )
{
	if( NULL == pParamElement )
	{
		return NULL;
	}

	MMonitorProtocolParam* pParam = new MMonitorProtocolParam;
	if( NULL == pParam )
	{
		return NULL;
	}

	_Attribute(pParam->m_strType, pParamElement, MONITOR_PROTOCOLPARAM_TYPE);
	_Attribute(pParam->m_strDescription, pParamElement, MONITOR_PROTOCOLPARAM_DESCRIPTION);

	_Contents(pParam->m_strData, pParamElement);

	return pParam;
}


bool MMonitorProtocolManager::Init(  )
{
	if( !AddMonitorDefaultProtocol() )
		return false;

	if( !LoadProtocolXmlTest() )
	{
		_ASSERT( 0 );
		return false;
	}
	
	return true;
}


const MMonitorProtocol* MMonitorProtocolManager::GetProtocol( const string& strProtocolType, const string& strProtocolID )
{
	if( MONITOR_REQEUST_COMMAND == strProtocolType )
		return GetRequestProtocol( strProtocolID );
	else if( MONITOR_RESPONSE_COMMAND == strProtocolType )
		return GetResponseProtocol( strProtocolID );

	return NULL;
}


const MMonitorProtocol* MMonitorProtocolManager::GetRequestProtocol( const string& strProtocolID )
{
	map< string, MMonitorProtocol* >::iterator itFind = m_RequestProtocolList.find( strProtocolID );
	if( m_RequestProtocolList.end() == itFind )
		return NULL;

	return itFind->second;
}


const MMonitorProtocol* MMonitorProtocolManager::GetResponseProtocol( const string& strProtocolID )
{
	map< string, MMonitorProtocol* >::iterator itFind = m_ResponseProtocolList.find( strProtocolID );
	if( m_ResponseProtocolList.end() == itFind )
		return NULL;

	return itFind->second;
}


void MMonitorProtocolManager::Release()
{
	ReleaseRequestProtocolList();
	ReleaseResponseProtocolList();
}


void MMonitorProtocolManager::ReleaseRequestProtocolList()
{
	m_RequestProtocolList;

	map< string, MMonitorProtocol* >::iterator it, end;

	end = m_RequestProtocolList.end();
	for( it = m_RequestProtocolList.begin(); it != end; ++it )
		DeleteMonitorProtocol( it->second );
	m_RequestProtocolList.clear();
}


void MMonitorProtocolManager::ReleaseResponseProtocolList()
{
	map< string, MMonitorProtocol* >::iterator it, end;

	end = m_ResponseProtocolList.end();
	for( it = m_ResponseProtocolList.begin(); it != end; ++it )
		DeleteMonitorProtocol( it->second );
	m_ResponseProtocolList.clear();
}


bool MMonitorProtocolManager::AddProtocol( MMonitorProtocol* pProtocol )
{
	if( NULL == pProtocol )
		return false;

	if( pProtocol->IsRequsetType() )
	{
		if( NULL == GetRequestProtocol(pProtocol->GetID()) )
			m_RequestProtocolList.insert( pair<string, MMonitorProtocol*>(pProtocol->GetID(), pProtocol) );
		else
			return false;
	}
	else if( pProtocol->IsResponseType() )
	{
		if( NULL == GetResponseProtocol(pProtocol->GetID()) )
			m_ResponseProtocolList.insert( pair<string, MMonitorProtocol*>(pProtocol->GetID(), pProtocol) );
		else
			return false;
	}
	else
	{
		_ASSERT( 0 );
		return false;
	}

	return true;
}


bool MMonitorProtocolManager::AddMonitorDefaultProtocol()
{
	if( !AddRequestPingProtocol() )
	{
		_ASSERT( 0 );
		return false;
	}

	if( !AddResponsePingProtocol() )
	{
		_ASSERT( 0 );
		return false;
	}

	return true;
}


MMonitorProtocolParam* MMonitorProtocolManager::MakeProtocolParam( const string& strType, const string& strDesc, const string& strData )
{
	MMonitorProtocolParam* pParam = new MMonitorProtocolParam;
	if( NULL == pParam )
		return NULL;

	pParam->m_strType			= strType;
	pParam->m_strDescription	= strDesc;
	pParam->m_strData			= strData;

    return pParam;
}


bool MMonitorProtocolManager::AddRequestPingProtocol()
{
	MMonitorProtocol* pRequest = new MMonitorProtocol;
	if( NULL == pRequest )
		return false;

	pRequest->m_strType			= MONITOR_REQEUST_COMMAND;
	pRequest->m_strID			= MONITOR_PING_PROTOCOLID;
	pRequest->m_strDescription	= "request monitor ping";

	if( !pRequest->AddParam(MakeProtocolParam("string", "request task name", MMCXML_TRANSMIT_TASKNAME)) )
	{
		DeleteMonitorProtocol( pRequest );
		return false;
	}

	if( !pRequest->AddParam(MakeProtocolParam("string", "target", MMCXML_TRANSMIT_TARGET)) )
	{
		DeleteMonitorProtocol( pRequest );
		return false;
	}

	if( !pRequest->AddParam(MakeProtocolParam("string", "ip", MMCXML_TRANSMIT_IP)) )
	{
		DeleteMonitorProtocol( pRequest );
		return false;
	}

	if( !pRequest->AddParam(MakeProtocolParam("string", "ip", MMCXML_TRANSMIT_PORT)) )
	{
		DeleteMonitorProtocol( pRequest );
		return false;
	}

	return AddProtocol( pRequest );
}


bool MMonitorProtocolManager::AddResponsePingProtocol()
{
	MMonitorProtocol* pResponse = new MMonitorProtocol;
	if( NULL == pResponse )
		return false;

	pResponse->m_strType		= MONITOR_RESPONSE_COMMAND;
	pResponse->m_strID			= MONITOR_PING_PROTOCOLID;
	pResponse->m_strDescription = "response monitor ping";

	if( !pResponse->AddParam(MakeProtocolParam("string", "response task name", MMCXML_TRANSMIT_TASKNAME)) )
	{
		DeleteMonitorProtocol( pResponse );
		return false;
	}

	if( !pResponse->AddParam(MakeProtocolParam("string", "target", MMCXML_TRANSMIT_TARGET)) )
	{
		DeleteMonitorProtocol( pResponse );
		return false;
	}

	if( !pResponse->AddParam(MakeProtocolParam("string", "ip", MMCXML_TRANSMIT_IP)) )
	{
		DeleteMonitorProtocol( pResponse );
		return false;
	}

	if( !pResponse->AddParam(MakeProtocolParam("string", "ip", MMCXML_TRANSMIT_PORT)) )
	{
		DeleteMonitorProtocol( pResponse );
		return false;
	}

	return AddProtocol( pResponse );
}

}