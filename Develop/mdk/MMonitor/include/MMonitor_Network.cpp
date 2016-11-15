#include "stdafx.h"
#include "MMonitor.h"
#include "MMonitorCommunicator.h"
#include "MTask.h"
#include "MTaskManager.h"
#include "MTaskManagerGroup.h"
#include "MMonitorProtocolBuilder.h"
#include "MMonitorUDPXml.h"
#include "MMonitorCommand.h"
#include "MMonitorMemPool.h"
#include "MMonitorCommandDefine.h"

#include <crtdbg.h>

namespace mmonitor {

void MMonitor::SafePushRecvXml( MMonitorUDPXml* pRecvXml )
{
	if( NULL != m_pRecvUDPXmlManager )
	{
		m_pRecvUDPXmlManager->SafePush( pRecvXml );

#ifdef _DEBUG
		IncreaseRecvCount();
#endif
	}
}


void MMonitor::MoveRecvUDPXmlListToOnCommandUDPXmlList()
{
	_ASSERT( 0 != m_pRecvUDPXmlManager );
	_ASSERT( 0 != m_pOnCommandUDPXmlManager );

	m_pRecvUDPXmlManager->Lock();

	const DWORD dwRecvUDPXmlCount = m_pRecvUDPXmlManager->GetSize();
	for( DWORD i = 0; i < dwRecvUDPXmlCount; ++i )
		m_pOnCommandUDPXmlManager->Push( m_pRecvUDPXmlManager->Pop() );

	_ASSERT( 0 == m_pRecvUDPXmlManager->Pop() );

	m_pRecvUDPXmlManager->Unlock();
}


bool MMonitor::Run_MonitorCommand()
{
	MoveRecvUDPXmlListToOnCommandUDPXmlList();

	_ASSERT( NULL != m_pOnCommandUDPXmlManager );

	const DWORD dwRecvUDPXmlCount = m_pOnCommandUDPXmlManager->GetSize();
	for( DWORD i = 0; i < dwRecvUDPXmlCount; ++i )
	{
		MMonitorUDPXml* pRecvUDPXml = m_pOnCommandUDPXmlManager->Pop();
		_ASSERT( 0 != pRecvUDPXml );

		MMonitorCommand* pMonitorCmd = m_pProtocolBuilder->BuildMonitorCommand( pRecvUDPXml );
		if( NULL == pMonitorCmd )
		{
			delete pRecvUDPXml;
			continue;
		}

		RequestMonitorCommand( pMonitorCmd );
		ResponseMonitorCommand( pMonitorCmd );

		DeleteMonitorCommand( pMonitorCmd );

#ifdef _DEBUG
		IncreaseOnCmdCount();
#endif

		delete pRecvUDPXml;
	}

	_ASSERT( 0 == m_pOnCommandUDPXmlManager->GetSize() );

	return true;
}


bool MMonitor::RequestMonitorCommand( const MMonitorCommand* pRequestCmd )
{
	if( NULL == pRequestCmd )
		return false;

	if( 0 == pRequestCmd->GetRequestElementCount() )
		return false;

	MMonitorCommand* pResponseCmd = NULL; 
	
	string strTarget;
	const DWORD dwElementCount = pRequestCmd->GetRequestElementCount();

	for( DWORD i = 0; i < dwElementCount; ++i )
	{
		const MMonitorCommandElement* pCmdElement = pRequestCmd->GetRequestElement( i );
		if( NULL == pCmdElement )
			continue;

		MMonitorCommandElement* pResponseCmdElement = RequestMonitorCommandElement( pCmdElement );
		if( NULL == pResponseCmdElement )
			continue;

		if( NULL == pResponseCmd )
		{
			pResponseCmd = new MMonitorCommand;
			if( NULL == pResponseCmd )
				return false;

			pResponseCmd->SetIP( pRequestCmd->GetIP() );
			pResponseCmd->SetPort( pRequestCmd->GetPort() );
		}

		pResponseCmd->AddCommandElement( pResponseCmdElement );
	}

	if( NULL != pResponseCmd )
		PostMonitorCommand( pResponseCmd );

	return true;
}


bool MMonitor::ResponseMonitorCommand( const MMonitorCommand* pResponseCmd )
{
	if( NULL == pResponseCmd )
		return false;

	if( 0 == pResponseCmd->GetResponseElementCount() )
		return false;

	MMonitorCommand* pRequestCmd = NULL; 
	
	string strTarget;
	const DWORD dwElementCount = pResponseCmd->GetResponseElementCount();

	for( DWORD i = 0; i < dwElementCount; ++i )
	{
		const MMonitorCommandElement* pCmdElement = pResponseCmd->GetResponseElement( i );
		if( NULL == pCmdElement )
			continue;

		MMonitorCommandElement* pRequestCmdElement = ResponseMonitorCommandElement( pCmdElement );
		if( NULL == pRequestCmdElement )
			continue;

		if( NULL == pRequestCmd )
		{
			pRequestCmd = new MMonitorCommand;
			if( NULL == pRequestCmd )
				return false;

			pRequestCmd->SetIP( pResponseCmd->GetIP() );
			pRequestCmd->SetPort( pResponseCmd->GetPort() );
		}

		pRequestCmd->AddCommandElement( pRequestCmdElement );
	}

	if( NULL != pRequestCmd )
		PostMonitorCommand( pRequestCmd );

	return true;
}


void MMonitor::PostMonitorCommand( MMonitorCommand* pMonitorCommand )
{
	if( NULL == pMonitorCommand )
		return;

	if( 0 == pMonitorCommand->GetElementCount() )
	{
		DeleteMonitorCommand( pMonitorCommand );
		return;
	}

	m_SendCommandList.push_back( pMonitorCommand );
}


void MMonitor::Run_FlushSendUDPCommand()
{
	string strSendXml;
	size_t nSendCmdCount = m_SendCommandList.size();
	for( size_t i = 0; i < nSendCmdCount; ++i )
	{
		const MMonitorCommand* pSendMonitorCmd = m_SendCommandList[ i ];

		OnPreFlushSendUDPCommand( pSendMonitorCmd );

		if( !m_pProtocolBuilder->BuildXml(pSendMonitorCmd, strSendXml) )
		{
			DeleteMonitorCommand( m_SendCommandList[i] );
			continue;
		}

		_ASSERT( 0 != m_pCommunicator );

		m_pCommunicator->Send( pSendMonitorCmd->GetIP()
			, pSendMonitorCmd->GetPort()
			, strSendXml );

#ifdef _DEBUG
		IncreaseSendCount();
#endif

		OnFlushSendUDPCommand( pSendMonitorCmd );

		DeleteMonitorCommand( m_SendCommandList[i] );
	}

	m_SendCommandList.clear();
}


MMonitorCommunicator* MMonitor::GetCommunicator()
{ 
	_ASSERT( NULL != m_pCommunicator ); 
	return m_pCommunicator; 
}


const DWORD MMonitor::GetRecvXmlListSize() 
{
	return m_pRecvUDPXmlManager->GetSize(); 
}


const DWORD	MMonitor::GetOnCommandXmlListSize()
{ 
	return m_pOnCommandUDPXmlManager->GetSize(); 
}


MMonitorCommandElement* MMonitor::RequestMonitorCommandElement( const MMonitorCommandElement* pCmdElement )
{
	if( string(MONITOR_PING_TASK_NAME) == pCmdElement->GetTarget() )
	{
		return RequestMonitorPingCommand( pCmdElement );
	}

	return OnRequestMonitorCommandElement( pCmdElement );
}


MMonitorCommandElement* MMonitor::ResponseMonitorCommandElement( const MMonitorCommandElement* pCmdElement )
{
	if( string(MONITOR_PING_TASK_NAME) == pCmdElement->GetTarget() )
	{
		return ResponseMonitorPingCommand( pCmdElement );
	}

	return OnResponseMonitorCommandElement( pCmdElement );
}


MMonitorCommandElement* MMonitor::RequestMonitorPingCommand( const MMonitorCommandElement* pCmdElement )
{
	if( NULL == pCmdElement )
		return NULL;

	if( string(MONITOR_PING_TASK_NAME ) != pCmdElement->GetTaskName() )
		return NULL;

	const MMonitorCommandParam* pParamIP = pCmdElement->GetParam( TASK_IP );
	if( NULL == pParamIP )
		return NULL;

	const MMonitorCommandParam* pParamPort = pCmdElement->GetParam( TASK_PORT );
	if( NULL == pParamPort )
		return NULL;

	MMonitorCommandElement* pCmdResElement = new MMonitorCommandElement;
	if( NULL == pCmdResElement )
		return NULL;

	pCmdResElement->SetProtocolID( pCmdElement->GetProtocolID() );
	pCmdResElement->SetTaskName( MONITOR_PING_TASK_NAME );
	pCmdResElement->SetTarget( MONITOR_PING_TASK_NAME );
	pCmdResElement->AddParam( MMCXML_TRANSMIT_IP, pParamIP->GetData() );
	pCmdResElement->AddParam( MMCXML_TRANSMIT_PORT, pParamPort->GetData() );

	pCmdResElement->ThisisResponse();

	return pCmdResElement;
}


MMonitorCommandElement* MMonitor::ResponseMonitorPingCommand( const MMonitorCommandElement* pCmdElement )
{
	if( NULL == pCmdElement )
		return NULL;

	if( string(MONITOR_PING_TASK_NAME ) != pCmdElement->GetTarget() )
		return NULL;

	const MMonitorCommandParam* pParamIP = pCmdElement->GetParam( MMCXML_TRANSMIT_IP );
	if( NULL == pParamIP )
		return NULL;

	const MMonitorCommandParam* pParamPort = pCmdElement->GetParam( MMCXML_TRANSMIT_PORT );
	if( NULL == pParamPort )
		return NULL;

	const unsigned short nPort = static_cast<unsigned short>(atol( pParamPort->GetData().c_str() ));

	MTaskManager* pMgr = m_pTaskManagerGroup->GetTaskManager( TASK_TYPE_NETWORK, pParamIP->GetData(), nPort );
	if( NULL == pMgr ) 
		return NULL;

	pMgr->SetLastPingUpdateTime( timeGetTime() );

	return OnResponseMonitorPingCommand( pCmdElement, pMgr );
}

}