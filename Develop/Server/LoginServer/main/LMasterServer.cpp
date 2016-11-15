#include "StdAfx.h"
#include "LMasterServer.h"

LMasterServer::LMasterServer( const LMasterServerInfo& Info )
: m_Info(Info)
{

}

LMasterServer::~LMasterServer( void )
{

}

void LMasterServer::TryConnection( MUID uidPlayer )
{
	// TODO: 해당 마스터서버에게 연결을 요청한다 (MC_MASTERSERVER_PLAYER_CONNECTION_REQ)
}