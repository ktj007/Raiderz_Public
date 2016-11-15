#ifndef XPLAYER_H
#define XPLAYER_H

#include "XActor.h"

class XPlayerPVP;
class XModulePlayerData;
class XDuel;
class XNPCInfo;

// 탈것에 대한 정보 저장			_by tripleJ
class XRideInfo
{
public:
									XRideInfo()
									: m_nRideNpcID( 0 ), m_bRide( false )	{}
									~XRideInfo()							{}

			void					SetRideNpcID( int nRideNpcID )			{ m_nRideNpcID = nRideNpcID; }
			int						GetRideNpcID()							{ return m_nRideNpcID; }

			bool					IsRide()								{ return m_bRide; }
			void					SetRide( bool bRide )					{ m_bRide = bRide; }

protected:
			int						m_nRideNpcID;
			bool					m_bRide;
};

/// 플레이어 클래스
class XPlayer : public XActor
{
	MDeclareRTTI;
protected:
	XModulePlayerData*				m_pModulePlayerData;

	CHAR_STANCE						m_nStance;
	bool							m_bSwim;
	bool							m_bSitting;		// status(stance, swim, sitting)는 나중에 한곳으로 모아야 할듯. - birdkr
	bool							m_bAFK;		// status(stance, swim, sitting)는 나중에 한곳으로 모아야 할듯. - birdkr
	bool							m_bCutScene;

	XPlayerPVP*						m_pPlayerPVP;		///< 플레이어들끼리 Faction이 다르면 서로 판정이 된다.
	XDuel*							m_pDuel;			///< 일단 1:1만 파티vs파티도 추가예정

	//==  탈것 추가	_by tripleJ
	XRideInfo						m_RideInfo;		
	//==

protected:
	virtual void					RegisterModules();
	virtual void					UnregisterModules();
	virtual bool					OnCreate();

public:
	XPlayer(MUID uid, UIID nUIID);
	virtual ~XPlayer();

	//==  탈것 추가	_by tripleJ
			int						GetRideID()				{ return m_RideInfo.GetRideNpcID(); }
			// 서버버프 동기화 할때 위치이동/수정이 필요함
			void					SetRide( int nRideID );
			XRideInfo*				GetRideInfo()			{ return &m_RideInfo; }
			bool					IsRide()				{ return m_RideInfo.IsRide(); }
			void					RemoveRide();
	//==

	virtual XModulePlayerData*		GetModulePlayerData()			{ return m_pModulePlayerData; }

	virtual ARMOR_TYPE				GetArmorType()		{ return ARMOR_NONE;}		// MyPlayer, NetPlayer 각각 작성해야한다.
	virtual WEAPON_TYPE				GetCurrWeaponType() { return WEAPON_NONE;	}	// MyPlayer, NetPlayer 각각 작성해야한다.
	virtual WEAPON_TYPE				GetEquipWeaponType(bool bLeft){ return WEAPON_NONE;	}	// MyPlayer, NetPlayer 각각 작성해야한다.
	virtual bool					IsEquipShield()		{ return false;			}	// MyPlayer, NetPlayer 각각 작성해야한다.
	virtual void					GetWeaponParentName(wstring& strRWeapon, wstring& strLWeapon) {} // MyPlayer, NetPlayer 각각 작성해야한다.
	virtual void					GetCurrentWeaponSetWeaponItemData(XItemData** pItemDataR, XItemData** pItemDataL) {}

	bool							IsCurrStanceBattle();
	bool							IsSwim()							{ return m_bSwim; }
	bool							IsSitting()							{ return m_bSitting; }
	bool							IsCutScene()						{ return m_bCutScene; }
	bool							IsAFK()								{ return m_bAFK; }

	void							SetStance(CHAR_STANCE nStance)		{ m_nStance = nStance; }
	void							SetSwim(bool bSwim)					{ m_bSwim = bSwim; }
	void							SetSitting(bool bSitting)			{ m_bSitting = bSitting; }
	void							SetAFK(bool bVar)					{ m_bAFK = bVar; }
	void							SetDuel( bool bDuel, MUID OpponentUID);
	void							SetBattleArenaEnemy();
	void							SetCutScene(bool bEnterCutScene);


	CHAR_STANCE						GetStance()							{ return m_nStance; }
	float							GetMoveSpeedDefault(unsigned int nMovementFlags);	// 보정되지 않은 기본 이동속도
	float							GetMoveSpeed(unsigned int nMovementFlags);			// 보정된 이동속도

	void							ChangeStance(CHAR_STANCE nStance, bool bQuick);
	void							ChangeWeapon(CHAR_STANCE nSwitching, bool bQuick);

	virtual const wchar_t*			GetName() { return L""; }
	virtual SEX						GetSex()	{ return SEX_MALE; }
	virtual RACE					GetRace()	{ return RACE_HUMAN; }
	virtual bool					IsPlayer() { return true; }

	bool							IsAlly(XPlayer* pTargetPlayer);
	bool							IsEnemy( XPlayer* pTargetPlayer );

	//bool IsAllyInPvPArea( XPlayer* pTargetPlayer );
	bool							IsCollidableTo(XPlayer* pTarPlayer);		/// 플레이어들끼리 충돌 체크를 할지 여부

	bool							IsDuel();
	XPlayerPVP*						GetPlayerPVP() { return m_pPlayerPVP; }
	virtual const wchar_t*			GetRealNameInPvP() { return L""; }

	virtual int						GetVisualEnchantBuffID() { return 0; }

	virtual const wstring			GetCaptionName();
	virtual void					SetCharacterPane_Player()		{}
	virtual void					SetCharacterPane_VSNetPlayer()	{}
	//virtual void					SetCharacterPane_AllyNetPlayer(){}

	// 결투
	XDuel*							GetPlayerDuel() const { return m_pDuel; }
	void							RemoveOpponentUID(MUID OpponentUID);
	void							SetOpponentUIDs(const vector<MUID>& vecUIDs);

	// 브레이커블 파츠 무기화
	virtual void					EquipOverlapped(int nItemID);
	virtual void					UnEquipOverlapped(int nItemID);
	bool							IsUsableBreakablePartsWeapon();
	
};




#endif
