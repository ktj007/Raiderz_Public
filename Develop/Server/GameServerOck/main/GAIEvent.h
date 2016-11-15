#pragma once

// AI LOD Event
struct GAIEvent
{
	enum AI_EVENT_ID
	{
		EVENT_ID_NONE = -1,
		EVENT_ID_FIELD_ENTER_PLAYER,		// 아무도 없는 필드에 누군가 입장
		EVENT_ID_FIELD_LEAVE_PLAYER,		// 필드에서 모든 플레이어 퇴장
		EVENT_ID_SECTOR_ENTER_PLAYER,		// 아무도 없는 섹터에 누군가 입장
		EVENT_ID_SECTOR_LEAVE_PLAYER,		// 섹터에서 모든 플레이어 퇴장
		EVENT_ID_COMBAT_BEGIN,				// 전투 시작
		EVENT_ID_COMBAT_END,				// 전투 끝
		EVENT_ID_SECTOR_MOVE_BE_PLAYER,		// NPC가 인접섹터에 플레이어가 있는 섹터로 이동
		EVENT_ID_SECTOR_MOVE_EMPTY_PLAYER,	// NPC가 인접섹터에 플레이어가 없는 섹터로 이동
		EVENT_ID_SPAWN_EMPTY_FIELD,			// NPC가 아무도 없는 필드에 스폰
		EVENT_ID_SPAWN_EMPTY_SECTOR,		// NPC가 아무도 없는 섹터에 스폰
		EVENT_ID_SPAWN_PLAYER_SECTOR,		// NPC가 플레이어 인근 섹터에 스폰
		EVENT_ID_MAKE_MAJOR_LOD_NPC,		// 주요 LOD NPC 선언 - 전투 외에 LOD에 영향을 받지 않는다.

		MAX_EVENT_ID
	};

	AI_EVENT_ID eEventID;
	MUID uidField;
	int nSectorX;
	int nSectorY;
	MUID uidNPC;

	GAIEvent(): eEventID(EVENT_ID_NONE), uidField(MUID::ZERO), nSectorX(-1), nSectorY(-1), uidNPC(MUID::ZERO)	{}

	wstring GetName(void)
	{
		switch(eEventID)
		{
		case EVENT_ID_FIELD_ENTER_PLAYER:			return L"FIELD_ENTER_PLAYER";
		case EVENT_ID_FIELD_LEAVE_PLAYER:			return L"FIELD_LEAVE_PLAYER";
		case EVENT_ID_SECTOR_ENTER_PLAYER:			return L"SECTOR_ENTER_PLAYER";
		case EVENT_ID_SECTOR_LEAVE_PLAYER:			return L"SECTOR_LEAVE_PLAYER";
		case EVENT_ID_COMBAT_BEGIN:					return L"BATTLE_BEGIN";
		case EVENT_ID_COMBAT_END:					return L"BATTLE_END";
		case EVENT_ID_SECTOR_MOVE_BE_PLAYER:		return L"SECTOR_MOVE_BE_PLAYER";
		case EVENT_ID_SECTOR_MOVE_EMPTY_PLAYER:		return L"SECTOR_MOVE_EMPTY_PLAYER";
		case EVENT_ID_SPAWN_EMPTY_FIELD:			return L"SPAWN_EMPTY_FIELD";
		case EVENT_ID_SPAWN_EMPTY_SECTOR:			return L"SPAWN_EMPTY_SECTOR";
		case EVENT_ID_SPAWN_PLAYER_SECTOR:			return L"SPAWN_PLAYER_SECTOR";
		case EVENT_ID_MAKE_MAJOR_LOD_NPC:			return L"MAKE_MAJOR_LOD_NPC";
		}

		return L"EVENT_ID_NONE";
	}
};
