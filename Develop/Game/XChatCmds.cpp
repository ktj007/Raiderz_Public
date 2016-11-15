#include "stdafx.h"
#include "XChatCmds.h"
#include "XGlueGameEvent.h"
#include "XChatCmdLoader.h"
#include "XChatCmdID.h"
#include "XMyPlayer.h"
#include "XChatChannelManager.h"
#include "XChatChannel.h"
#include "XGlobalVar.h"
#include "XPost_Char.h"
#include "XPost_Msg.h"
#include "XPost_ChatChannel.h"

XStrCmdManager XChatCmds::m_CmdManager;

void XChatCmds::Init()
{
	m_CmdManager.Clear();

	InitCmds();
}

bool XChatCmds::Input(const wchar_t* szCommand)
{
	return OnInput(szCommand);
}

bool XChatCmds::OnInput(const wchar_t* szCommand)
{
	if (szCommand[0] == L'/')
	{
		const int buf_len = 1024;
		wchar_t buf[buf_len];
		memset(buf, 0, sizeof(buf));
		wcscpy_s(buf, &szCommand[1]);

		return m_CmdManager.DoCommand(buf);
	}
	return false;
}

void XChatCmds::Output(const wchar_t* szText)
{
	global.ui->OnSystemMsg(szText);
}

void _AddCmdFromXml(XStrCmdManager* pCmdManager, XChatCmdLoader* pParser, 
					int nCmdID, XStrCmdProc* fnProc, unsigned long int flag,
					int nMinArgs, int nMaxArgs, bool bRepeatEnabled)
{
	XChatCmdLoader::_CmdStr* pCmdStr = pParser->GetStr(nCmdID);
	if (pCmdStr)
	{
		pCmdManager->AddCommand(nCmdID, pCmdStr->szName, fnProc, nMinArgs, nMaxArgs, pCmdStr->szUsage, pCmdStr->szHelp);

		for (int i = 0; i < (int)pCmdStr->vecAliases.size(); i++)
		{
			pCmdManager->AddAlias(pCmdStr->vecAliases[i].c_str(), pCmdStr->szName);
		}
	}
}

#define _CC_AC(X1,X2,X3,X4,X5,X6)			m_CmdManager.AddCommand(0,X1,X2,X3,X4,X5,X6)
#define _CC_ALIAS(NEW,TAR)					m_CmdManager.AddAlias(NEW,TAR)
#define _CC_ACX(X1,X2,X3,X4,X5,X6)			_AddCmdFromXml(&m_CmdManager, &chat_cmd_loader,X1,X2,X3,X4,X5,X6)

