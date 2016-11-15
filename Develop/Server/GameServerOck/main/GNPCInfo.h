#ifndef _GNPCINFO_H
#define _GNPCINFO_H

#include "CSChrInfo.h"
#include "GColtInfo.h"
#include "GActorMeshInfo.h"
#include "GNPCInteractionInfo.h"
#include "GImmuneInfo.h"
#include "CSPalette.h"

class GLootInfo;
class GNPCShopInfo;

typedef vector<int> VEC_TEACHABLE_TALENT;
typedef vector<int> VEC_SELL_ITEM;



struct BPART_REWARD_INFO
{
	int nID;
	int nFlag;
	int nLootID;
	float fRate;
	USABLE_BPART_DIRECTION nDir;
	vector<int> vecPalettes;

	BPART_REWARD_INFO()
	{
		nLootID = INVALID_ID;
		nID = INVALID_ID;
		nFlag = INVALID_ID;
		fRate = 100.0f;
		nDir = UBD_NONE;

		vecPalettes.resize(PALETTESLOT_MAX, INVALID_ID);
	}

};

typedef map<int,vector<BPART_REWARD_INFO>> BPartRewardMap;

/// NPC 정보
class GNPCInfo : public CSNPCInfo, public MTestMemPool<GNPCInfo>
{
	friend class GNPCInfoMgr;
	friend class GModuleAI;
public:
	list<int>		m_Talents;			///< 사용가능한 스킬 ID	

	int				nSightRange;		///< 시야거리
	NPC_ATTACKABLE_TYPE	nInitAttackable;///< 초기 공격가능여부
	float			fStandupTime;		///< 넉다운시 일어날때까지 걸리는 시간
	float			fChaseMaxDurationTime;	///< 탤런트를 쓰기위해 추적하는 최대 시간
	float			m_fHateExpiredTime; ///< 헤이트 테이블 엔트리 갱신 제한시간
	int				m_nColtLink;	///< 연결할 COLT의 NPC ID

	// 전투 공식 관련
	float			m_fCriticalMelee;
	float			m_fCriticalRange;
	float			m_fCriticalMagic;
	float			m_fCriticalDmgAmp;			///< 치명 데미지율
	int				m_nMinDamage;
	int				m_nMaxDamage;
	float			m_fSpellPower;


	// 회전
	int				m_nLRotationID;
	int				m_nRRotationID;
	float			m_fRotationAngle;
	float			m_fRotationActAngle;

	// Auto Assist
	bool			m_bAutoAssist;
	float			m_fAutoAssistRange;
	bool			m_bAssistAfterCombat; ///< 전투 종료시 주변의 적이 있으면 다시 싸울지 여부


	vector<GNPCInfo*>	vecModes;		///< NPC 모드(일반, 하늘, 땅속 등)

	GLootInfo*		m_pLootInfo;	///< 일반 루팅 정보

	// 플래그
	bool			bRooted;		///< 가만히 서 있는지 여부
	bool			bCounterAttack; ///< 반격하는지 여부
	bool			bCombatable;	///< 전투 상태로 변하는지 여부
	bool			bTauntable;	
	bool			bGainEXP;		///< NPC를 죽이는 플레이어에게 경험치를 줄지 여부

	// 면역 정보
	immune::INFO	ImmuneInfo;

	// 콜트 정보
	GColtGroupInfo		ColtInfo;

	// MeshMeshInfo 관련
	GActorMeshInfo*	m_pMeshInfo;					///< 메쉬 정보
	wstring			m_strOpeningStatement;		///< 오프닝 대사
	GNPCInteractionInfo m_InteractionInfo;			///< 인터랙션 정보

	bool			m_bObjectThinkable;			///< Object타입일때 AI사용 여부

	uint8			m_nFactionGainID;			///< NPC를 죽였을때 얻는 팩션의 종류
	uint16			m_nFactionGainQuantity;		///< NPC를 죽였을때 얻는 팩션의 양	
	uint16			m_nFactionLossQuantity;		///< NPC를 죽였을때 잃는 팩션의 양

	bool			m_bUseScriptTimer;
	bool			m_bEnableAILod;				///< AI LOD를 사용할지 여부
	bool			m_isMajorLodNPC;			///< AI LOD 메이저 NPC 선언 - 다른 LOD 이벤트에 영향을 받지 않고 독자적으로 움직인다.

	int				m_nLuaSpawnCount;			///< 루아로 스폰할때 한필드에 최대로 스폰될수 있는수

	BPartRewardMap m_mapBPartRewards;

	float			m_fKeepCorpseTime; // 시체유지시간
	float			m_fHateModifierForNPC; // NPC에 대한 헤이트테이블 보정률
public:
	GNPCInfo();
	~GNPCInfo();

	void Clear();
	
	bool HasTalent(int nTalentID);
	bool HasFaction();
	bool HasQuestIE();
	void ModeCopyFromParent(GNPCInfo* pParentInfo);
	void InitStandupTime();
	void InitMaxChaseDurationTime();
	void Cooking();

	void CheckUsingScriptTimer();
	bool IsBoss() const;
	bool IsUsingScriptTimer() const { return m_bUseScriptTimer; }
};

#define NPC_XML_ATTR_AGGRO_FLIP				L"flip"
#define NPC_XML_ATTR_AGGRO_RANDOM			L"random"
#define NPC_XML_ATTR_AGGRO_FAR				L"far"
#define NPC_XML_ATTR_AGGRO_NEAR				L"near"
#define NPC_XML_ATTR_AGGRO_SHORT			L"short"

#endif