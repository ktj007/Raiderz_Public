#pragma once

struct TargetActorInfo
{
	UIID			m_nUIID;		// 목표 상대 UIID
	int				m_nMaxHP;		// 최대 hp
	uint8			m_nHPPercent;	// hp(퍼센트)
	uint8			m_nLevel;		// npc level
	bool			m_bComabt;		// 전투중
	NPC_ICON_TYPE	m_nNPCIConType;	// 부연설명을 위해서 보내준다. 아이콘 관련기획 픽스되면 그때 수정

	unsigned long	m_time;			// 정보 받은 시간

	TargetActorInfo(const TD_TARGET_INFO& tdInfo)
	{
		this->Copy(tdInfo);
	}

	TargetActorInfo(const TD_ENEMY_INFO& tdInfo)
	{
		this->Copy(tdInfo);
	}

	void Copy(const TD_TARGET_INFO& tdInfo)
	{
		m_nUIID		= tdInfo.m_nUIID;
		m_nMaxHP	= tdInfo.m_nMaxHP;
		m_nHPPercent= tdInfo.m_nHPPercent;
		m_nLevel	= tdInfo.m_nLevel;
		m_bComabt	= tdInfo.m_bComabt;
		m_nNPCIConType = tdInfo.m_nNPCIConType;
		m_time		= timeGetTime();
	}

	void Copy(const TD_ENEMY_INFO& tdInfo)
	{
		m_nUIID		= tdInfo.nUIID;
		m_nMaxHP	= tdInfo.nMaxHP;
		m_nHPPercent= tdInfo.nHPPercent;
		m_nLevel	= tdInfo.nLevel;
		m_bComabt	= false;		//일부러 나와 전투중이면 false
		m_nNPCIConType = NIT_NONE;
		m_time		= timeGetTime();
	}
};

class XTargetActorInfo
{
public:
	UIID			nMouseSearchUIID;		// 마우스 타겟 UIID

	UIID			nSearchUIID;			// 타겟 UIID

	UIID			nTargetUIID;			// 타겟 락온 UIID
	bool			bLockOnTarget;			// 타겟 락온
	unsigned long	nLockOnTime;			// 락온 한 시간

	unsigned long	nPostReqTime;			/// 타겟정보 요청한 시간

private:
	map<UIID, TargetActorInfo*>		m_ActorMap;

	void	Add( const UIID nUIID, TD_ENEMY_INFO& tdInfo );
	void	Clear();

public:
	void	Init();
	void	Add( const UIID nUIID, TD_TARGET_INFO& tdInfo );
	bool	Find( const UIID nUIID );
	bool	Find_Time( const UIID nUIID );
	TargetActorInfo* Get( const UIID nUIID );
	void	AddEnemyList();

	bool	IsPostTime();
};
