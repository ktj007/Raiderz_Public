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

#include <crtdbg.h>

namespace mmonitor {

bool MMonitor::Init()
{
	InitMonitorObjectMemPool();

	if( !InitTaskManagerGroup() )
		return false;
	
	_ASSERT( NULL == m_pProtocolBuilder);
	if( !InitProtocolBuilder() )
		return false;

	if( !InitUDPXmlManager() )
		return false;
	
	return OnInit();
}


bool MMonitor::InitTaskManagerGroup()
{
	_ASSERT( NULL == m_pTaskManagerGroup );

	m_pTaskManagerGroup = new MTaskManagerGroup;
	if( 0 == m_pTaskManagerGroup )
		return false;

	return true;
}


void MMonitor::Release()
{
	OnPreRelease();

	ReleaseMCommunicator();
	ReleaseTaskManagerGroup();	
	ReleaseProtocolBuilder();
	ReleaseUDPXmlManager();
	ReleaseSendCommandList();
	ReleaseMonitorObjectMemPool();

	OnRelease();
}


void MMonitor::ReleaseMCommunicator()
{
	if( 0 != m_pCommunicator )
	{
		m_pCommunicator->Release();
		delete m_pCommunicator;
		m_pCommunicator = 0;
	}
}


void MMonitor::ReleaseTaskManagerGroup()
{
	if( 0 != m_pTaskManagerGroup )
	{
		m_pTaskManagerGroup->Release();
		delete m_pTaskManagerGroup;
		m_pTaskManagerGroup = 0;
	}
}


void MMonitor::ReleaseProtocolBuilder()
{
	if( NULL == m_pProtocolBuilder )
		return;

	m_pProtocolBuilder->Release();
	delete m_pProtocolBuilder;
	m_pProtocolBuilder = NULL;
}


bool MMonitor::InitUDPXmlManager()
{
	_ASSERT( NULL == m_pRecvUDPXmlManager );
	m_pRecvUDPXmlManager = new MMonitorUDPXmlManager;
	if( NULL == m_pRecvUDPXmlManager )
		return false;
	if( !m_pRecvUDPXmlManager->Init() )
		return false;

	_ASSERT( NULL == m_pOnCommandUDPXmlManager );
	m_pOnCommandUDPXmlManager = new MMonitorUDPXmlManager;
	if( NULL == m_pOnCommandUDPXmlManager )
		return false;
	if( !m_pOnCommandUDPXmlManager->Init() )
		return false;

	return true;
}


void MMonitor::ReleaseUDPXmlManager()
{
	if( NULL != m_pRecvUDPXmlManager )
	{
		m_pRecvUDPXmlManager->Release();
		delete m_pRecvUDPXmlManager;
		m_pRecvUDPXmlManager = NULL;
	}


	if( NULL != m_pOnCommandUDPXmlManager )
	{
		m_pOnCommandUDPXmlManager->Release();
		delete m_pOnCommandUDPXmlManager;
		m_pOnCommandUDPXmlManager = NULL;
	}
}


void MMonitor::ReleaseSendCommandList()
{
	const size_t nSendCmdCount = m_SendCommandList.size();

	for( size_t i = 0; i < nSendCmdCount; ++i )
		DeleteMonitorCommand( m_SendCommandList[i] );

	m_SendCommandList.clear();
}


void MMonitor::InitMonitorObjectMemPool()
{
	InitMonitorMemPool( MMonitorUDPXml );
	InitMonitorMemPool( MMonitorCommandParam );
	InitMonitorMemPool( MMonitorCommandElement );
	InitMonitorMemPool( MMonitorCommand );
}


void MMonitor::ReleaseMonitorObjectMemPool()
{
	ReleaseMonitorMemPool( MMonitorUDPXml );
	ReleaseMonitorMemPool( MMonitorCommandParam );
	ReleaseMonitorMemPool( MMonitorCommandElement );
	ReleaseMonitorMemPool( MMonitorCommand );


	UninitMonitorMemPool( MMonitorUDPXml );
	UninitMonitorMemPool( MMonitorCommandParam );
	UninitMonitorMemPool( MMonitorCommandElement );
	UninitMonitorMemPool( MMonitorCommand );
}


void MMonitor::SetCommunicator( MMonitorCommunicator* pCommunicator )		
{ 
	_ASSERT( 0 != pCommunicator );

	m_pCommunicator = pCommunicator; 
}


void MMonitor::SetMonitorProtocolBuilder( MMonitorProtocolBuilder* pProtocolBuilder )
{
	_ASSERT( 0 != pProtocolBuilder );

	m_pProtocolBuilder = pProtocolBuilder;
}

}