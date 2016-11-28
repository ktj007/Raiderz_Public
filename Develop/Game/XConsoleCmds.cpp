#include "stdafx.h"
#include "XConsoleCmds.h"
#include "MConsole.h"
#include "XPost_Local.h"
#include "XPost_GM.h"
#include "XPost_Quest.h"
#include "XItemContainer.h"
#include "XCameraEffect.h"
#include "XScreenDebugger.h"
#include "XMyPlayer.h"
#include "XItem.h"
#include "CSItemData.h"
#include "XGame.h"
#include "XWorld.h"
#include "XTalentInfo.h"
#include "XConst.h"
#include "XItemManager.h"
#include "RConfiguration.h"
#include "RCameraShakeController.h"
#include "XModuleMyControl.h"
#include "XController.h"
#include "XPost_Loot.h"
#include "XPost_Training.h"
#include "XPost_Craft.h"
#include "XSystem.h"
#include "XCameraManager.h"
#include "XPost_Guild.h"
#include "XPost_Item.h"
#include "XEngineDebugger.h"
#include "XTalentInfoMgr.h"
#include "XPost_Channel.h"
#include "XPost_Comm.h"
#include "XPost_Trade.h"
#include "XPost_Sensor.h"
#include "AString.h"
#include "XMLogFrm.h"
#include "XNetwork.h"
#include "CSDef_Loot.h"
#include "XNPCInfo.h"
#include "XItemManager.h"
#include "XTalentEffectMgr.h"
#include "XEffectInfo.h"
#include "XWidgetNameDef.h"
#include "MStringUtil.h"
#include "XPost_Msg.h"
#include "XFreeCamTracer.h"
#include "XHelp.h"
#include "XNonPlayer.h"
#include "XDialogTextInfoMgr.h"
#include "XStrings.h"
#include "XBaseApplication.h"
#include "XGameInitializer.h"


XStrCmdManager XConsoleCmds::m_CmdManager;
MConsole* XConsoleCmds::m_pConsole = NULL;

void XConsoleCmds::Init(MConsole* pConsole)
{
	if (pConsole)
	{
		m_pConsole = pConsole;
		m_pConsole->SetCallback(XConsoleCmds::_Input);
	}
	m_CmdManager.Clear();
	
	// Stable 빌드(_PUBLISH)는 내부 테스트용 안정버젼으로 대부분의 콘솔 명령어가 사용 가능해야 합니다. - aibeast
	// 일반 커멘드, GM 커멘드, Test 커멘드 등의 분류가 애매하여 일단 모두 사용 가능하도록 해둡니다.
	// TODO : CBT2 이전에 커맨드 명령어 분류가 필요합니다. - aibeast
	InitCmds();
	InitTestCmds();
	//InitUnusedCmds();

	InitGMCmds();
}


void XConsoleCmds::_Input(const char* szCommand)
{
	Input(MLocale::ConvAnsiToUTF16(szCommand).c_str());
}

bool XConsoleCmds::Input(const wchar_t* szCommand)
{
	wstring strBuf = szCommand;
	strBuf = cml2::TrimBlanks(strBuf);

	if (strBuf.length() > 1)
	{
		if (strBuf[0] == L'/')
		{
			strBuf.replace(0, 1, L"");
		}
	}

	wchar_t buf[1024];
	wcscpy_s(buf, strBuf.c_str());

	if (m_CmdManager.DoCommand(buf) == false)
	{
		return false;
	}

//#ifndef _PUBLISH
	if (global.script)
	{
		global.script->GetGlueDebugEvent().OnConsoleInput(MLocale::ConvUTF16ToAnsi(szCommand).c_str());
	}
//#endif

	return true;
}

void XConsoleCmds::Output(const wchar_t* szText)
{
	if (m_pConsole)
	{
		m_pConsole->OutputMessage(MLocale::ConvUTF16ToAnsi(szText).c_str());
	}

	global.ui->OnSystemMsg(szText);
}

void XConsoleCmds::OutputSyntaxError()
{
	Output(L"syntax error");
}

#define _CC_AC(X1,X2,X3,X4,X5,X6)			m_CmdManager.AddCommand(0,X1,X2,X3,X4,X5,X6)
#define _CC_ALIAS(NEW,TAR)					m_CmdManager.AddAlias(NEW,TAR)

void XConsoleCmds::InitCmds()
{
	// 콘솔 명령어 등록
	// 명령어 이름, 실행 함수 포인터, 최소인자개수, 최대인자개수, 사용법, 도움말
	_CC_AC(L"help",			&ConsoleCmd_Help,				ARGVNoMin,	1,			L"help <명령어>",	L"특정 명령어의 자세한 사용법을 알려줍니다.");
	_CC_AC(L"clog",			&ConsoleCmd_CmdLog,				1,			ARGVNoMax,	L"clog | clog d <CmdID> | clog a <CmdID> | clog ilist",		L"CommandLog창을 보이거나 숨깁니다.");

	_CC_AC(L"mlog",			&ConsoleCmd_MLog,				0,	0,	L"mlog", L"MLog창을 보이거나 숨깁니다.");

	_CC_AC(L"reload",		&ConsoleCmd_Reload,			1,			ARGVNoMax,	L"reload <c/s/ui/lan/all>",	L"데이터를 새로 로드합니다. (npc/talent/ui/lan)");
	_CC_ALIAS(L"ㄱ디ㅐㅁㅇ", L"reload");

	_CC_AC(L"designer",		&ConsoleCmd_DesignerMode,		1,			ARGVNoMax,	L"designer",			L"UI 편집 모드로 변경");
	_CC_AC(L"exec",			&ConsoleCmd_Exec,				1,	1,	L"exec <func>",	L"루아 함수를 실행한다.");

	_CC_AC(L"print",		&ConsoleCmd_Print,				1,	1,	L"print <Var>",	L"값을 출력한다.(출력 가능 정보: myuid, mystatus, marker, sensor, field, slot, item, party, durability, talent)");
	_CC_AC(L"show",			&ConsoleCmd_Show,				1,	2,	L"show <Var>",	L"Var = col, scrlog, ui, widget, sector, hit, aabb, area, id");
	_CC_AC(L"enable",		&ConsoleCmd_Enable,				1,	2,	L"enable <type> <var>",	L"type = script, var = true/false");


	_CC_AC(L"enemy",			&ConsoleCmd_ShowEnemyUID,				0,	0,	L"enemy",L"어그로를 가진 NPC");


	_CC_AC(L"ai",			&ConsoleCmd_AIRunning,			0,	0,	L"ai",	L"[GM] NPC 인공지능 활성화");
	_CC_ALIAS(L"먀", L"ai");

	_CC_AC(L"aicmd",			&ConsoleCmd_AICommand,			3,	3,	L"aicmd <uidNPC> <Command> <Param>",	L"[GM] NPC 명령한다");
	_CC_AC(L"npc",			&ConsoleCmd_AISetNPC,			0,	1,	L"aisetnpc <NPCUIID>",				L"[GM] 서버에서 모니터링할 NPC 선택");
	_CC_AC(L"report",		&ConsoleCmd_ReportTargetEntity,	1,	1,	L"report <uidTarget>",	L"특정 대상에 대한 정보를 보고함 (서버 머신에 로컬 파일로 생성 됨)");
	_CC_AC(L"ds",			&ConsoleCmd_DebugString,			3,	3,	L"ds <KEY> <PARAM> <TEXT>",		L"Debug String");
	_CC_AC(L"set",			&ConsoleCmd_Set,				2, 2, L"set <TYPE> <VALUE>", L"시스템 정보를 수정한다. TYPE['fov']");

	_CC_AC(L"rbpart",		&ConsoleCmd_GMRangeBreakPart,	0,	1,	L"rbpart <fRange>", L"범위(범위 안적는 경우 기본 범위1000)안의 NPC의 브레이커블 파츠를 부숩니다");

	_CC_AC(L"logcrti",		&ConsoleCmd_LogCRTInsert,		1,	1,	L"logcrti <CRT>", L"서버 로그에 남길 CRT를 추가합니다.");
	_CC_AC(L"logcrtd",		&ConsoleCmd_LogCRTDelete,		1,	1,	L"logcrtd <CRT>", L"서버 로그에 남길 CRT를 제거합니다.");


	_CC_AC(L"invensort",	&ConsoleCmd_InvenSort,			1,	1,	L"invensort",	L"인벤 아이템 정렬");
}

