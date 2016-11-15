#include "stdafx.h"
#include "MMonitorProtocol.h"
#include "MMonitorProtocolBuilder.h"
#include "MMonitorCommand.h"
#include "MMonitorXmlInterface.h"
#include "MMonitorUDPXml.h"
#include "MMXml.h"

#include <crtdbg.h>

namespace mmonitor {

MMonitorProtocolBuilder::MMonitorProtocolBuilder()
{
//	m_pXmlDocument = NULL;
}

	
MMonitorProtocolBuilder::~MMonitorProtocolBuilder()
{
	Release();
}


bool MMonitorProtocolBuilder::Init(/* MMonitorXmlDocument* pXmlDocument*/ )
{
	/*if( NULL == pXmlDocument )
		return false;

	m_pXmlDocument = pXmlDocument;*/

	m_strXmlPreTag = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<xml>\n";
	m_strXmlTailTag = "</xml>\n";

	return m_ProtocolManager.Init();
}


void MMonitorProtocolBuilder::Release()
{
	m_ProtocolManager.Release();

	/*if( NULL != m_pXmlDocument )
		DeleteMonitorXmlDocument( m_pXmlDocument );*/
}


bool MMonitorProtocolBuilder::BuildXml( const MMonitorCommand* pCmd, string& strOutXml )
{
	if( NULL == pCmd )
		return false;

	m_strXmlPreTag;
	m_strXmlTailTag;


	strOutXml.clear();

	strOutXml += m_strXmlPreTag;


	string strParamName;
	string strParamValue;

	const DWORD dwElementCount = pCmd->GetElementCount();
	for( DWORD i = 0; i < dwElementCount; ++i )
	{
		const MMonitorCommandElement* pCmdElement = pCmd->GetElement( i );
		if( NULL == pCmdElement)
			return false;

		const MMonitorProtocol* pProtocol = m_ProtocolManager.GetProtocol( pCmdElement->GetProtocolType(), pCmdElement->GetProtocolID() );
		if( NULL == pProtocol )
			return false;

		strOutXml += "<" + pProtocol->GetType() + " id=\"" + pProtocol->GetID() + "\" ";

		const DWORD dwParamCount = pProtocol->GetParamCount();
		for( DWORD j = 0; j < dwParamCount; ++j )
		{
			 if( !pProtocol->GetParamName(j, strParamName) )
				 return false;

			 if( !pCmdElement->GetParamValue(strParamName, strParamValue) )
			 	return false;

			strOutXml += " " + strParamName + "=\"" + strParamValue + "\" ";
		}

		strOutXml += "/>\n";
	}

	strOutXml += m_strXmlTailTag;

	return true;
}

MMonitorCommand* MMonitorProtocolBuilder::BuildMonitorCommand( const MMonitorUDPXml* pMonitorUDPvXml )
{
	if( NULL == pMonitorUDPvXml )
	{
		return NULL;
	}

	TiXmlDocument xml;
	xml.Parse(pMonitorUDPvXml->GetXml().c_str());

	MMonitorCommand* pCmd = new MMonitorCommand;
	if( NULL == pCmd )
	{
		return NULL;
	}

	MMonitorXmlElement* pChrElement = xml.FirstChildElement()->FirstChildElement();
	for( pChrElement; pChrElement !=NULL;  pChrElement = pChrElement->NextSiblingElement())
	{
		char szTagName[MMONITOR_XML_BUFFER_MAX_LEN] = "";
		strcpy(szTagName, pChrElement->Value());
		if( szTagName[0] == '#' )
		{
			continue;
		}
		
		if( (0 == _stricmp(MONITOR_REQEUST_COMMAND, szTagName)) || (0 == _stricmp(MONITOR_RESPONSE_COMMAND, szTagName)) )
		{
			MMonitorCommandElement* pCmdElement = new MMonitorCommandElement;
			if( 0 == pCmdElement )
			{
				DeleteMonitorCommand( pCmd );
				return NULL;
			}

			pCmdElement->SetProtocolType( szTagName );

			// MMonitorCommandElement생성.
			char szAttrValue[ MMONITOR_XML_BUFFER_MAX_LEN ] = "";
			if(_Attribute(szAttrValue, pChrElement, MONITOR_PROTOCOLID))
			{
				pCmdElement->SetProtocolID( szAttrValue );
			}
			
			const MMonitorProtocol* pProtocol = m_ProtocolManager.GetProtocol( pCmdElement->GetProtocolType(), pCmdElement->GetProtocolID() );
			if( 0 == pProtocol )
			{
				_ASSERT( 0 );
				DeleteMonitorCommand( pCmd );
				DeleteMonitorCommandElement( pCmdElement );
				return NULL;
			}

			if( pProtocol->GetParamCount() != pCmdElement->GetParamCount() )
			{
				DeleteMonitorCommand( pCmd );
				DeleteMonitorCommandElement( pCmdElement );
				return NULL;
			}

			// 정상적인 MMonitorCommandElement인지 검사.
			string strParamName;
			string strParamValue;
			const DWORD dwParamCount = pProtocol->GetParamCount();
			for( DWORD k = 0; k < dwParamCount; ++k )
			{
				const MMonitorProtocolParam* pProtocolParam = pProtocol->GetParam( k );
				if( NULL == pProtocolParam )
					return false;

				MMonitorCommandParam* pCmdParam = pCmdElement->GetParam( pProtocolParam->m_strData );
				if( NULL == pCmdParam )
				{
					DeleteMonitorCommand( pCmd );
					DeleteMonitorCommandElement( pCmdElement );

					return NULL;
				}

				pCmdParam->SetType( pProtocolParam->m_strType );
			}

			pCmd->AddCommandElement( pCmdElement );
		}
	}

	pCmd->SetIP( pMonitorUDPvXml->GetIP() );
	pCmd->SetPort( pMonitorUDPvXml->GetPort() );

	return pCmd;
}

}