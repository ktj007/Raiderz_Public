#ifndef _XCONSOLE_CMDS_H
#define _XCONSOLE_CMDS_H

#include "XStrCmd.h"
#include "MConsole.h"

class XUISystem;

/// 콘솔 명령어
class XConsoleCmds
{
	friend class XConsoleCmdsModifier;

private:
	static MConsole*			m_pConsole;
	static XStrCmdManager		m_CmdManager;
	static void InitCmds();
	static void InitGMCmds();
	static void InitTestCmds();
	static void InitUnusedCmds();
	static void Output(const wchar_t* szText);
	static void OutputSyntaxError();
	static void _Input(const char* szCommand);

	static void ReloadInfo();
private:
	#define _DEF_CONSOLE_FUNC(func)		static bool func(const wchar_t* line, const int argc, wchar_t **const argv);

	// 명령어 함수 ////////////////////
	_DEF_CONSOLE_FUNC(ConsoleCmd_Help);
	_DEF_CONSOLE_FUNC(ConsoleCmd_Reload);
	_DEF_CONSOLE_FUNC(ConsoleCmd_DesignerMode);
	_DEF_CONSOLE_FUNC(ConsoleCmd_CmdLog);
	_DEF_CONSOLE_FUNC(ConsoleCmd_EquipItem);
	_DEF_CONSOLE_FUNC(ConsoleCmd_UnEquipItem);
	_DEF_CONSOLE_FUNC(ConsoleCmd_View);
	_DEF_CONSOLE_FUNC(ConsoleCmd_ReportTargetEntity);
	_DEF_CONSOLE_FUNC(ConsoleCmd_ShowEnemyUID);
	_DEF_CONSOLE_FUNC(ConsoleCmd_Ghost);
	_DEF_CONSOLE_FUNC(ConsoleCmd_GameGuardEnable);
		
	// 테스트 관련 -----------------
//#ifndef _PUBLISH
	_DEF_CONSOLE_FUNC(ConsoleCmd_Cam);
	_DEF_CONSOLE_FUNC(ConsoleCmd_Print);
	_DEF_CONSOLE_FUNC(ConsoleCmd_Show);
	_DEF_CONSOLE_FUNC(ConsoleCmd_Enable);
	_DEF_CONSOLE_FUNC(ConsoleCmd_ChangeMap);
	_DEF_CONSOLE_FUNC(ConsoleCmd_ChangeChannel);
	_DEF_CONSOLE_FUNC(ConsoleCmd_ChannelList);
	_DEF_CONSOLE_FUNC(ConsoleCmd_God);
	_DEF_CONSOLE_FUNC(ConsoleCmd_AIRunning);
	_DEF_CONSOLE_FUNC(ConsoleCmd_AICommand);
	_DEF_CONSOLE_FUNC(ConsoleCmd_AISetNPC);
	_DEF_CONSOLE_FUNC(ConsoleCmd_DebugString);
	_DEF_CONSOLE_FUNC(ConsoleCmd_DyeItem);
	_DEF_CONSOLE_FUNC(ConsoleCmd_ServerReload);	
	_DEF_CONSOLE_FUNC(ConsoleCmd_ServerReport);	
	_DEF_CONSOLE_FUNC(ConsoleCmd_Repair);	
	_DEF_CONSOLE_FUNC(ConsoleCmd_EngineDebug);	

	// 트레이닝
	_DEF_CONSOLE_FUNC(ConsoleCmd_Exec);		// 루아 함수 실행

	// 파티

			
	// 서버와 관련된 테스트 명령 ---
	_DEF_CONSOLE_FUNC(ConsoleCmd_Rebirth);
	_DEF_CONSOLE_FUNC(ConsoleCmd_Spawn);
	_DEF_CONSOLE_FUNC(ConsoleCmd_Despawn);
	_DEF_CONSOLE_FUNC(ConsoleCmd_DespawnByID);
	_DEF_CONSOLE_FUNC(ConsoleCmd_WarpToPickingPos);

	_DEF_CONSOLE_FUNC(ConsoleCmd_GiveItem);

	// 퀘스트 -----
	_DEF_CONSOLE_FUNC(ConsoleCmd_QuestShare);
	_DEF_CONSOLE_FUNC(ConsoleCmd_QuestGive);
	_DEF_CONSOLE_FUNC(ConsoleCmd_QuestCancel);
	_DEF_CONSOLE_FUNC(ConsoleCmd_QuestReset);
	_DEF_CONSOLE_FUNC(ConsoleCmd_QuestComplete);
	_DEF_CONSOLE_FUNC(ConsoleCmd_QuestReward);
	_DEF_CONSOLE_FUNC(ConsoleCmd_QuestVar);
	_DEF_CONSOLE_FUNC(ConsoleCmd_QuestFail);
	

	// 이동
	_DEF_CONSOLE_FUNC(ConsoleCmd_Move);
	_DEF_CONSOLE_FUNC(ConsoleCmd_MoveToSensor);
	_DEF_CONSOLE_FUNC(ConsoleCmd_MoveToMarker);
	_DEF_CONSOLE_FUNC(ConsoleCmd_MoveToPlayer);
	_DEF_CONSOLE_FUNC(ConsoleCmd_MoveToNPC);
	_DEF_CONSOLE_FUNC(ConsoleCmd_Summon);
	_DEF_CONSOLE_FUNC(ConsoleCmd_SensorSelect);
	_DEF_CONSOLE_FUNC(ConsoleCmd_SensorInteract);
	

