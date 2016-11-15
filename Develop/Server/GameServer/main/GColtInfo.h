#pragma once

#include "CSChrInfo.h" // for NPC_MODE_MAX

#include <string>
#include <vector>

enum COLT_CHECK_TYPE
{
	AST_NONE = 0,
	AST_HP,
	AST_EN,
	AST_SP,
	AST_UNBREAKPART,
	AST_BREAKPART,
	AST_DAMAGE,
	AST_MOTIONFACTOR,
	AST_DISTANCE,
	AST_ANGLE,
	AST_MODE,
	AST_ENEMY_HP,
	AST_ENEMY_MOTIONFACTOR,
	AST_STRESS,
	AST_HIT_CAPSULE,	// NPC의 충돌 캡슐 설정
	AST_USED_TALENT,	// 사용중인 탤런트
	AST_GAINED_BUFF,	// 얻고 있는 버프
	AST_ENEMY_GAINED_BUFF,	// 얻고 있는 버프
	AST_REAR,			// 목표 뒤에 위치한 경우
	AST_FRONT,			// 목표 앞에 위치한 경우
	AST_RAGE,			// 분노치
	AST_DEAD,			// 죽었는지
	AST_COMBAT,			// 전투중인지
	AST_PEACE,			// 비전투중인지
	AST_SIZE,
};

enum COLT_ACTION_TYPE
{
	CAT_NONE = 0,
	CAT_NOTHING,
	CAT_GROUP,			// 그룹 액션 (아무일도 하지 않음)
	CAT_TALENT,			// 탤런트를 사용
	CAT_LUAFUNC,		// 루아 함수를 호출
	CAT_BREAKPART,		// 파트 파괴
	CAT_DAMAGE,
	CAT_MOTIONFACTOR,	// 모션 팩터 변화
	CAT_FLEE,			// 도망
	CAT_YELL,			// 외침 (도움요청)
	CAT_GUARD,			// 방어
	CAT_ADJUST,			// 컴뱃 기본 속성 변경
	CAT_DISTANCE,		// 타겟으로 일정 거리를 유지
	CAT_FOLLOW,			// 타겟을 쫒아간다
	CAT_MOVE,			// 현재위치와 타겟 사이의 직선에 해당하는 법선상을 이동
	CAT_CHANGE_HIT_CAPSULE,	// NPC의 충돌 캡슐 설정
	CAT_SAY,				// 대화: 말하기
	CAT_BALLOON,				// 대화: 말풍선
	CAT_SHOUT,				// 대화: 외치기
	CAT_NARRATION,			// 대화: 나레이션
	CAT_AGGRO,				// 어그로 변경
	CAT_FACETO,

	CAT_SIZE,
};


#define MAX_AI_INFO_PARAM 4
#define INVALID_AI_ACTION_COUNT	-1

struct GColtAction;

// Colt 액션에 대한 인스턴스 정보, 모든 NPC 개체마다 별도로 관리되야하는 변수들의 구조체
class GColtActionInst
{
	friend GColtActionInst;
public:
	GColtActionInst();
	GColtActionInst(const GColtAction& action);

	// 지금 사용 가능한지 여부
	bool IsEnableCycleTime() const;

	// 싸이클 타임을 사용가능하게 설정
	void SetEnableCycleTime();

	// 싸이클 타임 초기화
	void ResetCycleTime(float fDelaySecond);

	// Duration 시간이 무한한지 여부
	bool IsInfinityTime();
	
	// 사용 횟수 증가
	void IncreaseUseCount();

	// 포함한 내부 정보 갯수
	vector<GColtActionInst>& GetInnerInfos();

	// 확률 반환
	int GetRate() const;
	
	// 수정된 확률정보 설정
	void SetRate(int nRate); 

	// 지속시간 반환
	float GetDurationTime() const;

	// 사용한 횟수 반환
	int  GetUseCount() const;

	// 싸이클 타임 반환
	DWORD GetCycleTime() const;

	// DurationTime 사용여부
	bool IsModifiedDurationTime() const;

	// 스트레스 받는지 여부
	bool IsGainStress() const;

	bool operator==(const GColtActionInst& other) const;

private:
	int				m_nModifiedRate;		// GColtActionInfo::nRate를 기반으로 변경된 실제 Rate
	int				m_nCurCount;			// 해당 액션이 사용된 횟수
	DWORD			m_dwCycleTime;			// 다음에 액션까지 대기해야할 시각
	float			m_fDurationTime;		// 해당 액션이 지속되는 시간
	bool			m_bModifyDurationTime;	// DurationTime 사용여부
	bool			m_bGainStress;			// 탤런트가 실패할때 스트레스를 받을지 여부 (NPC 전용)
	vector<GColtActionInst>  m_vecInnerInfos;
};