void XConsoleCmds::InitGMCmds()
{
	// 이부분은 운영자도 사용 가능
	_CC_AC(L"rebirth",		&ConsoleCmd_Rebirth,			ARGVNoMin,	ARGVNoMax,	L"rebirth",	L"부활한다.");
	_CC_ALIAS(L"r",L"rebirth");
	_CC_ALIAS(L"ㄱ", L"rebirth");

	_CC_AC(L"spawn",		&ConsoleCmd_Spawn,				ARGVNoMin,	ARGVNoMax,	L"spawn <NPCID> <개수>",	L"NPC 스폰");
	_CC_ALIAS(L"넴주", L"spawn");

	_CC_AC(L"rct",			&ConsoleCmd_ResetCoolTime,		0,	0,	L"rct",	L"모든 쿨타임을 리셋합니다");
	_CC_ALIAS(L"ㄱㅊㅅ", L"rct");

	_CC_AC(L"mvp",			&ConsoleCmd_MoveToPlayer,		3,	3,	L"mvp <Player ID>",	L"[GM] 해당 플레이어에게 이동");
	_CC_ALIAS(L"ㅡ페", L"mvp");

	_CC_AC(L"mvm",			&ConsoleCmd_MoveToMarker,		1,	2,	L"mvm <Marker ID> | mvm <FIELD_ID> <Marker ID>",	L"[GM] 해당 마커로 이동");
	_CC_ALIAS(L"ㅡ프", L"mvm");

	_CC_AC(L"move",			&ConsoleCmd_Move,				3,	4,	L"move <FIELD_ID> <POS_X> <POS_Y> <POS_Z> | move <FIELD_ID> <POS_X> <POS_Y> | move <POS_X> <POS_Y>",	L"[GM] 해당 위치로 이동");
	_CC_ALIAS(L"ㅡㅐㅍㄷ", L"move");

	_CC_AC(L"mvn",			&ConsoleCmd_MoveToNPC,			1,	2,	L"mvn <NPC ID>",	L"[GM] 해당 엔피씨에게 이동");
	_CC_ALIAS(L"ㅡ푸", L"mvn");

	_CC_AC(L"mvs",			&ConsoleCmd_MoveToSensor,		3,	3,	L"mvs <Sensor ID>",	L"[GM] 해당 센서로 이동");
	_CC_ALIAS(L"ㅡㅍㄴ", L"mvs");

	_CC_AC(L"god",			&ConsoleCmd_God,				0,	0,	L"god",	L"[GM] 현재 캐릭터가 무적이 됩니다.");
	_CC_ALIAS(L"g",L"god");
	_CC_ALIAS(L"ㅎ", L"god");
	_CC_ALIAS(L"행", L"god");

	_CC_AC(L"changeweather",&ConsoleCmd_GMChangeWeather,	1,	1,	L"changeweather <WEATHER_TYPE>", L"[GM] 날씨를 변경합니다. 0-SUN, 1-CLD, 2-RNY, 3-HRN, 4-SNW, 5-HSW");
	_CC_AC(L"changetime",	&ConsoleCmd_GMChangeTime,		0,	1,	L"changetime <TIME_TYPE>", L"[GM] 시간을 변경합니다. 0-DAWN, 1-DAYTIME, 2-SUNSET, 3-NIGHT");

	_CC_AC(L"setme",		&ConsoleCmd_SetMe,				2, 2,	L"setme <TYPE> <VALUE>", L"[GM] 내 캐릭터 정보를 수정한다. TYPE['level', 'tp', 'silver', 'hp', 'grade', 'preset'], PRESET VALUE(defender, defender20, cleric, cleric20, hybrid, hybrid20, berserker, berserker20)");
	_CC_ALIAS(L"ㄴㄷ슫", L"setme");

	_CC_AC(L"ghost",		&ConsoleCmd_Ghost,				0,	0,	L"ghost",	L"[GM] 현재 캐릭터가 고스트 상태가 됩니다.");
	_CC_AC(L"die",			&ConsoleCmd_KillMe,				0, 0,	L"die", L"자살...");
	_CC_ALIAS(L"얃", L"die");

	_CC_AC(L"despawn",		&ConsoleCmd_Despawn,			0,	1,	L"despawn <nRange>", L"NPC 디스폰");
	_CC_ALIAS(L"ㅇㄷ넴주", L"despawn");

	_CC_AC(L"despawnbyid",	&ConsoleCmd_DespawnByID,		1,	2,	L"despawn <nNPCID> <nRange>", L"NPCID로 디스폰");
	_CC_AC(L"find",			&ConsoleCmd_Find,				1,	2,	L"find <str> | find npc <str> | find item <str> | find quest <str> | find talent <str> | find buff <str>",	L"ID 찾기");
	_CC_ALIAS(L"랴ㅜㅇ", L"find");


	_CC_AC(L"rkn",			&ConsoleCmd_RangeKillNPC,		0, 1, L"rkn <nRange>", L"범위(범위 안적는 경우 기본1000)만큼의 NPC를 죽입니다");
	_CC_ALIAS(L"가ㅜ", L"rkn");

	_CC_AC(L"giveitem",		&ConsoleCmd_GiveItem,			1,	2,	L"giveitem <ITEM_ID> <ITEM_QUANTITY>", L"[테스트] 아이템 제공");	
	_CC_ALIAS(L"햪댯드", L"giveitem");

	_CC_AC(L"repair",		&ConsoleCmd_Repair,				0, 0, L"repair", L"장착하고 있는 아이템을 전부 수리");
	_CC_AC(L"clearinven",	&ConsoleCmd_ClearInven,			0, 0, L"clearinven", L"인벤토리의 아이템을 전부 삭제");

	
	_CC_AC(L"summon",		&ConsoleCmd_Summon,				3,	3,	L"summon <Player ID>",	L"[GM] 해당 플레이어에게 이동");
	_CC_AC(L"regen",		&ConsoleCmd_Regen,				0, 0, L"regen", L"생명력/정신력/기력[자동회복] 토글");	

	_CC_AC(L"factioni",		&ConsoleCmd_FactionIncrease,	2,	2,	L"factioni <FactionID> <Quantity>", L"<FactionID>를 <Quantity>만큼 팩션 증가");
	_CC_AC(L"factiond",		&ConsoleCmd_FactionDecrease,	2,	2,	L"factiond <FactionID> <Quantity>", L"<FactionID>를 <Quantity>만큼 팩션 감소");

	_CC_AC(L"changeservermode",	&ConsoleCmd_ChangeServerMode,	1,	1, L"chageservermode <MODE>", L"Chage Server Mode. 0-NORMAL, 1-EXPO");
	_CC_AC(L"resetexpochar",	&ConsoleCmd_ResetExpoCharacters,0,	0, L"resetexpochar", L"Reset Expo Characters.");
	_CC_ALIAS(L"resetexpo", L"resetexpochar");
	_CC_ALIAS(L"resetexpocharacters", L"resetexpochar");

	_CC_AC(L"ban", &ConsoleCmd_BanAccount, 1, 1, L"ban <char name>", L"Bans an account using the character name");

}


void XConsoleCmds::InitTestCmds()
{
	_CC_AC(L"setnpc",		&ConsoleCmd_SetNPC,				2, 2, L"setnpc <TYPE> <VALUE>", L"NPC의 상태값을 변경한다.");
	_CC_AC(L"gameguard",	&ConsoleCmd_GameGuardEnable,	1,	1,	L"gameguard",	L"[GM] GameGuard <on/off>");	

	_CC_AC(L"buffi",			&ConsoleCmd_GMInsertBuff,		3,	3,	L"buffi <BuffID> <DurationTime> <PeriodTime>",		L"[테스트] 버프 추가");
	_CC_AC(L"buffd",			&ConsoleCmd_GMDeleteBuff,		1,	1,	L"buffd <BuffID>",	L"[테스트] 버프 제거");

	_CC_AC(L"chchannel",		&ConsoleCmd_ChangeChannel,		1,	1,	L"chchannel <CHANNEL ID>",	L"[테스트] 채널을 바꿉니다.");
	_CC_AC(L"enter",			&ConsoleCmd_ChangeMap,			1,	1,	L"enter <DYNAMIC_FIELD ID>",	L"[테스트] 다이나믹 필드로 진입합니다.");
	_CC_AC(L"chlist",		&ConsoleCmd_ChannelList,		ARGVNoMin,	ARGVNoMax,	L"chlist",	L"[테스트] 채널 목록을 요청.");

	_CC_AC(L"qshare",		&ConsoleCmd_QuestShare,			1,	1,	L"qshare <QUEST_ID>", L"[테스트] 퀘스트 공유");
	_CC_AC(L"qgive",			&ConsoleCmd_QuestGive,			1,	1,	L"qgive <QUEST_ID>",	L"[테스트] 퀘스트를 부여");
	_CC_AC(L"qcancel",		&ConsoleCmd_QuestCancel,		1,	1,	L"qcancel <QUEST_ID>", L"[테스트] 퀘스트 취소");	
	_CC_AC(L"qreset",		&ConsoleCmd_QuestReset,			1,	1,	L"qreset", L"[테스트] 수행한 퀘스트 초기화");
	_CC_AC(L"qcomplete",	&ConsoleCmd_QuestComplete,		1,	1,	L"qcomplete <QUEST_ID>", L"[테스트] 퀘스트 완료");
	_CC_AC(L"qreward",		&ConsoleCmd_QuestReward,		1,	1,	L"qreward <QUEST_ID>", L"[테스트] 퀘스트 보상");
	_CC_AC(L"qvar",			&ConsoleCmd_QuestVar,			1,	1,	L"qvar <QUEST_ID>", L"[테스트] quest_var");
	_CC_AC(L"qfail",			&ConsoleCmd_QuestFail,			1,	1,	L"qfail <QUEST_ID>", L"[테스트] 퀘스트 실패");	
	
	_CC_AC(L"setitem",		&ConsoleCmd_SetItem,			3, 3, L"setitem <TYPE> <INVEN_SLOT_ID> <VALUE>", L"[GM] 내 아이템 정보를 수정한다. TYPE - 'durability'");




	_CC_AC(L"getpu",			&ConsoleCmd_GetPlayerUID,		1, 1, L"getpu <PLAYER ID>", L"해당 플레이어의 UID를 요청 합니다.");	
	_CC_AC(L"warpp",			&ConsoleCmd_WarpToPickingPos,	0, 0, L"", L"");
	_CC_AC(L"dye",			&ConsoleCmd_DyeItem,			0, 0, L"dye <0(머리), 1(얼굴), 2(손), 3(발), 4(몸통), 5(다리)>, <COLOR>", L"아이템을 염색한다.");
	_CC_AC(L"re",			&ConsoleCmd_ServerReload,		0, 0, L"re <n/f/t> <ID>", L"서버 데이터를 리로딩합니다. (npc/field/talent)");
	_CC_ALIAS(L"ㄱㄷ", L"re");

	_CC_AC(L"treport",		&ConsoleCmd_ServerReport,		0, 0, L"report <despawn>", L"서버 특정 디버깅 정보를 서버머신에 로그로 남깁니다. (despawn)");

	
	//크래프팅
	_CC_AC(L"ke",			&ConsoleCmd_KillEntity,			1, 1, L"ke <UIID>", L"해당 UIID의 엔터티를 죽입니다.");
	_CC_AC(L"sselect",		&ConsoleCmd_SensorSelect,				1, 1, L"sselect <number>", L"센서 목적지 중복 목록중 선택");
	_CC_AC(L"sinteract",		&ConsoleCmd_SensorInteract,				1, 1, L"sselect <sensor id>", L"센서 안에서 인터랙트 효과");

	_CC_AC(L"restoresinteract",		&ConsoleCmd_SensorInteract,				1, 1, L"sselect <sensor id>", L"센서 안에서 인터랙트 효과");

	// 길드
	_CC_AC(L"gcreate",		&ConsoleCmd_GuildCreate,		1, 1, L"gcreate <Guild Name>", L"길드 생성.");
	_CC_AC(L"ginviteq",		&ConsoleCmd_GuildInvite_Req,	1, 1, L"ginviteq <PLAYER ID>", L"길드 초대 요청.");
	_CC_AC(L"ginvites",		&ConsoleCmd_GuildInvite_Res,	1, 1, L"ginvites <0:거절 or 1:수락>", L"길드 초대 응답.");
	_CC_AC(L"gleave",		&ConsoleCmd_GuildLeave,			0, 0, L"gleave", L"길드 탈퇴.");
	_CC_AC(L"gdestroy",		&ConsoleCmd_GuildDestroy,		0, 0, L"gdestroy", L"길드 해체.");
	_CC_AC(L"gsay",			&ConsoleCmd_GuildSay,			1, 1, L"gsay <text>", L"길드말.");
	_CC_AC(L"gchangem",		&ConsoleCmd_GuildChangeMaster,	1, 1, L"gchangem <PLAYER ID>", L"길마 변경.");	


	// 로컬 테스트용
	_CC_AC(L"lchange_env",	&ConsoleCmd_LocalChangeEnv,		2, 2, L"lchange_env <WEATHER_TYPE> <TIME_TYPE>", L"[LOCALTEST] 날씨를 변경합니다. 0-SUN, 1-CLD, 2-RNY, 3-HRN, 4-SNW, 5-HSW | 0-DAWN,1-DAYTIME,2-SUNSET,3-NIGHT");
	_CC_AC(L"lspawn",		&ConsoleCmd_LocalSpawn,			1, 1, L"lspawn <NPC ID>", L"[LOCALTEST] 특정 NPC를 스폰시킵니다.");
	_CC_AC(L"ed",			&ConsoleCmd_EngineDebug,		ARGVNoMin, ARGVNoMax, L"ed <특정커맨드 / help>", L"[DEBUG] 엔진(그래픽/물리 등) 디버깅용 커맨드.");

	// 투기장
	_CC_AC(L"arena",			&ConsoleCmd_Arena,				1,	1,	L"arena <register/deregister>",	L"투기장 참가를 신청하거나 해지합니다.");

	// 로그
	_CC_AC(L"lualog",		&ConsoleCmd_LuaLog,				0,	0,	L"lualog",	L"루아로그창을 보이거나 숨깁니다.");
	_CC_AC(L"dump",			&ConsoleCmd_ServerDump,			0,	1,	L"dump or dump <ID>",	L"서버에 덤프를 남깁니다.");

	// 커맨드 프로파일링
	_CC_AC(L"start_cmd_profile",	&ConsoleCmd_StartCommandProfile,	0,	0,	L"start_cmd_profile",	L"서버의 커맨드 프로파일링 시작");
	_CC_AC(L"end_cmd_profile",	&ConsoleCmd_EndCommandProfile,	0,	0,	L"end_cmd_profile",	L"서버의 커맨드 프로파일링 종료");
	_CC_AC(L"local_cmd_profile",	&ConsoleCmd_LocalCommandProfile,	0,	0,	L"local_cmd_profile",	L"로컬 커맨드 프로파일링");

	// 프리 카메라 이동
	_CC_AC(L"freecam",	&ConsoleCmd_FreeCamera,	1,	2,	L"freecam <clear/run/stop/check/velocity>",	L"프리 카메라 이동");

	// 도움말
	_CC_AC(L"showhelp",	&ConsoleCmd_ShowHelp,	1,	1,	L"showhelp <ID>",	L"도움말 창 띄우기");
	_CC_AC(L"resethelp",	&ConsoleCmd_ResetHelp,	1,	1,	L"resethelp",	L"도움말 리셋");

	// 컷씬
	_CC_AC(L"start_cutscene",	&ConsoleCmd_StartCutScene,	1,	1,	L"start_cutscene <ID>",	L"컷씬을 시작합니다.");
	_CC_AC(L"end_cutscene",		&ConsoleCmd_EndCutScene,	1,	1,	L"start_cutscene <ID>",	L"컷씬을 끝냅니다.");

	// 가드 테스트
	_CC_AC(L"guard", &ConsoleCmd_Guard,	0,	0,	L"guard",	L"자동 가드 on/off");

	// 언어 테스트
	_CC_AC(L"locale", &ConsoleCmd_Locale, ARGVNoMin, 1, L"locale | locale <locale_code>", L"현재 로케일 설정을 알아보거나, 새 로케일을 설정합니다. ko_KR, en_US");
}

