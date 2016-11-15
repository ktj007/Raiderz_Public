#include "stdafx.h"
#include "SCmdHandler_ProxyPlayer.h"
#include "MCommand.h"
#include "SCommandTable.h"
#include "STransData.h"
#include "SProxyPlayerManager.h"

SProxyPlayerManager* SCmdHandler_ProxyPlayer::sm_pManager = NULL;

SCmdHandler_ProxyPlayer::SCmdHandler_ProxyPlayer(MCommandCommunicator* pCC, SProxyPlayerManager* pManager)
: MCommandHandler(pCC) 
{
	sm_pManager = pManager;

	SetCmdHandler(MSC_PROXY_PLAYER_ADD,			OnProxyPlayerAdd);
	SetCmdHandler(MSC_PROXY_PLAYER_REMOVE,		OnProxyPlayerRemove);
	SetCmdHandler(MSC_PROXY_PLAYER_UPDATE,		OnProxyPlayerUpdate);
	SetCmdHandler(MSC_PROXY_ALL_PLAYER_INFO,	OnProxyPlayerRefresh);
}

SCmdHandler_ProxyPlayer::~SCmdHandler_ProxyPlayer()
{		
	sm_pManager = NULL;
}

MCommandResult SCmdHandler_ProxyPlayer::OnProxyPlayerAdd(MCommand* pCommand, MCommandHandler* pHandler)
{	
	// 커맨드 확인
	TD_PROXY_PLAYER_INFO info;
	if (!pCommand->GetParameter(&info, 0, MPT_SINGLE_BLOB))	return CR_ERROR;

	// 플레이어 정보 추가
	sm_pManager->AddPlayer(info);

	return CR_TRUE;
}

MCommandResult SCmdHandler_ProxyPlayer::OnProxyPlayerRemove(MCommand* pCommand, MCommandHandler* pHandler)
{
	// 커맨드 확인
	CID cid;
	if (!pCommand->GetParameter(&cid, 0, MPT_INT)) return CR_ERROR;

	// 플레이어 정보 제거
	sm_pManager->RemovePlayer(cid);

	return CR_TRUE;
}

MCommandResult SCmdHandler_ProxyPlayer::OnProxyPlayerUpdate(MCommand* pCommand, MCommandHandler* pHandler)
{
	// 커맨드 확인
	TD_PROXY_PLAYER_INFO info;
	if (!pCommand->GetParameter(&info, 0, MPT_SINGLE_BLOB)) return CR_ERROR;

	// 플레이어 정보 갱신
	sm_pManager->UpdatePlayer(info);

	return CR_TRUE;
}

MCommandResult SCmdHandler_ProxyPlayer::OnProxyPlayerRefresh(MCommand* pCommand, MCommandHandler* pHandler)
{
	// 커맨드 확인
	TD_PROXY_PLAYER_INFO* pInfo;
	int nCount;	
	
	if (!pCommand->GetBlob(pInfo, nCount, 0))	return CR_ERROR;


	// 플레이어 정보 새로고침
	for(int i = 0; i < nCount; i++)
	{
		sm_pManager->AddPlayer(pInfo[i]);
	}

	return CR_TRUE;
}