void XChatCmds::InitCmds()
{
	XChatCmdLoader chat_cmd_loader;
	if (!chat_cmd_loader.Load())
	{
		mlog("Error while Read Item Descriptor %s", "system/chatcmds.xml");
	}

	_CC_ACX(CCMD_ID_HELP,					&ChatCmd_Help,			0, ARGVNoMin, 1, true);
	_CC_ACX(CCMD_ID_DIE,					&ChatCmd_Die,			0, 0, 0, true);
	_CC_ACX(CCMD_ID_AFK,					&ChatCmd_AFK,			0, 0, 0, true);

	// 채팅
	_CC_ACX(CCMD_ID_SAY,					&ChatCmd_Say,			0, ARGVNoMin, 1, true);
	_CC_ACX(CCMD_ID_TELL,					&ChatCmd_Tell,			0, ARGVNoMin, 2, true);
	_CC_ACX(CCMD_ID_REPLY,					&ChatCmd_Reply,			0, ARGVNoMin, 1, true);
	_CC_ACX(CCMD_ID_PARTY,					&ChatCmd_Party,			0, ARGVNoMin, 1, true);
	_CC_ACX(CCMD_ID_GUILD,					&ChatCmd_Guild,			0, ARGVNoMin, 1, true);
	_CC_ACX(CCMD_ID_SHOUT,					&ChatCmd_Shout,			0, ARGVNoMin, 1, true);
	_CC_ACX(CCMD_ID_AUCTION,				&ChatCmd_Auction,		0, ARGVNoMin, 1, true);
	_CC_ACX(CCMD_ID_CHANNEL,				&ChatCmd_Channel,		0, ARGVNoMin, 2, true);
	_CC_ACX(CCMD_ID_FIELD,					&ChatCmd_Field,			0, ARGVNoMin, 1, true);
	_CC_ACX(CCMD_ID_GLOBAL,					&ChatCmd_Global,		0, ARGVNoMin, 1, true);
	_CC_ACX(CCMD_ID_NARRATION,				&ChatCmd_Narration,		0, ARGVNoMin, 1, true);
	_CC_ACX(CCMD_ID_ANNOUNCE,				&ChatCmd_Announce,		0, ARGVNoMin, 1, true);
	_CC_ACX(CCMD_ID_NOTICE,					&ChatCmd_Notice,		0, ARGVNoMin, 1, true);
	
	// 채팅 채널 명령
	_CC_ACX(CCMD_ID_JOIN,					&ChatCmd_ChannelJoin,	0, ARGVNoMin, 1, true);
	_CC_ACX(CCMD_ID_LEAVE,					&ChatCmd_ChannelLeave,	0, ARGVNoMin, 1, true);
	_CC_ACX(CCMD_ID_KICK,					&ChatCmd_ChannelKick,	0, ARGVNoMin, 2, true);
	_CC_ACX(CCMD_ID_BAN,					&ChatCmd_ChannelBan,	0, ARGVNoMin, 2, true);
	_CC_ACX(CCMD_ID_UNBAN,					&ChatCmd_ChannelUnban,	0, ARGVNoMin, 2, true);
	_CC_ACX(CCMD_ID_OWNER,					&ChatCmd_ChannelOwner,	0, ARGVNoMin, 2, true);
	_CC_ACX(CCMD_ID_CREATE,					&ChatCmd_ChannelCreate, 0, ARGVNoMin, 1, true);

	// 소셜 모션
	_CC_ACX(CCMD_ID_SOCIAL_MOTION_BEG,		&ChatCmd_SocialMotion_Beg,	0, 0, 0, true);
	_CC_ACX(CCMD_ID_SOCIAL_MOTION_BOW,		&ChatCmd_SocialMotion_Bow,	0, 0, 0, true);
	_CC_ACX(CCMD_ID_SOCIAL_MOTION_CHEER,	&ChatCmd_SocialMotion_Cheer,	0, 0, 0, true);
	_CC_ACX(CCMD_ID_SOCIAL_MOTION_CLAP,		&ChatCmd_SocialMotion_Clap,	0, 0, 0, true);
	_CC_ACX(CCMD_ID_SOCIAL_MOTION_CRY,		&ChatCmd_SocialMotion_Cry,	0, 0, 0, true);
	_CC_ACX(CCMD_ID_SOCIAL_MOTION_DANCE,	&ChatCmd_SocialMotion_Dance,	0, 0, 0, true);
	_CC_ACX(CCMD_ID_SOCIAL_MOTION_IGNORE,	&ChatCmd_SocialMotion_Ignore,	0, 0, 0, true);
	_CC_ACX(CCMD_ID_SOCIAL_MOTION_KISS,		&ChatCmd_SocialMotion_Kiss,	0, 0, 0, true);
	_CC_ACX(CCMD_ID_SOCIAL_MOTION_LAUGH,	&ChatCmd_SocialMotion_Laugh,	0, 0, 0, true);
	_CC_ACX(CCMD_ID_SOCIAL_MOTION_LOOKAROUND,	&ChatCmd_SocialMotion_Lookaround,	0, 0, 0, true);
	_CC_ACX(CCMD_ID_SOCIAL_MOTION_NEGATIVE,	&ChatCmd_SocialMotion_Negative,	0, 0, 0, true);
	_CC_ACX(CCMD_ID_SOCIAL_MOTION_POINT,	&ChatCmd_SocialMotion_Point,	0, 0, 0, true);
	_CC_ACX(CCMD_ID_SOCIAL_MOTION_POSITIVE,	&ChatCmd_SocialMotion_Positive,	0, 0, 0, true);
	_CC_ACX(CCMD_ID_SOCIAL_MOTION_SALUTE,	&ChatCmd_SocialMotion_Salute,	0, 0, 0, true);
	_CC_ACX(CCMD_ID_SOCIAL_MOTION_SIGH,		&ChatCmd_SocialMotion_Sigh,	0, 0, 0, true);
	_CC_ACX(CCMD_ID_SOCIAL_MOTION_TALK1,	&ChatCmd_SocialMotion_Talk1,	0, 0, 0, true);
	_CC_ACX(CCMD_ID_SOCIAL_MOTION_TALK2,	&ChatCmd_SocialMotion_Talk2,	0, 0, 0, true);
	_CC_ACX(CCMD_ID_SOCIAL_MOTION_TAUNT,	&ChatCmd_SocialMotion_Taunt,	0, 0, 0, true);
	_CC_ACX(CCMD_ID_SOCIAL_MOTION_WAVE,		&ChatCmd_SocialMotion_Wave,	0, 0, 0, true);

	// 콘솔 명령어 등록
	// 명령어 이름, 실행 함수 포인터, 최소인자개수, 최대인자개수, 사용법, 도움말
}