void XConsoleCmds::InitUnusedCmds()
{
	_CC_AC(L"cam",			&ConsoleCmd_Cam,				ARGVNoMin,	1,	L"cam <TargetUID>",	L"카메라 타겟을 해당 UID의 오브젝트로 설정합니다.");
}

bool XConsoleCmds::ConsoleCmd_Help(const wchar_t* line, const int argc, wchar_t **const argv)
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
			Output(L"해당 커맨드는 존재하지 않습니다.");
		}
	}
	return true;
}

bool XConsoleCmds::ConsoleCmd_Reload(const wchar_t* line, const int argc, wchar_t **const argv)
{
	XCommandLogFrm* pCLF = global.ui->GetCommandLog();
	if (!pCLF) return true;

	if (argc == 1)
	{
		XPostLocal_ReloadUI();
	}
	else if (argc == 2)
	{
		if (!_wcsicmp(argv[1], L"ui"))
		{
			XPostLocal_ReloadUI();
		}
		else if (!_wcsicmp(argv[1], L"c"))
		{
			ReloadInfo();
			gg.omgr->ReloadObjectAll();
		}
		else if (!_wcsicmp(argv[1], L"s"))
		{
//			XPostDebugString(L"re", -1, L"all");
		}
		else if (!_wcsicmp(argv[1], L"lan"))
		{
#ifndef _PUBLISH
			global.app->GetInitializer()->ReloadCountryLanguages( global.app->GetGlobalInfoInstance());
#endif
			XPostLocal_ReloadUI();
		}
		else if (!_wcsicmp(argv[1], L"all"))
		{
			ReloadInfo();
			gg.omgr->ReloadObjectAll();
//			XPostDebugString(L"re", -1, L"all");
		}
	}

	return true;
}

bool XConsoleCmds::ConsoleCmd_DesignerMode(const wchar_t* line, const int argc, wchar_t **const argv)
{
//#ifndef _PUBLISH
	static bool bDesignerMode = false;
	bDesignerMode = !bDesignerMode;
//	MSetDesignerMode(bDesignerMode);
//#endif
	return true;
}



//sample
bool XConsoleCmds::ConsoleCmd_CmdLog(const wchar_t* line, const int argc, wchar_t **const argv)
{
	XCommandLogFrm* pCLF = global.ui->GetCommandLog();
	if (!pCLF) return true;

	if (argc == 1)
	{
		if(pCLF->GetShow())
			pCLF->Show(false);
		else
		{
			pCLF->Show(true);
		}
	}
	else if (argc == 2)
	{
		if (!_wcsicmp(argv[1], L"ilost"))
		{
			wchar_t text[512]=L"";
			pCLF->GetIgnoredCmdListString(text, 512);
			Output(text);
		}
		else if (!_wcsicmp(argv[1], L"aall"))
		{
			pCLF->IgnoreCommandAll(false);
		}
		else if (!_wcsicmp(argv[1], L"dall"))
		{
			pCLF->IgnoreCommandAll();
		}
	}
	else if (argc >= 3)
	{
		if (!_wcsicmp(argv[1], L"d"))
		{
			int nID = _wtoi(argv[2]);
			pCLF->IgnoreCommand(nID);
		}
		else if (!_wcsicmp(argv[1], L"a"))
		{
			int nID = _wtoi(argv[2]);
			pCLF->IgnoreCommand(nID, false);
		}
	}
	return true;
}


//#ifndef _PUBLISH

bool XConsoleCmds::ConsoleCmd_Cam(const wchar_t* line, const int argc, wchar_t **const argv)
{
	if ((gg.omgr == NULL)) return true;

	MUID uidTarget;

	if (argc == 2) uidTarget = _wtoi64(argv[1]);
	else uidTarget = XGetMyUID();

	if (gg.omgr->Find(uidTarget) == NULL) 
	{
		Output(L"No Target");
		return true;
	}

	XCameraBackViewController* pCameraController = global.camera->GetCameraController<XCameraBackViewController>();
	if (pCameraController)
		pCameraController->Init(uidTarget);

	return true;
}

bool XConsoleCmds::ConsoleCmd_Print(const wchar_t* line, const int argc, wchar_t **const argv)
{
	wchar_t text[256]=L"";

	if (argc == 2)
	{
		if (!_wcsicmp(argv[1], L"myuid"))
		{
			swprintf_s(text, L"myuid = %I64u", XGetMyUID());
			Output(text);
			return true;
		}
		if (!_wcsicmp(argv[1], L"mystatus"))
		{
			wchar_t szChat[512];
			swprintf_s(szChat, 512, L"STR : %d\nDEX : %d\nINT : %d\nCHA : %d\nCON : %d\n", 
				gvar.MyInfo.GetSTR(),
				gvar.MyInfo.GetDEX(),
				gvar.MyInfo.GetINT(),
				gvar.MyInfo.GetCHA(),
				gvar.MyInfo.GetCON());

			Output(szChat);

			swprintf_s(szChat, L"HP: %d, EN: %d, STA: %d\n", gvar.MyInfo.ChrStatus.nHP, gvar.MyInfo.ChrStatus.nEN, gvar.MyInfo.ChrStatus.nSTA);
			Output(szChat);

		}
		else if (!_wcsicmp(argv[1], L"marker"))
		{
			wstring strText;
			MARKER_INFO_MAP_BY_ID& MarkerMap = gg.currentgamestate->GetWorld()->GetInfo()->m_mapMarkerByID;
			for (MARKER_INFO_MAP_BY_ID::iterator iter = MarkerMap.begin();
				iter != MarkerMap.end();
				++iter)
			{
				MARKER_INFO& Marker = iter->second;
				swprintf_s(text, 256, L"%3d | %s\n", Marker.nID, Marker.strName.c_str());

				strText += text;
			}
			Output(strText.c_str());
			return true;
		}
		else if (!_wcsicmp(argv[1], L"sensor"))
		{
			wstring strText;
			SENSOR_INFO_MAP_BY_ID& MarkerMap = gg.currentgamestate->GetWorld()->GetInfo()->m_mapSensorByID;
			for (SENSOR_INFO_MAP_BY_ID::iterator iter = MarkerMap.begin();
				iter != MarkerMap.end();
				++iter)
			{
				SENSOR_INFO& Sensor = iter->second;
				swprintf_s(text, 256, L"%3d | %s\n", Sensor.nID, Sensor.strName.c_str());

				strText += text;
			}
			Output(strText.c_str());
			return true;
		}
		else if (!_wcsicmp(argv[1], L"field"))
		{
			wstring strText;
			CSFieldList& field_list = info.field->GetFieldList();

			swprintf_s(text, 256, L"FIELD LIST\n"); strText += text;
			swprintf_s(text, 256, L"==========\n"); strText += text;


			for (CSFieldList::iterator itor = field_list.begin(); itor != field_list.end(); ++itor)
			{
				CSFieldFileInfo* pFieldInfo = (*itor).second;
				swprintf_s(text, 256, L"%4d = %s\n", pFieldInfo->nFieldID, XGetStr(pFieldInfo->strName));

				strText += text;
			}
			Output(strText.c_str());
		}
		else if (!_wcsicmp(argv[1], L"slot"))
		{
			wstring strText;
			for (int i = 0; i < ITEMSLOT_MAX; i++)
			{
				swprintf_s(text, 256, L"%d - %s\n", i, CSItemSlotStr[i]);
				strText += text;
			}
			Output(strText.c_str());
		}
		else if (!_wcsicmp(argv[1], L"item"))
		{
			wstring strText;

			// 이터레이팅을 못해서 그냥 이렇게 10만번까지 아이템이 있는 것들만
			for (int i = 0; i < 100000; i++)
			{
				XItemData* pItemData = info.item->GetItemData(i);
				if (pItemData)
				{
					swprintf_s(text, 256, L"%d - %s\n", i, pItemData->GetName());
					strText += text;
				}
			}
			Output(strText.c_str());
		}
		else if (!_wcsicmp(argv[1], L"party"))
		{
			wchar_t text[512]=L"";	

			wstring strText;

			swprintf_s(text, 256, L"- Party List (ID: %I64u) -\n", gvar.MyInfo.Party.GetUID()); strText += text;
			swprintf_s(text, 256, L"==============\n"); strText += text;

			for (PARTY_SET::iterator iter = gvar.MyInfo.Party.Begin();
				iter != gvar.MyInfo.Party.End();
				++iter)
			{
				XPartyMember& PartyMember = iter->second;

				bool bIsLeader = (PartyMember.m_uid == gvar.MyInfo.Party.GetLeader());

				swprintf_s(text, 256, L"- %16s(%I64u) [HP:%d][EN:%d][STA:%d][Lv:%d]%s%s%s\n", 
					PartyMember.m_strName.c_str(), 
					PartyMember.m_uid, 
					PartyMember.nHP, 
					PartyMember.nEN, 
					PartyMember.nSTA, 
					PartyMember.nLevel,
					bIsLeader?L" [Leader]":L"", 
					PartyMember.IsDead()?L" [Dead]:":L"",
					PartyMember.IsOffline()?L" [Offline]:":L""
					); 
				strText += text;
			}

			swprintf_s(text, 256, L"--------------\n"); strText += text;

			Output(strText.c_str());
		}
		else if (!_wcsicmp(argv[1], L"durability"))
		{
			XPlayer* user =
				static_cast<XPlayer*>(gg.omgr->Find(gvar.MyInfo.MyUID));

			wchar_t buff[512]=L"";

			int count = gvar.MyInfo.Inventory.GetSlotSize();
			for (int i = 0; i < count; ++i)
			{
				XItem* pItem = gvar.MyInfo.Inventory.GetItem(i);
				if (pItem == NULL) continue;
				if (0 >= pItem->m_pItemData->m_nMaxDurability) continue;

				swprintf_s(buff, L"%s : nID(%d), slotID(%d), durability(%d), max_durability(%d)\n", 
					pItem->m_pItemData->GetName(), 
					pItem->m_pItemData->m_nID,
					i,
					pItem->m_nDurability,
					pItem->m_pItemData->m_nMaxDurability,
					pItem->m_nAmount);

				Output(buff);
			}

			Output(L"----- equipment slot -----");
			for (int i=0; i<ITEMSLOT_MAX; i++)
			{
				XItem* nowItem = gvar.MyInfo.EquipmentSlot.GetItem((SH_ITEM_SLOT)i);
				if (nowItem == NULL) continue;
				if (0 >= nowItem->m_pItemData->m_nMaxDurability) continue;

				swprintf_s(buff, L"%s : nID(%d), slotID(%d), durability(%d), max_durability(%d)\n", 
					nowItem->m_pItemData->GetName(), 
					nowItem->m_pItemData->m_nID,			
					i,
					nowItem->m_nDurability,
					nowItem->m_pItemData->m_nMaxDurability,
					nowItem->m_nAmount);

				Output(buff);
			}
		}
		else if (!_wcsicmp(argv[1], L"talent"))
		{
			wchar_t text[256]=L"";	

			wstring strText;

			swprintf_s(text, 256, L"TALENT LIST\n"); strText += text;
			swprintf_s(text, 256, L"===========\n"); strText += text;

			deque<XTalentInfo*> qTestDisplayItems;
			vector<int> vecAllTalentID;
			gvar.MyInfo.Talent.GetAllTalentID(vecAllTalentID);
			for each (int nTalentID in vecAllTalentID)
			{
				XTalentInfo* pInfo = info.talent->Get(nTalentID);
				if (pInfo == NULL)	return true;

				if( pInfo->m_nTalentLine == 0)
				{
					qTestDisplayItems.push_back(pInfo);
					continue;
				}

				bool bHasLine = false;
				for (size_t i=0; i<qTestDisplayItems.size(); i++)
				{
					XTalentInfo* pInfo2 = qTestDisplayItems[i];
					if (pInfo->m_nTalentLine == pInfo2->m_nTalentLine)
					{
						bHasLine = true;
						if (pInfo->m_nRank >= pInfo2->m_nRank)
						{
							qTestDisplayItems[i] = pInfo;
							break;
						}
					}
				}

				if (!bHasLine)
					qTestDisplayItems.push_back(pInfo);
			}

			for (size_t i=0; i<qTestDisplayItems.size(); i++)
			{
				XTalentInfo* pInfo = qTestDisplayItems[i];

				swprintf_s(text, 256, L"%s 랭크.%d(%d)\n", pInfo->GetName(), pInfo->m_nRank, pInfo->m_nID);

				strText += text;
			}


			Output(strText.c_str());
		}
	}
	return true;
}

