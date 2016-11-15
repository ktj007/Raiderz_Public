#ifndef _XCHAT_CMDS_H
#define _XCHAT_CMDS_H

#include "XStrCmd.h"

class XUISystem;

/// 채팅 명령어
class XChatCmds
{
private:
	friend class XUISystem;
	static XStrCmdManager		m_CmdManager;

	static void InitCmds();
	static bool OnInput(const wchar_t* szCommand);
	static void Output(const wchar_t* szText);
private:
	#define _DEF_CHAT_FUNC(func)		static bool func(const wchar_t* line, const int argc, wchar_t **const argv);

	_DEF_CHAT_FUNC(ChatCmd_Help);
	_DEF_CHAT_FUNC(ChatCmd_Die);
	_DEF_CHAT_FUNC(ChatCmd_AFK);
	_DEF_CHAT_FUNC(ChatCmd_Quest);

	// 채팅
	_DEF_CHAT_FUNC(ChatCmd_Say);
	_DEF_CHAT_FUNC(ChatCmd_Tell);
	_DEF_CHAT_FUNC(ChatCmd_Reply);
	_DEF_CHAT_FUNC(ChatCmd_Party);
	_DEF_CHAT_FUNC(ChatCmd_Guild);
	_DEF_CHAT_FUNC(ChatCmd_Shout);
	_DEF_CHAT_FUNC(ChatCmd_Auction);
	_DEF_CHAT_FUNC(ChatCmd_Channel);
	_DEF_CHAT_FUNC(ChatCmd_Field);
	_DEF_CHAT_FUNC(ChatCmd_Global);
	_DEF_CHAT_FUNC(ChatCmd_Narration);
	_DEF_CHAT_FUNC(ChatCmd_Announce);
	_DEF_CHAT_FUNC(ChatCmd_Notice);

	// 채팅 채널 명령
	_DEF_CHAT_FUNC(ChatCmd_ChannelJoin);
	_DEF_CHAT_FUNC(ChatCmd_ChannelLeave);
	_DEF_CHAT_FUNC(ChatCmd_ChannelKick);
	_DEF_CHAT_FUNC(ChatCmd_ChannelBan);
	_DEF_CHAT_FUNC(ChatCmd_ChannelUnban);
	_DEF_CHAT_FUNC(ChatCmd_ChannelOwner);
	_DEF_CHAT_FUNC(ChatCmd_ChannelCreate);

	// 소셜 모션
	_DEF_CHAT_FUNC(ChatCmd_SocialMotion_Beg);
	_DEF_CHAT_FUNC(ChatCmd_SocialMotion_Bow);
	_DEF_CHAT_FUNC(ChatCmd_SocialMotion_Cheer);
	_DEF_CHAT_FUNC(ChatCmd_SocialMotion_Clap);
	_DEF_CHAT_FUNC(ChatCmd_SocialMotion_Cry);
	_DEF_CHAT_FUNC(ChatCmd_SocialMotion_Dance);
	_DEF_CHAT_FUNC(ChatCmd_SocialMotion_Ignore);
	_DEF_CHAT_FUNC(ChatCmd_SocialMotion_Kiss);
	_DEF_CHAT_FUNC(ChatCmd_SocialMotion_Laugh);
	_DEF_CHAT_FUNC(ChatCmd_SocialMotion_Lookaround);
	_DEF_CHAT_FUNC(ChatCmd_SocialMotion_Negative);
	_DEF_CHAT_FUNC(ChatCmd_SocialMotion_Point);
	_DEF_CHAT_FUNC(ChatCmd_SocialMotion_Positive);
	_DEF_CHAT_FUNC(ChatCmd_SocialMotion_Salute);
	_DEF_CHAT_FUNC(ChatCmd_SocialMotion_Sigh);
	_DEF_CHAT_FUNC(ChatCmd_SocialMotion_Talk1);
	_DEF_CHAT_FUNC(ChatCmd_SocialMotion_Talk2);
	_DEF_CHAT_FUNC(ChatCmd_SocialMotion_Taunt);
	_DEF_CHAT_FUNC(ChatCmd_SocialMotion_Wave);
public:
	static void Init();
	static bool Input(const wchar_t* szCommand);
};





#endif