bool XChatCmds::ChatCmd_Help(const wchar_t* line, const int argc, wchar_t **const argv)
{
	if (argc == 1)
	{
		wchar_t szBuf[1024]=L"";

		for (XStrCmdMap::iterator itor = m_CmdManager.GetCmdBegin(); itor != m_CmdManager.GetCmdEnd(); ++itor)
		{
			XStrCmd* pCmd = (*itor).second;
			swprintf_s(szBuf, L"[%s] : %s", pCmd->GetName(), pCmd->GetHelp());
			Output(szBuf);
		}
	}
	else if (argc > 1)
	{
		XStrCmd* pHelpCmd = m_CmdManager.GetCommandByName(argv[1]);
		if (pHelpCmd)
		{
			wchar_t text[512]=L"";
			swprintf_s(text, L"Usage: %s", pHelpCmd->GetUsage());
			Output(text);

			Output(pHelpCmd->GetHelp());
		}
		else
		{
			Output(L"That command does not exist.");
		}
	}

	return true;
}

bool XChatCmds::ChatCmd_Die(const wchar_t* line, const int argc, wchar_t **const argv)
{
	XPostChar_Die();

	return true;
}

bool XChatCmds::ChatCmd_AFK(const wchar_t* line, const int argc, wchar_t **const argv)
{
	XPostChar_AFK();

	return true;
}

bool XChatCmds::ChatCmd_Say(const wchar_t* line, const int argc, wchar_t **const argv)
{
	if (argc == 2)
	{
		if (argv[1][0] != 0)
		{
			XPostMsg_Say(argv[1]);
			return true;
		}
	}
	return true;
}

bool XChatCmds::ChatCmd_Tell(const wchar_t* line, const int argc, wchar_t **const argv)
{
	if (argc == 3)
	{
		if (argv[2][0] != 0)
		{
			XPostMsg_Tell(argv[1], argv[2]);
			return true;
		}
	}
	return true;
}

bool XChatCmds::ChatCmd_Reply(const wchar_t* line, const int argc, wchar_t **const argv)
{
	if (argc == 2)
	{
		if (!gvar.Game.strLastWhisper.empty())
		{
			if (argv[1][0] != 0)
			{
				XPostMsg_Reply(argv[1]);
			}
		}
	}
	return true;
}

bool XChatCmds::ChatCmd_Party(const wchar_t* line, const int argc, wchar_t **const argv)
{
	if (argc == 2)
	{
		if (gvar.MyInfo.Party.IsJoined())
		{
			if (argv[1][0] != 0)
			{
				XPostMsg_Party(argv[1]);
			}
		}
		else
		{
			Output(L"You are currently not in a party.");
		}
	}
	return true;
}