// Colt 액션, 체크에 따라 NPC가 해야할 행동에 대해 정의
struct GColtAction
{
	COLT_ACTION_TYPE	nType;			// 액션 타입
	vector<wstring>	qParams;		// 액션 파라미터
	int				nRate;			// 선택될 확률 (고정값)
	int				nMaxCount;		// 해당 액션이 사용될 최대 사용횟수 (0 이하가 되면 NPC는 더 이상 해당 액션을 선택하지 않는다)
	float			fCycleTime;		// 다음에 액션까지 대기해야할 시간
	float			fDurationTime;	// 해당 액션이 지속되는 시간
	bool			bGainStress;	// 탤런트가 실패할때 스트레스를 받을지 여부 (NPC 전용)
	bool			bAutoRun;		// Rate에 상관없이 무조건 실행되는지 여부
	bool			bModifyDurationTime;	// DurationTime을 수정했는지 여부

	vector<GColtAction>  vecSeqActions;

	GColtAction();
	
	bool IsPickable( const GColtActionInst& inst ) const;
	bool IsExpiredCount(const GColtActionInst& inst) const;

	bool operator==(const GColtAction& other) const
	{
		return nType == other.nType && 
			qParams == other.qParams &&
			nRate == other.nRate &&
			nMaxCount == other.nMaxCount &&
			fCycleTime == other.fCycleTime &&
			fDurationTime == other.fDurationTime &&
			bGainStress == other.bGainStress &&
			bAutoRun == other.bAutoRun &&
			bModifyDurationTime == other.bModifyDurationTime;
	}
};

typedef vector<pair<bool,int> >	COLT_PARAM_DATA;


// Colt 체크
struct GColtCheck : public MTestMemPool<GColtCheck>
{
	COLT_CHECK_TYPE			nType;				// 체크 타입
	COLT_PARAM_DATA			vecParams;			// 조건 파라미터: MAX_AI_INFO_PARAM로 최대 갯수가 정해지며 first는 사용여부, second는 실제 값
	vector<GColtAction>		vecActions;			// 해당 체크의 조건이 충족되면 실행할 액션들의 집합 (rate 기반으로 하나만 선택하며 실행)
	vector<GColtAction>		vecAutoRunActions;	// 해당 체크의 조건이 충족되면 실행할 액션들의 집합 (qAction과 별개로 오토런 액션의 갯수가 몇개든 조건 없이 전부 실행)
	vector<GColtCheck*>		vecChecks;			// 중첩 체크: 현재 체크의 조건이 충족되는 상태에서 중첩되는 체크이 정의되어 있으면 현재 체크은 무시되며 중첩 체크들로 제어권이 넘어간다.
	GColtCheck*				pCheckDefault;		// 중첩 체크의 기본 체크
	int8					nPickRate;

	GColtCheck();
	~GColtCheck();

public:
	// 항상 검사를 할지 여부
	bool CheckActionConditions() const;

	// 하위 체크문이 있는지 여부
	bool HasInnerChecks() const;
	// 하위 체크문이 있는지 여부
	bool HasActions() const;
	// auto run 이 있는지 여부
	bool HasAutoActions() const;

	// 초기화
	void Clear();

	// 복제
	void Copy(GColtCheck& new_check) const;
};

// Colt 구조체
struct GColtGroupInfo
{
	struct CombatAttr
	{
		// 다운어택을 위한 탤런트 ID
		int	nDownAttackTalentID;	
		// 승리후 실행하는 탤런트 ID
		int	nVictoryTalentID;		

		CombatAttr();

		void Clear();
	};

	CombatAttr attrCombat[NPC_MODE_MAX];
	// 전투 체크리스트
	vector<GColtCheck*>	vecCombatChecks[NPC_MODE_MAX];
	// 피격시 반응 체크리스트
	vector<GColtCheck*>	vecReactionChecks;	
	// 헤이트 초기발생 체크리스트 (1회 발생)
	vector<GColtCheck*>	vecAggroChecks;
	// 헤이트 초기발생 체크리스트 (스폰후 1회 발생)
	vector<GColtCheck*>	vecPrologue;
	// 죽기 직전에 발생 (스폰후 1회 발생)
	vector<GColtCheck*>	vecEpilogue;
	// 스트레스 발생 체크리스트
	vector<GColtCheck*>	vecStressChecks;	
	// 유휴 체크리스트
	vector<GColtCheck*>	vecIdleChecks;		

	GColtCheck			checkCombatDefault[NPC_MODE_MAX];
	GColtCheck			checkIdleDefault;
	GColtCheck			checkAggroDefault;
	GColtCheck			checkPrologueDefault;
	GColtCheck			checkEpilogueDefault;

	GColtGroupInfo();
	~GColtGroupInfo();

	void Clear();

	// 복제
	void Copy(GColtGroupInfo& new_info);
private:
	void _EachClear(vector<GColtCheck*>& vecCheckinfos);
	void _EachClone( const vector<GColtCheck*>& source, vector<GColtCheck*>& dest );
	
};