bool XConsoleCmds::ConsoleCmd_Show(const wchar_t* line, const int argc, wchar_t **const argv)
{
	wchar_t text[256]=L"";

	if ( argc < 2  ||  argc > 3)		return true;

	if (!_wcsicmp(argv[1], L"col"))
	{
		gvar.Debug.bShowCollision = !gvar.Debug.bShowCollision;
	}
	else if (!_wcsicmp(argv[1], L"aabb"))
	{
		gvar.Debug.bShowAABB = !gvar.Debug.bShowAABB;
	}
	else if (!_wcsicmp(argv[1], L"sector"))
	{
		gvar.Debug.bShowSector = !gvar.Debug.bShowSector;
	}
	else if (!_wcsicmp(argv[1], L"sensor"))
	{
		gvar.Debug.bShowSensor = !gvar.Debug.bShowSensor;
	}
	else if (!_wcsicmp(argv[1], L"area"))
	{
		gvar.Debug.bShowArea = !gvar.Debug.bShowArea;
	}
	else if (!_wcsicmp(argv[1], L"id"))
	{
		gvar.Debug.bShowID = !gvar.Debug.bShowID;
	}
	else if (!_wcsicmp(argv[1], L"sector2"))
	{
		gvar.Debug.bShowSector2 = !gvar.Debug.bShowSector2;
	}
	else if (!_wcsicmp(argv[1], L"scrlog"))
	{
		XScreenDebugger::GetInstance()->Show(!XScreenDebugger::GetInstance()->IsVisible());
	}
	else if (!_wcsicmp(argv[1], L"ui"))
	{
/*		static bool bShowUI = XCONST::SHOW_UI;
		bShowUI = !bShowUI;

		if (bShowUI)
		{
			XPostLocal_ReloadUI();
		}
		else
		{
			global.ui->ShowGameUI(bShowUI);
		}
*/
		global.ui->ToggleShowGameUI();
	}
	else if (!_wcsicmp(argv[1], L"widget"))
	{
		MWidget* pWidget = global.mint->FindWidget(MLocale::ConvUTF16ToAnsi(argv[ 2]));
		if ( pWidget != NULL)
			pWidget->Show( !pWidget->GetShow());
	}
	else if(!_wcsicmp(argv[1], L"hit"))
	{
		gvar.Debug.bShowHitCapsule = !gvar.Debug.bShowHitCapsule;
	}
	else if(!_wcsicmp(argv[1], L"hiteffect"))
	{
		gvar.Debug.bShowHitEffectCapsule = !gvar.Debug.bShowHitEffectCapsule;
	}
	else if(!_wcsicmp(argv[1], L"sound"))
	{
		gvar.Debug.bShowSoundRange = !gvar.Debug.bShowSoundRange;
	}
	else if(!_wcsicmp(argv[1], L"movecolreal"))
	{
		gvar.Debug.bShowMoveCollision_Real = !gvar.Debug.bShowMoveCollision_Real;
	}
	else if(!_wcsicmp(argv[1], L"movecolpoly"))
	{
		gvar.Debug.bShowMoveCollision_Poly = !gvar.Debug.bShowMoveCollision_Poly;
	}
	else if(!_wcsicmp(argv[1], L"movecoldown"))
	{
		gvar.Debug.bShowmovecollision_Down = !gvar.Debug.bShowmovecollision_Down;
	}
	else if(!_wcsicmp(argv[1], L"movecolview"))
	{
		gvar.Debug.bShowMoveCollision_View = !gvar.Debug.bShowMoveCollision_View;
	}
	else if(!_wcsicmp(argv[1], L"movecol"))
	{
		gvar.Debug.bShowMoveCollision = !gvar.Debug.bShowMoveCollision;
		if(gvar.Debug.bShowMoveCollision)
			gvar.Debug.nShowMoveCollision_InvestigateIndex = 0;
	}
	else if(!_wcsicmp(argv[1], L"movecolcheck"))
	{
		gvar.Debug.bShowmovecollision_Investigate = !gvar.Debug.bShowmovecollision_Investigate;
	}
	else if(!_wcsicmp(argv[1], L"movecolcheckup"))
	{
		if(argc == 3)
		{
			int nJump = _wtoi( argv[ 2]);

			if(nJump > 0)
				gvar.Debug.nShowMoveCollision_InvestigateIndex += nJump;
		}
		else
			gvar.Debug.nShowMoveCollision_InvestigateIndex++;
	}
	else if(!_wcsicmp(argv[1], L"movecolcheckdown"))
	{
		if(argc == 3)
		{
			int nJump = _wtoi( argv[ 2]);

			if(nJump > 0)
				gvar.Debug.nShowMoveCollision_InvestigateIndex -= nJump;
		}
		else
			gvar.Debug.nShowMoveCollision_InvestigateIndex--;

		if(gvar.Debug.nShowMoveCollision_InvestigateIndex < 0)
			gvar.Debug.nShowMoveCollision_InvestigateIndex = 0;
	}
	else if(!_wcsicmp(argv[1], L"objcol"))
	{
		gvar.Debug.bShowObjectCollision_Investigate = !gvar.Debug.bShowObjectCollision_Investigate;
		if(gvar.Debug.bShowObjectCollision_Investigate)
			gvar.Debug.nShowObjectCollision_InvestigateIndex = 0;
	}
	else if(!_wcsicmp(argv[1], L"objcolcheckup"))
	{
		if(argc == 3)
		{
			int nJump = _wtoi( argv[ 2]);

			if(nJump > 0)
				gvar.Debug.nShowObjectCollision_InvestigateIndex += nJump;
		}
		else
			gvar.Debug.nShowObjectCollision_InvestigateIndex++;
	}
	else if(!_wcsicmp(argv[1], L"objcolcheckdown"))
	{
		if(argc == 3)
		{
			int nJump = _wtoi( argv[ 2]);

			if(nJump > 0)
				gvar.Debug.nShowObjectCollision_InvestigateIndex -= nJump;
		}
		else
			gvar.Debug.nShowObjectCollision_InvestigateIndex--;

		if(gvar.Debug.nShowObjectCollision_InvestigateIndex < 0)
			gvar.Debug.nShowObjectCollision_InvestigateIndex = 0;
	}
	else if(!_wcsicmp(argv[1], L"magicarea"))
	{
		gvar.Debug.bShowMagicArea = !gvar.Debug.bShowMagicArea;
	}
	else if(!_wcsicmp(argv[1], L"npcmove"))
	{
		gvar.Debug.bShowNpcMoveInfo = !gvar.Debug.bShowNpcMoveInfo;
	}
	return true;
}

bool XConsoleCmds::ConsoleCmd_Enable(const wchar_t* line, const int argc, wchar_t **const argv)
{
	if ( argc == 3  &&  !_wcsicmp( argv[ 1], L"script"))
	{
		if ( !_wcsicmp( argv[ 2], L"true"))		global.mint->EnableCallScript( true);
		else									global.mint->EnableCallScript( false);
	}

	return true;
}

bool XConsoleCmds::ConsoleCmd_Rebirth(const wchar_t* line, const int argc, wchar_t **const argv)
{
	wchar_t text[256]=L"";

	if (argc != 1) return true;

	XPostGMRebirth(gvar.Game.pMyPlayer->GetPosition());

	return true;
}