bool XChatCmds::ChatCmd_Guild(const wchar_t* line, const int argc, wchar_t **const argv)
{
	if (argc == 2)
	{
		if (gvar.MyInfo.Guild.IsExist())
		{
			if (argv[1][0] != 0)
				XPostMsg_Guild(argv[1]);
		}
		else
		{
			Output(L"You are not a member of a guild.");
		}
	}
	return true;
}
	
bool XChatCmds::ChatCmd_Shout(const wchar_t* line, const int argc, wchar_t **const argv)
{
	if (argc == 2)
	{
		if (argv[1][0] != 0)
			XPostMsg_Shout(argv[1]);
	}
	return true;
}

bool XChatCmds::ChatCmd_Auction(const wchar_t* line, const int argc, wchar_t **const argv)
{
	if (argc == 2)
	{
		if (argv[1][0] != 0)
			XPostMsg_Auction(argv[1]);
	}
	return true;
}

bool XChatCmds::ChatCmd_Channel(const wchar_t* line, const int argc, wchar_t **const argv)
{
	if (argc == 3)
	{
		if (argv[2][0] != 0)
		{
			XChatChannel* pChannel = info.chat_channel->Find(argv[1]);
			if (pChannel == NULL)
			{
				Output(L"That channel does not exist.");
				return true;
			}
			
			MUID uidChannel = pChannel->GetUID();
			XPostMsg_Channel(uidChannel, argv[2]);
		}
	}
	return true;
}

bool XChatCmds::ChatCmd_Field(const wchar_t* line, const int argc, wchar_t **const argv)
{
	if (argc == 2)
	{
		if (argv[1][0] != 0)
			XPostMsg_Field(argv[1]);
	}
	return true;
}

bool XChatCmds::ChatCmd_Global(const wchar_t* line, const int argc, wchar_t **const argv)
{
	if (argc == 2)
	{
		if (argv[1][0] != 0)
			XPostMsg_Global(argv[1]);
	}
	return true;
}

bool XChatCmds::ChatCmd_Narration(const wchar_t* line, const int argc, wchar_t **const argv)
{
	// gm 전용
	if (gvar.MyInfo.ChrInfo.nPlayerGrade < PLAYER_GRADE_GM)
	{
		return false;
	}

	if (argc == 2)
	{
		if (argv[1][0] != 0)
			XPostMsg_Narration(argv[1]);
	}
	return true;
}

bool XChatCmds::ChatCmd_Announce(const wchar_t* line, const int argc, wchar_t **const argv)
{
	// gm 전용
	if (gvar.MyInfo.ChrInfo.nPlayerGrade < PLAYER_GRADE_GM)
	{
		return false;
	}

	if (argc == 2)
	{
		if (argv[1][0] != 0)
			XPostMsg_Announce(argv[1]);
	}
	return true;
}

bool XChatCmds::ChatCmd_Notice(const wchar_t* line, const int argc, wchar_t **const argv)
{
	// gm 전용
	if (gvar.MyInfo.ChrInfo.nPlayerGrade < PLAYER_GRADE_GM)
	{
		return false;
	}

	if (argc == 2)
	{
		if (argv[1][0] != 0)
			XPostMsg_Notice(argv[1]);
	}
	return true;
}

bool XChatCmds::ChatCmd_ChannelJoin(const wchar_t* line, const int argc, wchar_t **const argv)
{
	if (argc == 2)
	{
		if (argv[1][0] != 0)
			XPostChatChannel_Join(argv[1]);
	}
	return true;
}

bool XChatCmds::ChatCmd_ChannelLeave(const wchar_t* line, const int argc, wchar_t **const argv)
{
	if (argc == 2)
	{
		if (argv[1][0] != 0)
		{
			XChatChannel* pChannel = info.chat_channel->Find(argv[1]);
			if (pChannel == NULL)
			{
				Output(L"입장하지 않은 채팅 채널입니다.");
				return true;
			}
			
			MUID uidChannel = pChannel->GetUID();
			XPostChatChannel_Leave(uidChannel);
		}
	}
	return true;
}

