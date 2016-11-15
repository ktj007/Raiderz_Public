#ifndef __GTRIGGER_INFO_H__
#define __GTRIGGER_INFO_H__

#include "CSDef.h"
#include <deque>

// 패트롤 타입
enum PATROL_TYPE
{
	PT_ONCE=0,
	PT_ONCE_RETURN,
	PT_LOOP,
	PT_LOOP_BACKORDER,
	PT_SIZE,
};

static const char* PATROL_TYPE_STR[] = 
{
	"ONCE",
	"ONCE_RETURN",
	"LOOP",
	"LOOP_BACKORDER",
};

#define INVALID_MAREKR_ID 0
typedef unsigned long MARKER_ID;

typedef vector<MARKER_ID> MARKER_LIST;

struct SimpleMarker
{
	vec3			vPoint;			///< 위치
	vec3			vDir;			///< 방향

	SimpleMarker()			: vDir(0.0f, 1.0f, 0.0f), vPoint(vec3::ZERO) {}
	SimpleMarker(vec3 pt)	: vDir(0.0f, 1.0f, 0.0f), vPoint(pt) {}
};

struct MARKER_INFO : SimpleMarker
{
	unsigned long	nID;			///< ID
	string			strName;		///< 이름 (또다른 ID)
	int				nFieldID;		///< 필드 ID
	bool			bRandomDir;		///< 방향을 설정할지 여부
	
	bool			IsValid() const		{ return (nFieldID != 0); }
	bool			IsInvalid()	const	{ return !IsValid(); }
	static	MARKER_INFO	Invalid()		{ return MARKER_INFO(); }

	MARKER_INFO(): nID(0), nFieldID(0), bRandomDir(true) {}
	MARKER_INFO(vec3 pt): SimpleMarker(pt), nID(0), nFieldID(0), bRandomDir(true) {}
	MARKER_INFO(int id, int fieldID, vec3 pt): SimpleMarker(pt), nID(id), nFieldID(fieldID), bRandomDir(true) {}
};

enum GATE_TYPE
{
	TGT_NONE = -1,

	TGT_AUTO,
	TGT_MANUAL,	

	TGT_MAX,
};

enum WARP_TYPE
{
	WT_NONE,

	WT_FIELD,
	WT_INNROOM,

	WT_MAX,
};

struct SENSOR_WARP_INFO
{
	WARP_TYPE		eType;
	int				nConditionID;
	int				nFieldID;
	int				nFieldGroupID;
	GATE_TYPE		eGateType;
	int				nMarkerID;
	int				nQuestID;	
	TCHAR			szTitle[80];

	void	SetInvalid()			{ eType=WT_NONE; nFieldID=INVALID_ID; nFieldGroupID=INVALID_ID; eGateType=TGT_NONE; nMarkerID=INVALID_ID; nQuestID=INVALID_ID; }
	bool	IsValid()		const	
	{ 
		if (WT_NONE == eType) return false;

		if (WT_FIELD == eType)
		{
			return (nFieldID != INVALID_ID && nMarkerID != INVALID_ID) || (nFieldGroupID != INVALID_ID && nQuestID != INVALID_ID && eGateType != TGT_NONE);
		}

		return true;
		
	}
	bool	IsInvalid()		const	{ return !IsValid(); }

	SENSOR_WARP_INFO(): eType(WT_FIELD), nConditionID(INVALID_ID), nFieldID(INVALID_ID), nFieldGroupID(INVALID_ID), eGateType(TGT_NONE), nMarkerID(INVALID_ID), nQuestID(INVALID_ID) {}
};

struct SENSOR_INFO
{
	unsigned long	nID;			///< ID
	tstring			strName;		///< 이름 (또다른 ID)
	bool			bLocal;			///< 로컬센서 여부
	bool			bAutoParty;		///< 자동파티용인지 여부
	vec3			vPoint;			///< 위치
	float			fRadius;		///< 적용 범위
	bool			bStartingEnable;		///< 실행중인지 여부
	bool			bHasScript;				///< bHasScript
	int				nCheckpointMarkerID;	///< 동적필드에서 저장될 안전지대 마커ID
	int				nExitMarkerID;	///< 해당 센서로 진입했을때 탈출할 마커ID
	int				nTime_s;		///< 스폰 트리거에 정밀시간 조건걸기 시작
	int				nTime_e;		///< 스폰 트리거에 정밀시간 조건걸기 끝

	vector<SENSOR_WARP_INFO>	vecWarpInfos;	///< 터치이벤트시 즉시 워프시킬 트리거 목록
	SENSOR_WARP_INFO DefaultWarpInfo;

	tstring			strEffect;
	vec3			vEffectLocalPos;
	vec3			vEffectDir;
	vec3			vEffectUp;
	vec3			vEffectScale;

	SENSOR_INFO(): nID(0), bLocal(false), bAutoParty(false), bStartingEnable(true), bHasScript(false)
		, vPoint(0.0f, 0.0f, 0.0f), fRadius(0.0f), nExitMarkerID(INVALID_ID)
		, nCheckpointMarkerID(INVALID_ID), nTime_s(0), nTime_e(0)
		, vEffectLocalPos(vec3::ZERO), vEffectDir(vec3::AXISY), vEffectUp(vec3::AXISZ)
		, vEffectScale(1.f, 1.f, 1.f)
	{

	}

	bool HasInnroomWarp() const
	{
		// 여관방 워프는 단독으로 기술되고, 별도의 센서트리거로 처리 된다.
		if (1 != vecWarpInfos.size()) return false;
		if (WT_INNROOM != vecWarpInfos.front().eType) return false;

		return true;
	}

	bool IsExistAutoGate() const
	{
		if (TGT_AUTO == DefaultWarpInfo.eGateType) return true;

		for each (const SENSOR_WARP_INFO& warpInfo in vecWarpInfos)
		{
			if (TGT_AUTO == warpInfo.eGateType)	return true;
		}

		return false;
	}
};


#endif