bool XConsoleCmds::ConsoleCmd_Spawn(const wchar_t* line, const int argc, wchar_t **const argv)
{
	wchar_t text[256]=L"";

	if ((argc != 2) && (argc != 3) && (argc != 5)) return true;

	int nNPCID = _wtoi(argv[1]);
	if ( nNPCID == 0)
	{
		if ( wcscmp( argv[ 1], L"늑대") == 0)				nNPCID = RandomNumber(107008, 107010);
		else if ( wcscmp( argv[ 1], L"거미") == 0)			nNPCID = RandomNumber(107002, 107006);
		else if ( wcscmp( argv[ 1], L"테레시스") == 0)		nNPCID = 107007;
		else if ( wcscmp( argv[ 1], L"렌고트") == 0)			nNPCID = RandomNumber(107023, 107029);
		else if ( wcscmp( argv[ 1], L"보르고") == 0)			nNPCID = 110001;
		else if ( wcscmp( argv[ 1], L"폴루토") == 0)			nNPCID = 117000;
		else if ( wcscmp( argv[ 1], L"토나델로") == 0)		nNPCID = 105001;
		else if ( wcscmp( argv[ 1], L"소레나") == 0)			nNPCID = 105002;
		else if ( wcscmp( argv[ 1], L"베루스") == 0)			nNPCID = 106001;
		else if ( wcscmp( argv[ 1], L"봉봉") == 0)			nNPCID = RandomNumber(121006, 121008);
		else if ( wcscmp( argv[ 1], L"아루카") == 0)			nNPCID = 121009;
		else
		{
			nNPCID = info.npc->GetID(argv[1]);
		}
	}

	int nCount = 1;

	if (argc >= 3)
		nCount = _wtoi(argv[2]);

	vec3 vSpawnPos = vec3::ZERO;
	if (argc >= 5)
	{
		vSpawnPos.x = _wtof(argv[3]);
		vSpawnPos.y = _wtof(argv[4]);
	}

	XPostGM_Spawn(nNPCID, nCount, vSpawnPos);

	return true;
}

bool XConsoleCmds::ConsoleCmd_Despawn(const wchar_t* line, const int argc, wchar_t **const argv)
{	
	if (2 < argc) return true;

	float fRange = 1000.0f;
	if (2 == argc)
	{
		fRange = _wtof(argv[1]);
	}

	XPostGM_Despawn(fRange);

	return true;
}

bool XConsoleCmds::ConsoleCmd_DespawnByID(const wchar_t* line, const int argc, wchar_t **const argv)
{	
	if (2 < argc) return true;

	int nNPCID = _wtoi(argv[1]);

	float fRange = 1000.0f;
	if (3 == argc)
	{
		fRange = _wtof(argv[2]);
	}

	XPostGM_DespawnByID(nNPCID, fRange);

	return true;
}

//#ifndef _PUBLISH

bool XConsoleCmds::ConsoleCmd_ChangeMap(const wchar_t* line, const int argc, wchar_t **const argv)
{
	if (argc != 2) return true;

	int nMAPID = _wtoi(argv[1]);

	XPOSTCMD1(MC_DYNAMIC_FIELD_ENTER_REQ, MCommandParameterInt(nMAPID)); // 

	return true;
}

bool XConsoleCmds::ConsoleCmd_ChangeChannel(const wchar_t* line, const int argc, wchar_t **const argv)
{
	if (argc != 2) return true;

	int nChannelID = _wtoi(argv[1]);

	XPostChannel_ChangeChannel();

	return true;
}

bool XConsoleCmds::ConsoleCmd_ChannelList(const wchar_t* line, const int argc, wchar_t **const argv)
{
	XPostChannel_GetChannelList();

	return true;
}

//#endif

bool XConsoleCmds::ConsoleCmd_Exec(const wchar_t* line, const int argc, wchar_t **const argv)
{
	if (argc != 2) return true;

	// 루아로 실행
	global.script->RunString(argv[1]);

	return true;
}

bool XConsoleCmds::ConsoleCmd_QuestShare(const wchar_t* line, const int argc, wchar_t **const argv)
{
	if (2 != argc) return true;

	XPostQuest_RequestShare(_wtoi(argv[1]));

	return true;
}

bool XConsoleCmds::ConsoleCmd_QuestGive(const wchar_t* line, const int argc, wchar_t **const argv)
{
	if (argc != 2) return true;

	int nQuestIndex = _wtoi(argv[1]);

	XPostGMGiveQuest(nQuestIndex);

	return true;
}

bool XConsoleCmds::ConsoleCmd_QuestCancel(const wchar_t* line, const int argc, wchar_t **const argv)
{
	if (2 != argc) return true;

	XPostQuest_RequestCancel(_wtoi(argv[1]));

	return true;
}

bool XConsoleCmds::ConsoleCmd_QuestReset(const wchar_t* line, const int argc, wchar_t **const argv)
{
	XPostGM_QuestReset();

	return true;
}

bool XConsoleCmds::ConsoleCmd_QuestComplete(const wchar_t* line, const int argc, wchar_t **const argv)
{
	if (2 != argc) return true;

	XPostGM_QuestComplete(_wtoi(argv[1]));

	return true;
}

bool XConsoleCmds::ConsoleCmd_QuestReward(const wchar_t* line, const int argc, wchar_t **const argv)
{
	if (2 != argc) return true;

	XPostGM_QuestReward(_wtoi(argv[1]));

	return true;
}

bool XConsoleCmds::ConsoleCmd_QuestVar(const wchar_t* line, const int argc, wchar_t **const argv)
{
	if (2 != argc) return true;

	XPostGM_QuestVar(_wtoi(argv[1]));

	return true;
}


bool XConsoleCmds::ConsoleCmd_QuestFail(const wchar_t* line, const int argc, wchar_t **const argv)
{
	if (2 != argc) return true;

	XPostGM_QuestFail(_wtoi(argv[1]));

	return true;
}

bool XConsoleCmds::ConsoleCmd_GiveItem(const wchar_t* line, const int argc, wchar_t **const argv)
{
	if (argc != 2 && argc != 3) return true;

	int nItemID = _wtoi(argv[1]);
	if ( nItemID == 0)
	{
		XItemDataMap& itemDataMap = info.item->GetItemDataMap();
		XItemDataMap::iterator itor = itemDataMap.begin();
		for (; itor != itemDataMap.end(); ++itor)
		{
			XItemData* pItemData = static_cast<XItemData*>((*itor).second);
			const wchar_t* token = wcsstr(pItemData->GetName(), argv[ 1]);
			int result = (int)(token - pItemData->GetName());
			if (token != NULL && result >= 0)
			{
				token = wcsstr(pItemData->GetName(), L"샘플");
				if (token == NULL)
				{
					nItemID = pItemData->m_nID;
					break;
				}
			}
		}

		if(itor == itemDataMap.end())	return false;
	}

	int nQuantity = 1;

	if (argv[2]) nQuantity = _wtoi(argv[2]);

	XPostRequestGiveItem(nItemID, nQuantity);

	return true;
}



bool XConsoleCmds::ConsoleCmd_Move(const wchar_t* line, const int argc, wchar_t **const argv)
{
	if ((gg.currentgamestate->GetWorld()==NULL) || (gg.currentgamestate->GetWorld()->GetInfo()==NULL)) return true;

	int nFieldID = 0;
	float fStartingZ = 25000;
	vec3 pos_to = vec3(0,0,fStartingZ);

	if (argc == 5)
	{
		nFieldID = _wtoi(argv[1]);
		pos_to.x = _wtof(argv[2]);
		pos_to.y = _wtof(argv[3]);
		pos_to.z = _wtof(argv[4]);
	}
	else if (argc == 4)
	{
		nFieldID = _wtoi(argv[1]);
		pos_to.x = _wtof(argv[2]);
		pos_to.y = _wtof(argv[3]);

		if (nFieldID == gg.currentgamestate->GetWorld()->GetInfo()->m_nFieldID)
		{
			pos_to.z = gg.currentgamestate->GetWorld()->GetGroundZ(pos_to) + 50.0f;
		}
	}
	else if (argc == 3)
	{
		nFieldID = gg.currentgamestate->GetWorld()->GetInfo()->m_nFieldID;
		pos_to.x = _wtof(argv[1]);
		pos_to.y = _wtof(argv[2]);

		pos_to.z = gg.currentgamestate->GetWorld()->GetGroundZ(pos_to) + 50.0f;
	}
	else
	{
		OutputSyntaxError();
		return true;
	}

	XPostGM_Move(nFieldID, pos_to);

	return true;
}

bool XConsoleCmds::ConsoleCmd_MoveToMarker(const wchar_t* line, const int argc, wchar_t **const argv)
{
	if ((gg.currentgamestate->GetWorld()==NULL) || (gg.currentgamestate->GetWorld()->GetInfo()==NULL)) return true;
	
	int nFieldID = 0;
	MARKER_INFO Marker;

	if (argc == 2) 
	{
		nFieldID = gg.currentgamestate->GetWorld()->GetInfo()->m_nFieldID;

		MARKER_INFO_MAP_BY_ID& MarkerIDMap = gg.currentgamestate->GetWorld()->GetInfo()->m_mapMarkerByID;
		MARKER_INFO_MAP_BY_ID::iterator iter = MarkerIDMap.find(_wtoi(argv[1]));

		if (iter == MarkerIDMap.end())
		{
			Output(L"잘못된 마커 아이디.");
			return true;
		}
		Marker = iter->second;
	}
	else if (argc == 3) 
	{
		nFieldID = _wtoi(argv[1]);

		XFieldInfo* pFieldInfo = info.field->NewFieldInfo(nFieldID);
		if (pFieldInfo == NULL)
		{
			wchar_t text[512]=L"";
			swprintf_s(text, L"cannot find the field id(%d)", nFieldID);
			Output(text);
			return true;
		}

		MARKER_INFO_MAP_BY_ID& MarkerIDMap = pFieldInfo->m_mapMarkerByID;
		MARKER_INFO_MAP_BY_ID::iterator iter = MarkerIDMap.find(_wtoi(argv[2]));

		if (iter == MarkerIDMap.end())
		{
			Output(L"잘못된 마커 아이디.");
			return true;
		}
		Marker = iter->second;

		delete pFieldInfo;
	}
	else
	{
		OutputSyntaxError();
		return true;
	}

	XPostGM_Move(nFieldID, Marker.vPoint);

	return true;
}

bool XConsoleCmds::ConsoleCmd_MoveToSensor(const wchar_t* line, const int argc, wchar_t **const argv)
{
	if ((gg.currentgamestate->GetWorld()==NULL) || (gg.currentgamestate->GetWorld()->GetInfo()==NULL)) return true;

	int nFieldID = 0;
	vec3 pos;

	if (argc == 2)
	{
		wchar_t* szArg = argv[1];

		SENSOR_INFO Sensor;
		SENSOR_INFO_MAP_BY_ID& SensorIDMap = gg.currentgamestate->GetWorld()->GetInfo()->m_mapSensorByID;
		SENSOR_INFO_MAP_BY_ID::iterator iter = SensorIDMap.find(_wtoi(szArg));

		if (iter == SensorIDMap.end())
		{
			Output(L"잘못된 센서 아이디.");
			return true;
		}

		Sensor = iter->second;

		nFieldID = gg.currentgamestate->GetWorld()->GetInfo()->m_nFieldID;
		pos = Sensor.vPoint;
	}
	else
	{
		return true;
	}

	XPostGM_Move(nFieldID, pos);

	return true;
}

bool XConsoleCmds::ConsoleCmd_MoveToPlayer(const wchar_t* line, const int argc, wchar_t **const argv)
{
	if ((gg.currentgamestate->GetWorld()==NULL) || (gg.currentgamestate->GetWorld()->GetInfo()==NULL)) return true;

	if (argc != 2) return true;

	XPostGM_MoveToPlayer(argv[1]);

	return true;
}