bool XChatCmds::ChatCmd_ChannelKick(const wchar_t* line, const int argc, wchar_t **const argv)
{
	if (argc == 3)
	{
		if (argv[1][0] != 0 && argv[2][0] != 0)
		{
			XChatChannel* pChannel = info.chat_channel->Find(argv[1]);
			if (pChannel == NULL)
			{
				Output(L"입장하지 않은 채팅 채널입니다.");
				return true;
			}

			if (!pChannel->IsOwner())
			{
				Output(L"방장 전용 명령입니다.");
				return true;
			}
			
			MUID uidChannel = pChannel->GetUID();
			XPostChatChannel_Kick(uidChannel, argv[2]);
		}
	}
	return true;
}

bool XChatCmds::ChatCmd_ChannelBan(const wchar_t* line, const int argc, wchar_t **const argv)
{
	if (argc == 3)
	{
		if (argv[1][0] != 0 && argv[2][0] != 0)
		{
			XChatChannel* pChannel = info.chat_channel->Find(argv[1]);
			if (pChannel == NULL)
			{
				Output(L"입장하지 않은 채팅 채널입니다.");
				return true;
			}

			if (!pChannel->IsOwner())
			{
				Output(L"방장 전용 명령입니다.");
				return true;
			}
			
			MUID uidChannel = pChannel->GetUID();
			XPostChatChannel_Ban(uidChannel, argv[2]);
		}
	}
	return true;
}

bool XChatCmds::ChatCmd_ChannelUnban(const wchar_t* line, const int argc, wchar_t **const argv)
{
	if (argc == 3)
	{
		if (argv[1][0] != 0 && argv[2][0] != 0)
		{
			XChatChannel* pChannel = info.chat_channel->Find(argv[1]);
			if (pChannel == NULL)
			{
				Output(L"입장하지 않은 채팅 채널입니다.");
				return true;
			}

			if (!pChannel->IsOwner())
			{
				Output(L"방장 전용 명령입니다.");
				return true;
			}
			
			MUID uidChannel = pChannel->GetUID();
			XPostChatChannel_Unban(uidChannel, argv[2]);
		}
	}
	return true;
}

bool XChatCmds::ChatCmd_ChannelOwner(const wchar_t* line, const int argc, wchar_t **const argv)
{
	if (argc == 3)
	{
		if (argv[1][0] != 0 && argv[2][0] != 0)
		{
			XChatChannel* pChannel = info.chat_channel->Find(argv[1]);
			if (pChannel == NULL)
			{
				Output(L"입장하지 않은 채팅 채널입니다.");
				return true;
			}

			if (!pChannel->IsOwner())
			{
				Output(L"방장 전용 명령입니다.");
				return true;
			}
			
			MUID uidChannel = pChannel->GetUID();
			XPostChatChannel_Owner(uidChannel, argv[2]);
		}
	}
	return true;
}

bool XChatCmds::ChatCmd_ChannelCreate(const wchar_t* line, const int argc, wchar_t **const argv)
{
	if (argc == 2)
	{
		if (argv[1][0] != 0)
		{
			XPostChatChannel_Create(argv[1]);
		}
	}
	return true;
}

static void _UseSocialTalent(CSSocialMotionType nSocialMotionType)
{
	if (gvar.Game.pMyPlayer)
	{
		gvar.Game.pMyPlayer->GetModuleMyControl()->DoActionUseTalent(CSTalentInfoHelper::SocialMotionTalentID(nSocialMotionType));
	}
}

bool XChatCmds::ChatCmd_SocialMotion_Beg(const wchar_t* line, const int argc, wchar_t **const argv)
{
	_UseSocialTalent(SOCIAL_MOTION_BEG);
	return true;
}

bool XChatCmds::ChatCmd_SocialMotion_Bow(const wchar_t* line, const int argc, wchar_t **const argv)
{
	_UseSocialTalent(SOCIAL_MOTION_BOW);
	return true;
}