	// UID 얻어 오기
	_DEF_CONSOLE_FUNC(ConsoleCmd_GetPlayerUID);
	_DEF_CONSOLE_FUNC(ConsoleCmd_GetItemUID);
	_DEF_CONSOLE_FUNC(ConsoleCmd_GetTradeItemUID);

	// 길드
	_DEF_CONSOLE_FUNC(ConsoleCmd_GuildCreate);
	_DEF_CONSOLE_FUNC(ConsoleCmd_GuildInvite_Req);
	_DEF_CONSOLE_FUNC(ConsoleCmd_GuildInvite_Res);
	_DEF_CONSOLE_FUNC(ConsoleCmd_GuildLeave);
	_DEF_CONSOLE_FUNC(ConsoleCmd_GuildDestroy);
	_DEF_CONSOLE_FUNC(ConsoleCmd_GuildSay);
	_DEF_CONSOLE_FUNC(ConsoleCmd_GuildChangeMaster);

	// 로컬 테스트 ---
	_DEF_CONSOLE_FUNC(ConsoleCmd_LocalChangeEnv);
	_DEF_CONSOLE_FUNC(ConsoleCmd_LocalSpawn);
	

	// GM
	_DEF_CONSOLE_FUNC(ConsoleCmd_KillEntity);
	_DEF_CONSOLE_FUNC(ConsoleCmd_RangeKillNPC);
	_DEF_CONSOLE_FUNC(ConsoleCmd_KillMe);
	
	_DEF_CONSOLE_FUNC(ConsoleCmd_GMChangeWeather);	
	_DEF_CONSOLE_FUNC(ConsoleCmd_GMChangeTime);
	_DEF_CONSOLE_FUNC(ConsoleCmd_SetMe);
	_DEF_CONSOLE_FUNC(ConsoleCmd_SetNPC);
	_DEF_CONSOLE_FUNC(ConsoleCmd_SetGStar);
	_DEF_CONSOLE_FUNC(ConsoleCmd_Set);
	_DEF_CONSOLE_FUNC(ConsoleCmd_SetItem);
	_DEF_CONSOLE_FUNC(ConsoleCmd_GMInsertBuff);
	_DEF_CONSOLE_FUNC(ConsoleCmd_GMDeleteBuff);
	_DEF_CONSOLE_FUNC(ConsoleCmd_ResetCoolTime);
	_DEF_CONSOLE_FUNC(ConsoleCmd_GMRangeBreakPart);
	_DEF_CONSOLE_FUNC(ConsoleCmd_InvenSort);
	_DEF_CONSOLE_FUNC(ConsoleCmd_ClearInven);
	_DEF_CONSOLE_FUNC(ConsoleCmd_Regen);

	
	// 투기장
	_DEF_CONSOLE_FUNC(ConsoleCmd_Arena);

	// 팩션
	_DEF_CONSOLE_FUNC(ConsoleCmd_FactionIncrease);
	_DEF_CONSOLE_FUNC(ConsoleCmd_FactionDecrease);

	// 서버 모드 변경
	_DEF_CONSOLE_FUNC(ConsoleCmd_ChangeServerMode);

	// Expo
	_DEF_CONSOLE_FUNC(ConsoleCmd_ResetExpoCharacters);
		
	// 로그
	_DEF_CONSOLE_FUNC(ConsoleCmd_LuaLog);
	_DEF_CONSOLE_FUNC(ConsoleCmd_LogCRTInsert);
	_DEF_CONSOLE_FUNC(ConsoleCmd_LogCRTDelete);

	_DEF_CONSOLE_FUNC(ConsoleCmd_MLog);
	_DEF_CONSOLE_FUNC(ConsoleCmd_ServerDump);

	_DEF_CONSOLE_FUNC(ConsoleCmd_StartCommandProfile);
	_DEF_CONSOLE_FUNC(ConsoleCmd_EndCommandProfile);
	_DEF_CONSOLE_FUNC(ConsoleCmd_LocalCommandProfile);


	// 프리 카메라
	_DEF_CONSOLE_FUNC(ConsoleCmd_FreeCamera);

	// 도움말
	_DEF_CONSOLE_FUNC(ConsoleCmd_ShowHelp);
	_DEF_CONSOLE_FUNC(ConsoleCmd_ResetHelp);
	_DEF_CONSOLE_FUNC(ConsoleCmd_Find);

	// 컷씬
	_DEF_CONSOLE_FUNC(ConsoleCmd_StartCutScene);
	_DEF_CONSOLE_FUNC(ConsoleCmd_EndCutScene);
	
	// 가드
	_DEF_CONSOLE_FUNC(ConsoleCmd_Guard);

	// 언어
	_DEF_CONSOLE_FUNC(ConsoleCmd_Locale);

	//Ban
	_DEF_CONSOLE_FUNC(ConsoleCmd_BanAccount);

//#endif

public:
	static void Init(MConsole* pConsole);
	static bool Input(const wchar_t* szCommand);			
};





#endif