bool XConsoleCmds::ConsoleCmd_MoveToNPC(const wchar_t* line, const int argc, wchar_t **const argv)
{
	if ((gg.currentgamestate->GetWorld()==NULL) || (gg.currentgamestate->GetWorld()->GetInfo()==NULL)) return false;
	if (argc < 2) return false;

	int nNPCID = _wtoi(argv[1]);
	if ( nNPCID == 0)
	{
		nNPCID = info.npc->GetID(argv[1]);
	}

	int nFieldID = 0;
	if (argc == 3)
	{
		nFieldID = _wtoi(argv[2]);
	}
	
	XPostGM_MoveToNPC(nNPCID, nFieldID);

	return true;
}

bool XConsoleCmds::ConsoleCmd_Summon(const wchar_t* line, const int argc, wchar_t **const argv)
{
	if ((gg.currentgamestate->GetWorld()==NULL) || (gg.currentgamestate->GetWorld()->GetInfo()==NULL)) return true;

	if (argc != 2) return true;

	XPostGM_Summon(argv[1]);

	return true;
}

bool XConsoleCmds::ConsoleCmd_SensorSelect(const wchar_t* line, const int argc, wchar_t **const argv)
{
	if ((gg.currentgamestate->GetWorld()==NULL) || (gg.currentgamestate->GetWorld()->GetInfo()==NULL)) return true;

	if (argc != 2) return true;

	XPostSensorTriggerSelection(_wtoi(argv[1]));

	return true;
}

bool XConsoleCmds::ConsoleCmd_SensorInteract(const wchar_t* line, const int argc, wchar_t **const argv)
{
	if ((gg.currentgamestate->GetWorld()==NULL) || (gg.currentgamestate->GetWorld()->GetInfo()==NULL)) return true;

	if (argc != 2) return true;

	vector<uint32> vecSensorID;
	vecSensorID.push_back(_wtoi(argv[1]));
	XPostSensorInteract(vecSensorID);

	return true;
}

bool XConsoleCmds::ConsoleCmd_God(const wchar_t* line, const int argc, wchar_t **const argv)
{
	XPostGMGod();

	return true;
}

bool XConsoleCmds::ConsoleCmd_AIRunning(const wchar_t* line, const int argc, wchar_t **const argv)
{
	if ((gg.currentgamestate->GetWorld()==NULL) || (gg.currentgamestate->GetWorld()->GetInfo()==NULL)) return true;

	if (2 == argc)
	{
		MUID uidNPC = _wtoi64(argv[1]);
		XPostGMToggleNPCAI(uidNPC);
		return true;
	}

	XPostGMAIRunning();

	return true;
}

bool XConsoleCmds::ConsoleCmd_AICommand(const wchar_t* line, const int argc, wchar_t **const argv)
{
	if ((gg.currentgamestate->GetWorld()==NULL) || (gg.currentgamestate->GetWorld()->GetInfo()==NULL)) return true;

	if (4 > argc) return true;

	MUID uidNPC = _wtoi64(argv[1]);
	XPostGMAICommand(uidNPC, argv[2], argv[3]);

	return true;
}

bool XConsoleCmds::ConsoleCmd_AISetNPC(const wchar_t* line, const int argc, wchar_t **const argv)
{
	if ((gg.currentgamestate->GetWorld()==NULL) || (gg.currentgamestate->GetWorld()->GetInfo()==NULL)) return true;

	UIID nNPCUIID = 0;

	if (1 == argc) nNPCUIID = 0;
	else if (2 == argc) nNPCUIID = _wtoi(argv[1]);
	else return true;

	XNonPlayer* pNPC = gg.omgr->FindNPC_UIID(nNPCUIID);
	if (NULL != pNPC)
	{
		gvar.Debug.uidTargetNPC = pNPC->GetUID();
	}

	XPostGMAISetMonitorNPC(nNPCUIID);

	Output(L"NPC 세팅됨\n");

	if(global.ui)
		global.ui->ShowNPCMonitorFrm();

	return true;
}

bool XConsoleCmds::ConsoleCmd_DebugString(const wchar_t* line, const int argc, wchar_t **const argv)
{
	if ((gg.currentgamestate->GetWorld()==NULL) || (gg.currentgamestate->GetWorld()->GetInfo()==NULL)) return true;

	//if (4 != argc) return true;
	if (2 > argc) return true;

	int nParam = 0;
	if (3 <= argc)
	{
		nParam = _wtoi(argv[2]);
	}

	wstring strText;
	if (4 <= argc)
	{
		strText = argv[3];
	}

//	XPostDebugString(argv[1], nParam, strText.c_str());

	return true;
}

bool XConsoleCmds::ConsoleCmd_GMChangeWeather(const wchar_t* line, const int argc, wchar_t **const argv)
{
	if ((gg.currentgamestate->GetWorld()==NULL) || (gg.currentgamestate->GetWorld()->GetInfo()==NULL)) return true;

	if (2 != argc) return true;

	int weather_type = _wtoi(argv[1]);
	if (weather_type<0 || weather_type >= MAX_WEATHER_TYPE)
	{
		Output(L"Error: Wrong Type");
		return true;
	}

	XPostGMChangeWeather(weather_type);

	return true;
}

bool XConsoleCmds::ConsoleCmd_GMChangeTime(const wchar_t* line, const int argc, wchar_t **const argv)
{
	if ((gg.currentgamestate->GetWorld()==NULL) || (gg.currentgamestate->GetWorld()->GetInfo()==NULL)) return true;

	int time_type = -1;
	if (2 == argc) 
	{
		time_type = _wtoi(argv[1]);
		if (time_type<0 || time_type >= MAX_TIME_TYPE)
		{
			time_type = -1;
		}
	}

	XPostGMChangeTime(time_type);

	return true;
}

bool XConsoleCmds::ConsoleCmd_LocalChangeEnv(const wchar_t* line, const int argc, wchar_t **const argv)
{
	if ((gg.currentgamestate->GetWorld()==NULL) || (gg.currentgamestate->GetWorld()->GetInfo()==NULL)) return true;

	if (3 != argc) return true;

	int weather_type = _wtoi(argv[1]);
	if (weather_type<0 || weather_type >= MAX_WEATHER_TYPE)
	{
		Output(L"Error: Wrong Weather Type");
		return true;
	}

	int time_type = _wtoi(argv[2]);
	if (time_type<0 || time_type >= MAX_TIME_TYPE)
	{
		Output(L"Error: Wrong Time Type");
		return true;
	}

	XPOSTCMD2(MC_LOCAL_TEST_CHANGE_ENV, MCommandParameterChar(time_type), MCommandParameterChar(weather_type));

	return true;
}


bool XConsoleCmds::ConsoleCmd_GetPlayerUID(const wchar_t* line, const int argc, wchar_t **const argv)
{
	if (2 != argc) return true;

	wstring strPlayerID = argv[1];

	XPOSTCMD1(MC_GM_GET_PLAYERUID_REQ, MCommandParameterWideString(strPlayerID.c_str()));

	return true;
}

bool XConsoleCmds::ConsoleCmd_WarpToPickingPos(const wchar_t* line, const int argc, wchar_t **const argv)
{
	gvar.Debug.bWarpToPickingPos = true;

	return true;
}

bool XConsoleCmds::ConsoleCmd_DyeItem(const wchar_t* line, const int argc, wchar_t **const argv)
{
	if (3 != argc) return true;

	SH_ITEM_SLOT nEquipSlot = static_cast<SH_ITEM_SLOT>(_wtoi(argv[1]));
	XItem* pItem =  gvar.MyInfo.EquipmentSlot.GetItem(nEquipSlot);
	if (NULL == pItem) return false;

	XPostGM_Dye(SLOTTYPE_EQUIP, pItem->m_nSlotID, _wtoi(argv[2]));

	return true;
}

bool XConsoleCmds::ConsoleCmd_ServerReload(const wchar_t* line, const int argc, wchar_t **const argv)
{
	wstring strParam;
	int nParam = 0;
	if (argc >= 3)
	{
		nParam = _wtoi(argv[2]);
	}

	if (argc >= 2)
	{
		strParam = argv[1];
	}

//	XPostDebugString(argv[0], nParam, strParam.c_str());

	return true;
}

bool XConsoleCmds::ConsoleCmd_ServerReport(const wchar_t* line, const int argc, wchar_t **const argv)
{
	wstring strParam;
	int nParam = -1;
	if (argc >= 3)
	{
		nParam = _wtoi(argv[2]);
	}

	if (argc >= 2)
	{
		strParam = argv[1];
	}

//	XPostDebugString(argv[0], nParam, strParam.c_str());

	return true;
}

bool XConsoleCmds::ConsoleCmd_Repair(const wchar_t* line, const int argc, wchar_t **const argv)
{
//	XPostDebugString(L"repair_all", 0, L"");

	return true;
}

bool XConsoleCmds::ConsoleCmd_ClearInven(const wchar_t* line, const int argc, wchar_t **const argv)
{
	XPostGM_ClearInventory();

	return true;
}

bool XConsoleCmds::ConsoleCmd_Regen(const wchar_t* line, const int argc, wchar_t **const argv)
{
	XPostGM_Regen();

	return true;
}

bool XConsoleCmds::ConsoleCmd_ReportTargetEntity(const wchar_t* line, const int argc, wchar_t **const argv)
{
	if ((gg.currentgamestate->GetWorld()==NULL) || (gg.currentgamestate->GetWorld()->GetInfo()==NULL)) return true;
	
	if (2 != argc) return true;

	MUID uidTarget = _wtoi64(argv[1]);

	XPostGMReportTargetEntity(uidTarget);

	return true;
}





bool XConsoleCmds::ConsoleCmd_KillEntity(const wchar_t* line, const int argc, wchar_t **const argv)
{
	if (2 != argc) return true;

	XPostComm_KillEntity(_wtoi(argv[1]));

	return true;
}

bool XConsoleCmds::ConsoleCmd_RangeKillNPC(const wchar_t* line, const int argc, wchar_t **const argv)
{
	if (2 < argc) return true;

	float fRange = 1000.0f;
	if (2 == argc)
	{
		fRange = _wtof(argv[1]);
	}

	XPostComm_RangeKillEntity(fRange);

	return true;
}

bool XConsoleCmds::ConsoleCmd_KillMe(const wchar_t* line, const int argc, wchar_t **const argv)
{
	XPostComm_KillEntity(XGetMyUIID());

	return true;
}

bool XConsoleCmds::ConsoleCmd_LocalSpawn(const wchar_t* line, const int argc, wchar_t **const argv)
{
	if ((gg.currentgamestate->GetWorld()==NULL) || (gg.currentgamestate->GetWorld()->GetInfo()==NULL)) return true;

	wchar_t text[256]=L"";

	if ((argc > 2)) return true;

	int nNPCID = _wtoi(argv[1]);

	gvar.Debug.uidTargetNPC = global.system->GenerateLocalUID();

	TD_UPDATE_CACHE_NPC	npc_info;
	npc_info.uid = gvar.Debug.uidTargetNPC;
	npc_info.nNPCID = nNPCID;
	npc_info.vPos = gvar.Game.pMyPlayer->GetPosition() + gvar.Game.pMyPlayer->GetDirection()*100.f;
	npc_info.svDir = -gvar.Game.pMyPlayer->GetDirection();
	npc_info.nLevel = 1;
	npc_info.vTarPos = npc_info.vPos;
	npc_info.nSpeed = 0;
	npc_info.nStatusFlag = 0;

	gg.currentgamestate->InNPCs(&npc_info, 1);

	return true;
}