bool XChatCmds::ChatCmd_SocialMotion_Cheer(const wchar_t* line, const int argc, wchar_t **const argv)
{
	_UseSocialTalent(SOCIAL_MOTION_CHEER);
	return true;
}

bool XChatCmds::ChatCmd_SocialMotion_Clap(const wchar_t* line, const int argc, wchar_t **const argv)
{
	_UseSocialTalent(SOCIAL_MOTION_CLAP);
	return true;
}
bool XChatCmds::ChatCmd_SocialMotion_Cry(const wchar_t* line, const int argc, wchar_t **const argv)
{
	_UseSocialTalent(SOCIAL_MOTION_CRY);
	return true;
}
bool XChatCmds::ChatCmd_SocialMotion_Dance(const wchar_t* line, const int argc, wchar_t **const argv)
{
	_UseSocialTalent(SOCIAL_MOTION_DANCE);
	return true;
}
bool XChatCmds::ChatCmd_SocialMotion_Ignore(const wchar_t* line, const int argc, wchar_t **const argv)
{
	_UseSocialTalent(SOCIAL_MOTION_IGNORE);
	return true;
}
bool XChatCmds::ChatCmd_SocialMotion_Kiss(const wchar_t* line, const int argc, wchar_t **const argv)
{
	_UseSocialTalent(SOCIAL_MOTION_KISS);
	return true;
}
bool XChatCmds::ChatCmd_SocialMotion_Laugh(const wchar_t* line, const int argc, wchar_t **const argv)
{
	_UseSocialTalent(SOCIAL_MOTION_LAUGH);
	return true;
}
bool XChatCmds::ChatCmd_SocialMotion_Lookaround(const wchar_t* line, const int argc, wchar_t **const argv)
{
	_UseSocialTalent(SOCIAL_MOTION_LOOKAROUND);
	return true;
}
bool XChatCmds::ChatCmd_SocialMotion_Negative(const wchar_t* line, const int argc, wchar_t **const argv)
{
	_UseSocialTalent(SOCIAL_MOTION_NEGATIVE);
	return true;
}
bool XChatCmds::ChatCmd_SocialMotion_Point(const wchar_t* line, const int argc, wchar_t **const argv)
{
	_UseSocialTalent(SOCIAL_MOTION_POINT);
	return true;
}
bool XChatCmds::ChatCmd_SocialMotion_Positive(const wchar_t* line, const int argc, wchar_t **const argv)
{
	_UseSocialTalent(SOCIAL_MOTION_POSITIVE);
	return true;
}
bool XChatCmds::ChatCmd_SocialMotion_Salute(const wchar_t* line, const int argc, wchar_t **const argv)
{
	_UseSocialTalent(SOCIAL_MOTION_SALUTE);
	return true;
}
bool XChatCmds::ChatCmd_SocialMotion_Sigh(const wchar_t* line, const int argc, wchar_t **const argv)
{
	_UseSocialTalent(SOCIAL_MOTION_SIGH);
	return true;
}
bool XChatCmds::ChatCmd_SocialMotion_Talk1(const wchar_t* line, const int argc, wchar_t **const argv)
{
	_UseSocialTalent(SOCIAL_MOTION_TALK1);
	return true;
}
bool XChatCmds::ChatCmd_SocialMotion_Talk2(const wchar_t* line, const int argc, wchar_t **const argv)
{
	_UseSocialTalent(SOCIAL_MOTION_TALK2);
	return true;
}
bool XChatCmds::ChatCmd_SocialMotion_Taunt(const wchar_t* line, const int argc, wchar_t **const argv)
{
	_UseSocialTalent(SOCIAL_MOTION_TAUNT);
	return true;
}
bool XChatCmds::ChatCmd_SocialMotion_Wave(const wchar_t* line, const int argc, wchar_t **const argv)
{
	_UseSocialTalent(SOCIAL_MOTION_WAVE);
	return true;
}
