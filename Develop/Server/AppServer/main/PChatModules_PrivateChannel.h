#pragma once

#include "STypes.h"
#include "PChatModuleBase.h"
#include "PChatModule_LeaveChannel.h"


//////////////////////////////////////////////////////////////////////////

class PChatModuleBase_PrivateChannel : public PChatModuleBase
{
public:
	PChatModuleBase_PrivateChannel(PChatChannel* pOwnerChannel) : PChatModuleBase(pOwnerChannel) {}

protected:
	bool CheckConditions_ChannelType_n_Owner_n_TargetNameLen(CID cidRequestPlayer, wstring& strTargetName);

};

//////////////////////////////////////////////////////////////////////////

class PChatModule_Kick : public PChatModuleBase_PrivateChannel
{
public:
	PChatModule_Kick(PChatChannel* pOwnerChannel) : PChatModuleBase_PrivateChannel(pOwnerChannel) {}
	virtual ~PChatModule_Kick() {}

protected:
	virtual void	OnRun(minet::MCommand* pCmd) override;

};

//////////////////////////////////////////////////////////////////////////

class PChatModule_Ban : public PChatModuleBase_PrivateChannel
{
public:
	PChatModule_Ban(PChatChannel* pOwnerChannel) : PChatModuleBase_PrivateChannel(pOwnerChannel) {}
	virtual ~PChatModule_Ban() {}

protected:
	virtual void	OnRun(minet::MCommand* pCmd) override;

};

//////////////////////////////////////////////////////////////////////////

class PChatModule_Unban : public PChatModuleBase_PrivateChannel
{
public:
	PChatModule_Unban(PChatChannel* pOwnerChannel) : PChatModuleBase_PrivateChannel(pOwnerChannel) {}
	virtual ~PChatModule_Unban() {}

protected:
	virtual void	OnRun(minet::MCommand* pCmd) override;

};

//////////////////////////////////////////////////////////////////////////

class PChatModule_ChangeHost : public PChatModuleBase_PrivateChannel
{
public:
	PChatModule_ChangeHost(PChatChannel* pOwnerChannel) : PChatModuleBase_PrivateChannel(pOwnerChannel) {}
	virtual ~PChatModule_ChangeHost() {}

	void Run(const MUID& uidGameServer, const MUID& uidChatChannelUID, CID cidPlayer, std::wstring& strTargetPlayerName);

protected:
	virtual void	OnRun(minet::MCommand* pCmd) override;

};

//////////////////////////////////////////////////////////////////////////

class PChatModule_Invite : public PChatModuleBase_PrivateChannel
{
public:
	PChatModule_Invite(PChatChannel* pOwnerChannel) : PChatModuleBase_PrivateChannel(pOwnerChannel) {}
	virtual ~PChatModule_Invite() {}

protected:
	virtual void	OnRun(minet::MCommand* pCmd) override;

};

//////////////////////////////////////////////////////////////////////////

class PChatModule_ChannelInfo : public PChatModuleBase_PrivateChannel
{
public:
	PChatModule_ChannelInfo(PChatChannel* pOwnerChannel) : PChatModuleBase_PrivateChannel(pOwnerChannel) {}
	virtual ~PChatModule_ChannelInfo() {}

protected:
	virtual void	OnRun(minet::MCommand* pCmd) override;

};

//////////////////////////////////////////////////////////////////////////