bool XConsoleCmds::ConsoleCmd_SetMe(const wchar_t* line, const int argc, wchar_t **const argv)
{
	if ((gg.currentgamestate->GetWorld()==NULL) || (gg.currentgamestate->GetWorld()->GetInfo()==NULL)) return true;

	if (3 != argc) return true;

	XPostGMSetMe(argv[1], argv[2]);

	return true;
}

bool XConsoleCmds::ConsoleCmd_SetNPC(const wchar_t* line, const int argc, wchar_t **const argv)
{
	if ((gg.currentgamestate->GetWorld()==NULL) || (gg.currentgamestate->GetWorld()->GetInfo()==NULL)) return true;

	if (3 != argc) return true;

	XPostGMSetNPC(argv[1], argv[2]);

	return true;
}

bool XConsoleCmds::ConsoleCmd_Set(const wchar_t* line, const int argc, wchar_t **const argv)
{
	if (3 != argc) return true;

	if (!wcsicmp(argv[1], L"fov"))
	{
		int nFOV = _wtoi(argv[2]);
		if (nFOV != 0)
		{
			XCONST::CAMERA_FOV = nFOV;
			if (global.camera) global.camera->ResetFOV();
			return true;
		}
	}

	return true;
}


bool XConsoleCmds::ConsoleCmd_SetItem(const wchar_t* line, const int argc, wchar_t **const argv)
{
	if ((gg.currentgamestate->GetWorld()==NULL) || (gg.currentgamestate->GetWorld()->GetInfo()==NULL)) return true;

	if (4 != argc) return true;

	XPostGMSetItem(argv[1], _wtoi(argv[2]), _wtoi(argv[3]));

	return true;
}

bool XConsoleCmds::ConsoleCmd_GuildCreate(const wchar_t* line, const int argc, wchar_t **const argv)
{
	if ((gg.currentgamestate->GetWorld()==NULL) || (gg.currentgamestate->GetWorld()->GetInfo()==NULL)) return true;

	if (2 != argc) return true;

	XPostGuild_Create(argv[1]);

	return true;
}

bool XConsoleCmds::ConsoleCmd_GuildInvite_Req(const wchar_t* line, const int argc, wchar_t **const argv)
{
	if ((gg.currentgamestate->GetWorld()==NULL) || (gg.currentgamestate->GetWorld()->GetInfo()==NULL)) return true;

	if (2 != argc) return true;

	XPlayer* pPlayer = gg.omgr->FindPlayer(argv[1]);
	if (NULL == pPlayer) return true;

	XPostGuild_Invite_Req(pPlayer->GetUID());

	return true;
}

bool XConsoleCmds::ConsoleCmd_GuildInvite_Res(const wchar_t* line, const int argc, wchar_t **const argv)
{
	if ((gg.currentgamestate->GetWorld()==NULL) || (gg.currentgamestate->GetWorld()->GetInfo()==NULL)) return true;

	if (2 != argc) return true;

	int nAccept = _wtoi(argv[1]);
	bool bAccept = (1 == nAccept)? true : false;

	XPostGuild_Invite_Res(bAccept);

	return true;
}

bool XConsoleCmds::ConsoleCmd_GuildLeave(const wchar_t* line, const int argc, wchar_t **const argv)
{
	if ((gg.currentgamestate->GetWorld()==NULL) || (gg.currentgamestate->GetWorld()->GetInfo()==NULL)) return true;

	XPostGuild_Leave();

	return true;
}

bool XConsoleCmds::ConsoleCmd_GuildDestroy(const wchar_t* line, const int argc, wchar_t **const argv)
{
	if ((gg.currentgamestate->GetWorld()==NULL) || (gg.currentgamestate->GetWorld()->GetInfo()==NULL)) return true;

	XPostGuild_Destroy();

	return true;
}

bool XConsoleCmds::ConsoleCmd_GuildSay(const wchar_t* line, const int argc, wchar_t **const argv)
{
	if ((gg.currentgamestate->GetWorld()==NULL) || (gg.currentgamestate->GetWorld()->GetInfo()==NULL)) return true;

	if (2 != argc) return true;

	XPostMsg_Guild(argv[1]);

	return true;
}

bool XConsoleCmds::ConsoleCmd_GuildChangeMaster(const wchar_t* line, const int argc, wchar_t **const argv)
{
	if ((gg.currentgamestate->GetWorld()==NULL) || (gg.currentgamestate->GetWorld()->GetInfo()==NULL)) return true;

	if (2 != argc) return true;

	XPostGuild_ChangeMaster(argv[1]);

	return true;
}

bool XConsoleCmds::ConsoleCmd_EngineDebug(const wchar_t* line, const int argc, wchar_t **const argv)
{
	XEngineDebugger* pEngineDebugger = XEngineDebugger::GetInstancePtr();
	if (pEngineDebugger)
	{
		pEngineDebugger->OnConsoleCmd(line, argc, argv);
	}
	else if ( argc == 2 )
	{
		XEngineDebugger::OnConsoleCmd( argv[1] );
	}

	return true;
}

bool XConsoleCmds::ConsoleCmd_GMInsertBuff(const wchar_t* line, const int argc, wchar_t **const argv)
{
	if ((gg.currentgamestate->GetWorld()==NULL) || (gg.currentgamestate->GetWorld()->GetInfo()==NULL)) return true;

	if (4 != argc) return true;

	XPostGM_InsertBuff(_wtoi(argv[1]), _wtof(argv[2]), _wtof(argv[3]));

	return true;
}

bool XConsoleCmds::ConsoleCmd_GMDeleteBuff(const wchar_t* line, const int argc, wchar_t **const argv)
{
	if ((gg.currentgamestate->GetWorld()==NULL) || (gg.currentgamestate->GetWorld()->GetInfo()==NULL)) return true;

	if (2 != argc) return true;

	XPostGM_DeleteBuff(_wtoi(argv[1]));

	return true;
}

bool XConsoleCmds::ConsoleCmd_ResetCoolTime(const wchar_t* line, const int argc, wchar_t **const argv)
{
	if ((gg.currentgamestate->GetWorld()==NULL) || (gg.currentgamestate->GetWorld()->GetInfo()==NULL)) return true;

	XPostGM_ResetCoolTime();

	gvar.MyInfo.Talent.CoolTime.Clear();

	return true;
}


bool XConsoleCmds::ConsoleCmd_ShowEnemyUID(const wchar_t* line, const int argc, wchar_t **const argv)
{
	if ((gg.currentgamestate->GetWorld()==NULL) || (gg.currentgamestate->GetWorld()->GetInfo()==NULL)) return true;

	XPostGM_ShowEnemyUID();

	return true;
}

bool XConsoleCmds::ConsoleCmd_Ghost(const wchar_t* line, const int argc, wchar_t **const argv)
{
	XPostGMGhost();

	return true;
}

bool XConsoleCmds::ConsoleCmd_GameGuardEnable(const wchar_t* line, const int argc, wchar_t **const argv)
{
	if (2 != argc) return true;

	wstring strEnable = argv[1];
	bool bEnable = true;
	if (strEnable == L"off")
		bEnable = false;

	XPostGM_GameGuardEnable(bEnable);

	return true;
}

bool XConsoleCmds::ConsoleCmd_InvenSort(const wchar_t* line, const int argc, wchar_t **const argv)
{
	XPostItem_SortInvenSlot();

	return true;
}

bool XConsoleCmds::ConsoleCmd_Arena(const wchar_t* line, const int argc, wchar_t **const argv)
{
	if (argc <= 2)
	{
		std::wstring strSubCmd = argv[1];
		if (strSubCmd == L"register")
		{
			XPostBattleArena_Register();
		}
		else if (strSubCmd == L"deregister")
		{
			XPostBattleArena_Deregister();
		}
	}

	return true;
}

bool XConsoleCmds::ConsoleCmd_LuaLog(const wchar_t* line, const int argc, wchar_t **const argv)
{
	XLuaLogFrm* pLLF = global.ui->GetLuaLog();
	if (!pLLF) return true;

	if (argc == 1)
	{
		if(pLLF->GetShow())
			pLLF->Show(false);
		else
		{
			pLLF->Show(true);
		}
	}

	return true;
}

bool XConsoleCmds::ConsoleCmd_LogCRTInsert(const wchar_t* line, const int argc, wchar_t **const argv)
{
	if ((gg.currentgamestate->GetWorld()==NULL) || (gg.currentgamestate->GetWorld()->GetInfo()==NULL)) return true;

	if (2 != argc) return true;

//	XPostGM_LogCRTInsert(_wtoi(argv[1]));

	return true;
}

bool XConsoleCmds::ConsoleCmd_LogCRTDelete(const wchar_t* line, const int argc, wchar_t **const argv)
{
	if ((gg.currentgamestate->GetWorld()==NULL) || (gg.currentgamestate->GetWorld()->GetInfo()==NULL)) return true;

	if (2 != argc) return true;

//	XPostGM_LogCRTDelete(_wtoi(argv[1]));

	return true;
}

bool XConsoleCmds::ConsoleCmd_GMRangeBreakPart(const wchar_t* line, const int argc, wchar_t **const argv)
{
	if ((gg.currentgamestate->GetWorld()==NULL) || (gg.currentgamestate->GetWorld()->GetInfo()==NULL)) return true;

	if (2 < argc) return true;

	float fRange = 1000.0f;
	if (2 == argc)
	{
		fRange = _wtof(argv[1]);
	}	

	XPostGM_RangeBreakPart(fRange);

	return true;
}

bool XConsoleCmds::ConsoleCmd_MLog(const wchar_t* line, const int argc, wchar_t **const argv)
{
	XMLogFrm* pCLF = global.ui->GetMLog();
	if (!pCLF) return true;

	if (argc == 1)
	{
		if(pCLF->GetShow())
			pCLF->Show(false);
		else
		{
			pCLF->Show(true);
		}
	}

	return true;
}

bool XConsoleCmds::ConsoleCmd_FactionIncrease(const wchar_t* line, const int argc, wchar_t **const argv)
{
	if ((gg.currentgamestate->GetWorld()==NULL) || (gg.currentgamestate->GetWorld()->GetInfo()==NULL)) return true;

	if (3 != argc) return true;

	XPostGM_FactionIncrease(_wtoi(argv[1]), _wtoi(argv[2]));

	return true;
}


bool XConsoleCmds::ConsoleCmd_FactionDecrease(const wchar_t* line, const int argc, wchar_t **const argv)
{
	if ((gg.currentgamestate->GetWorld()==NULL) || (gg.currentgamestate->GetWorld()->GetInfo()==NULL)) return true;

	if (3 != argc) return true;

	XPostGM_FactionDecrease(_wtoi(argv[1]), _wtoi(argv[2]));

	return true;
}

bool XConsoleCmds::ConsoleCmd_ChangeServerMode(const wchar_t* line, const int argc, wchar_t **const argv)
{
	if (2 != argc) return true;

	int nServerMode = _wtoi(argv[1]);
	if (nServerMode <= SERVER_MODE_INVALID || SERVER_MODE_MAX <= nServerMode)
	{
		Output(L"Invalid ServerMode!");
		return true;
	}

	if (gvar.Network.nServerMode == nServerMode)
	{
		wchar_t szBuf[512];
		swprintf_s(szBuf, L"ServerMode Already %d!", nServerMode);
		Output(szBuf);
		return true;
	}

	XPostGM_ChangeServerMode(nServerMode);
	return true;
}

