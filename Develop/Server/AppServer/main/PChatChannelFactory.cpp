#include "StdAfx.h"
#include "PChatChannelFactory.h"
#include "PChatChannel.h"
#include "PGlobal.h"
#include "PServer.h"
#include "PChatModules_PrivateChannel.h"
#include "PChatModule_EnterChannel.h"
#include "PChatModule_LeaveChannel.h"
#include "PChatModule_EnterPrivateChannel.h"
#include "PChatModule_LeavePrivateChannel.h"
#include "PChatModule_LeavePartyChannel.h"


PChatChannel* PChatChannelFactory::NewChatChannel_Private(int nOwnerCID, const std::wstring& strChannelName)
{
	MUID uidNewChannel = gsys.pServer->NewUID();

	PChatChannel* pNewChatChannel = new PChatChannel(uidNewChannel, CCT_PRIVATE);
	if (!pNewChatChannel->Init(nOwnerCID, strChannelName))
	{
		SAFE_DELETE(pNewChatChannel);
		return NULL;
	}

	PChatModuleMap& refChatCmdModules = pNewChatChannel->GetChatModules();
	refChatCmdModules.AddModule(CMT_ENTER_CHANNEL,	new PChatModule_EnterPrivateChannel(pNewChatChannel));
	refChatCmdModules.AddModule(CMT_LEAVE_CHANNEL,	new PChatModule_LeavePrivateChannel(pNewChatChannel));
	refChatCmdModules.AddModule(CMT_KICK,			new PChatModule_Kick(pNewChatChannel));
	refChatCmdModules.AddModule(CMT_BAN,			new PChatModule_Ban(pNewChatChannel));
	refChatCmdModules.AddModule(CMT_UNBAN,			new PChatModule_Unban(pNewChatChannel));
	refChatCmdModules.AddModule(CMT_CHANGE_HOST,	new PChatModule_ChangeHost(pNewChatChannel));
	refChatCmdModules.AddModule(CMT_INVITE,			new PChatModule_Invite(pNewChatChannel));
	refChatCmdModules.AddModule(CMT_CHANNEL_INFO,	new PChatModule_ChannelInfo(pNewChatChannel));

	return pNewChatChannel;
}
PChatChannel* PChatChannelFactory::NewChatChannel_Party(const MUID& uidParty)
{
	MUID uidNewChannel = gsys.pServer->NewUID();

	PChatChannel* pNewChatChannel = new PChatChannel(uidNewChannel, CCT_PARTY);
	if (!pNewChatChannel->Init(uidParty))
	{
		SAFE_DELETE(pNewChatChannel);
		return NULL;
	}

	PChatModuleMap& refChatCmdModules = pNewChatChannel->GetChatModules();
	refChatCmdModules.AddModule(CMT_ENTER_CHANNEL,		new PChatModule_EnterChannel(pNewChatChannel));
	refChatCmdModules.AddModule(CMT_LEAVE_CHANNEL,		new PChatModule_LeavePartyChannel(pNewChatChannel));

	return pNewChatChannel;
}

PChatChannel* PChatChannelFactory::NewChatChannel_Auction(void)
{
	// 채널 생성
	MUID uidChannel = gsys.pServer->NewUID();
	PChatChannel* pNewChatChannel = new PChatChannel(uidChannel, CCT_AUCTION);

	// 모듈 추가
	PChatModuleMap& refChatCmdModules = pNewChatChannel->GetChatModules();
	refChatCmdModules.AddModule(CMT_ENTER_CHANNEL,		new PChatModule_EnterChannel(pNewChatChannel));
	refChatCmdModules.AddModule(CMT_LEAVE_CHANNEL,		new PChatModule_LeaveChannel(pNewChatChannel));

	return pNewChatChannel;
}

PChatChannel* PChatChannelFactory::NewChatChannel_Field(int nFieldID)
{
	// 채널 생성
	MUID uidChannel = gsys.pServer->NewUID();
	PChatChannel* pNewChatChannel = new PChatChannel(uidChannel, CCT_FIELD);

	if (!pNewChatChannel->Init_Field(nFieldID))
	{
		delete pNewChatChannel;
		return NULL;
	}

	// 모듈 추가
	PChatModuleMap& refChatCmdModules = pNewChatChannel->GetChatModules();
	refChatCmdModules.AddModule(CMT_ENTER_CHANNEL,		new PChatModule_EnterChannel(pNewChatChannel));
	refChatCmdModules.AddModule(CMT_LEAVE_CHANNEL,		new PChatModule_LeaveChannel(pNewChatChannel));

	return pNewChatChannel;
}