bool XConsoleCmds::ConsoleCmd_ResetExpoCharacters(const wchar_t* line, const int argc, wchar_t **const argv)
{
	XPostGM_ResetExpoCharacters();
	Output(L"Reset Expo Characters.");
	return true;
}

bool XConsoleCmds::ConsoleCmd_StartCommandProfile(const wchar_t* line, const int argc, wchar_t **const argv)
{
	XPostGM_StartCommandProfile();
	Output(L"서버의 커맨드 프로파일링을 시작하였습니다.");

	return true;
}

bool XConsoleCmds::ConsoleCmd_EndCommandProfile(const wchar_t* line, const int argc, wchar_t **const argv)
{
	XPostGM_EndCommandProfile();
	Output(L"서버의 커맨드 프로파일링을 종료하였습니다.");

	return true;
}


bool XConsoleCmds::ConsoleCmd_LocalCommandProfile(const wchar_t* line, const int argc, wchar_t **const argv)
{
	static bool bNowCommandProfiling = false;

	if (!bNowCommandProfiling)
	{
		Output(L"커맨드 프로파일링을 시작하였습니다.");
		global.net->StartCommandProfile();
	}
	else
	{
		Output(L"커맨드 프로파일링을 종료하였습니다.");
		global.net->FinishCommandProfile();
	}

	bNowCommandProfiling = !bNowCommandProfiling;

	return true;
}

bool XConsoleCmds::ConsoleCmd_ServerDump(const wchar_t* line, const int argc, wchar_t **const argv)
{
	wstring strID;

	if (2 <= argc)
	{
		strID = argv[1];
	}
	XPostGM_RequestServerDump(strID.c_str());

	return true;
}

//#endif

void XConsoleCmds::ReloadInfo()
{
	gg.controller->OnChat(MT_NOTICE, wstring(L"Client Info Reload Start"));

	// npc.xml
	if(info.npc->Reload(FILENAME_NPC, FILENAME_NPC_CHANGEMTRL, info.mesh_info))
		gg.controller->OnChat(MT_NOTICE, wstring(L"Npc.xml Reload Complete"));

	// talent.xml
	if(info.talent->Reload(FILENAME_TALENT, FILENAME_TALENT_EXT, FILENAME_TALENTHITINFO))
		gg.controller->OnChat(MT_NOTICE, wstring(L"Talent Info Reload Complete"));

	// talent_Effect_info.xml
	if(info.talent_effect->Reload(FILENAME_EFFECTINFO))
		gg.controller->OnChat(MT_NOTICE, wstring(L"Talent_Effect_info.xml Reload Complete"));

	// effect.xml
	if(info.effect_Info->Reload(FILENAME_EFFECT_INFO))
		gg.controller->OnChat(MT_NOTICE, wstring(L"Effect.xml Reload Complete"));

	// buff.xml
	if(info.buff->ReLoad(FILENAME_BUFFINFO, FILENAME_BUFF_EFFECTINFO, FILENAME_ENCHANT_BUFF_EFFECTINFO))
		gg.controller->OnChat(MT_NOTICE, wstring(L"Buff Info Reload Complete"));

	// motion.xml, motion_npc.xml, motion_pc.xml
	if(info.motion->ReLoad())
		gg.controller->OnChat(MT_NOTICE, wstring(L"Motion Info Reload Complete"));

		// dialog
	if (info.dialog_text->Reload(FILENAME_DIALOG))
		gg.controller->OnChat(MT_NOTICE, wstring(L"Dialog Info Reload Complete"));

	gg.controller->OnChat(MT_NOTICE, wstring(L"Client Info Reload End"));
}


bool XConsoleCmds::ConsoleCmd_FreeCamera( const wchar_t* line, const int argc, wchar_t **const argv)
{
	if ( argc < 2  ||  argc > 3)
		return false;

	std::wstring strSubCmd = argv[ 1];
	if ( strSubCmd == L"clear")				XGetFreeCamTracer()->Clear();
	else if ( strSubCmd == L"run")			XGetFreeCamTracer()->Run();
	else if ( strSubCmd == L"stop")			XGetFreeCamTracer()->Stop();
	else if ( strSubCmd == L"check")		XGetFreeCamTracer()->Check();
	else if ( strSubCmd == L"velocity")
	{
		if ( argc < 3)
			return false;

		int _vel = _wtoi( argv[ 2]);
		XGetFreeCamTracer()->SetValocity( _vel);
	}

	return true;
}

bool XConsoleCmds::ConsoleCmd_ShowHelp( const wchar_t* line, const int argc, wchar_t **const argv)
{
	wstring strID;

	int nHelpID = _wtoi( argv[ 1]);

	if (global.ui)
	{
		// by tripleJ 110414	_기존 Help 관련 수정
		global.ui->GetHelpMgr()->OnHelpTriggerCmd(nHelpID);
	}

	return true;
}

bool XConsoleCmds::ConsoleCmd_ResetHelp( const wchar_t* line, const int argc, wchar_t **const argv)
{
	if (global.ui)
	{
		global.ui->GetHelpMgr()->ResetForTest();
	}

	return true;
}

bool XConsoleCmds::ConsoleCmd_StartCutScene( const wchar_t* line, const int argc, wchar_t **const argv)
{
	wstring strID;

	int nCutSceneID = _wtoi( argv[ 1]);

	if (gg.game)
	{
		gg.game->OnChangeStart(GPS_CUTSCENE, nCutSceneID);
		gg.game->OnChangeEnd();
	}


	return true;
}

bool XConsoleCmds::ConsoleCmd_EndCutScene( const wchar_t* line, const int argc, wchar_t **const argv)
{
	if (gg.game)
	{
		gg.game->OnChangeStart(GPS_GAMEPLAY);
		gg.game->OnChangeEnd();
	}


	return true;
}

bool XConsoleCmds::ConsoleCmd_Find(const wchar_t* line, const int argc, wchar_t **const argv)
{
	std::wstring strType;
	std::wstring strName;

	if (argc >= 3)
	{
		strType = argv[1];
		strName = argv[2];
	}
	else if (argc == 2)
	{
		strName = argv[1];
	}

	if (strName.empty()) return false;

	const TCHAR* pName = strName.c_str();

	if (strType.empty() || strType == L"npc")
	{
		XNPCInfoMgr* pNPCInfoMgr = info.npc;

		for each (const map<int, CSNPCInfo*>::value_type& val in *pNPCInfoMgr)
		{
			CSNPCInfo* pNPCInfo = val.second;
			const TCHAR* token = _tcsstr(pNPCInfo->strName.c_str(), pName);
			int result = (int)(token - pNPCInfo->strName.c_str());
			if (token != NULL && result >= 0)
			{
				token = _tcsstr(pNPCInfo->strName.c_str(), _T("샘플"));
				if (token == NULL)
				{
					wchar_t szBuf[512];
					swprintf_s(szBuf, L"[NPC %d] : %s", pNPCInfo->nID, pNPCInfo->strName.c_str());
					Output(szBuf);
				}
			}
		}

	}
	if (strType.empty() || strType == L"item")
	{
		XItemDataMap& itemDataMap = info.item->GetItemDataMap();

		for each (const map<int, XItemData*>::value_type& val in itemDataMap)
		{
			XItemData* pItemData = val.second;
			const TCHAR* token = _tcsstr(pItemData->GetName(), pName);
			int result = (int)(token - pItemData->GetName());
			if (token != NULL && result >= 0)
			{
				token = _tcsstr(pItemData->GetName(), _T("샘플"));
				if (token == NULL)
				{
					wchar_t szBuf[512];
					swprintf_s(szBuf, L"[ITEM %d] : %s", pItemData->m_nID, pItemData->GetName());
					Output(szBuf);
				}
			}
		}

	}
	if (strType.empty() || strType == L"quest")
	{
		XQuestInfoMgr* pQuestInfoMgr = info.quest;
		vector<XQuestInfo*> vecAllQuestInfo = pQuestInfoMgr->GetAllQuestInfo();

		for (size_t i = 0; i < vecAllQuestInfo.size(); i++)
		{
			XQuestInfo* pQuestInfo = vecAllQuestInfo[i];

			const TCHAR* token = _tcsstr(pQuestInfo->GetTitle(), pName);
			int result = (int)(token - pQuestInfo->GetTitle());
			if (token != NULL && result >= 0)
			{
				token = _tcsstr(pQuestInfo->GetTitle(), _T("샘플"));
				if (token == NULL)
				{
					wchar_t szBuf[512];
					swprintf_s(szBuf, L"[QUEST %d] : %s", pQuestInfo->nID, pQuestInfo->GetTitle());
					Output(szBuf);
				}
			}
		}
	}

	if (strType == L"buff")
	{
		XBuffInfoMgr* pBuffInfoMgr = info.buff;

		for each (const map<int, CSBuffInfo*>::value_type& val in *pBuffInfoMgr)
		{
			CSBuffInfo* pBuffInfo = val.second;
			const TCHAR* token = _tcsstr(pBuffInfo->m_strName.c_str(), pName);
			int result = (int)(token - pBuffInfo->m_strName.c_str());
			if (token != NULL && result >= 0)
			{
				wchar_t szBuf[512];
				swprintf_s(szBuf, L"[BUFF %d] : %s", pBuffInfo->m_nID, pBuffInfo->m_strName.c_str());
				Output(szBuf);
			}
		}
	}

	if (strType == L"talent")
	{
		XTalentInfoMgr* pTalentInfoMgr = info.talent;

		for each (const map<int, CSTalentInfo*>::value_type& val in *pTalentInfoMgr)
		{
			XTalentInfo* pTalentInfo = (XTalentInfo*)val.second;

			const TCHAR* token = _tcsstr(pTalentInfo->GetName(), pName);
			int result = (int)(token - pTalentInfo->GetName());
			if (token != NULL && result >= 0)
			{
				wchar_t szBuf[512];
				swprintf_s(szBuf, L"[TALENT %d] : %s", pTalentInfo->m_nID, pTalentInfo->GetName());
				Output(szBuf);
			}
		}
	}

	return true;
}

bool XConsoleCmds::ConsoleCmd_Guard(const wchar_t* line, const int argc, wchar_t **const argv)
{

	//gvar.Debug.bForceGuard = !gvar.Debug.bForceGuard;

	return true;
}

bool XConsoleCmds::ConsoleCmd_Locale(const wchar_t* line, const int argc, wchar_t **const argv)
{
	// 로케일 있으면 설정
	if (1 < argc)
	{	
		XStringResMgr::GetInstance()->SetLocale(argv[1]);
	}	


	// 현재 설정된 로케일 출력		
	wstring strResult = L"Unkown locale";

	for each(const CLangNode& node in *info.lang)
	{		
		wstring strLocale = XStringResMgr::GetInstance()->GetLocale();
		if (node.strLocale == strLocale)
		{
			strResult = CSFormatString::Format(L"{0}({1}) - {2}", FSParam(node.strName, node.strCountry, node.strLocale));			
			break;
		}
	}

	Output(strResult.c_str());	

	return true;
}

bool XConsoleCmds::ConsoleCmd_BanAccount(const wchar_t* line, const int argc, wchar_t **const argv)
{
	std::wstring playername = argv[1];

//	XPostGM_Ban(&playername);

	return true;
}