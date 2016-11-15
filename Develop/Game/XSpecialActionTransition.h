#pragma once

#include "XActionKeyDef.h"
#include "CSTalentInfo.h"
#include "XKeyTransition.h"

class XMyTalent;
class XMyPlayer;
class XSpecialActionTransChecker;

/// 특수키별 탤런트 공격 타입 전이
struct XSpecialActionTransition
{
	TALENT_NORMAL_ATTACK_TYPE	nAttackType;

	XSpecialActionTransition() : nAttackType(NA_NA) {}
	XSpecialActionTransition(TALENT_NORMAL_ATTACK_TYPE attackType) : nAttackType(attackType) {}
};

/// 특수키별 탤런트 공격 타입 전이 테이블
class XSpecialActionTransitionTable
{
	friend class XSpecialActionTransChecker;
private:
	XSpecialActionTransition		m_Table[WEAPON_MAX][SPECIAL_KEY_MAX];
public:
	XSpecialActionTransitionTable() {}
};

/// 탤런트 공격 타입 제약조건 플래그
enum NORMAL_ATTACK_CONSTRAINT_FLAG
{
	//NA_CONST_COMBO1					= 0x1,	///< 1타후 콤보 가능
	//NA_CONST_COMBO2					= 0x2,	///< 2타후 콤보 가능
	//NA_CONST_COMBO3					= 0x4,	///< 3타후 콤보 가능
	//NA_CONST_COMBO4					= 0x8,	///< 4타후 콤보 가능
	//NA_CONST_AFTER_COMBO_ENABLED	= 0x10,	///< 콤보로 전환 가능
};

/// 탤런트 공격 타입 제약조건
struct XNormalAttackTypeConstraint
{
	TALENT_EXTRA_PASSIVE_TYPE	nCheckPassiveType;
	uint32						nCheckStances;
	uint32						nFlags;

	XNormalAttackTypeConstraint() : nCheckPassiveType(TEPT_NONE), nCheckStances(0), nFlags(0)
	{

	}

	XNormalAttackTypeConstraint(TALENT_EXTRA_PASSIVE_TYPE extraPassiveType, uint32 stances, uint32 flags = 0)
		: nCheckPassiveType(extraPassiveType), nCheckStances(stances), nFlags(flags)
	{

	}

};

/// 탤런트 공격 타입 제약조건 테이블
class XNormalAttackTypeConstraintTable
{
	friend class XSpecialActionTransChecker;
private:
	XNormalAttackTypeConstraint		m_Table[WEAPON_MAX][NA_MAX];

	typedef set<TALENT_NORMAL_ATTACK_TYPE>	COMBO_ENABLED_ATTACK_SET;
	COMBO_ENABLED_ATTACK_SET		m_ComboEnabledAttackType[WEAPON_MAX][NA_MAX];
public:
	XNormalAttackTypeConstraintTable() {}
};

class XSpecialActionTransChecker
{
protected:
	XSpecialActionTransitionTable		m_TransTable;
	XNormalAttackTypeConstraintTable	m_ConstraintTable;
	XMyPlayer*							m_pPlayer;
	XMyTalent*							m_pMyTalent;
	void DefTrans(WEAPON_TYPE nWeaponType, SPECIAL_ACTION_COMMAND nSpecialAction, TALENT_NORMAL_ATTACK_TYPE normalAttack);
	void DefTrans_AllWeapons(SPECIAL_ACTION_COMMAND nSpecialAction, TALENT_NORMAL_ATTACK_TYPE attackType);
	void DefConst(WEAPON_TYPE nWeaponType, TALENT_NORMAL_ATTACK_TYPE attackType, XNormalAttackTypeConstraint& constraint);
	void DefConst_AllWeapons(TALENT_NORMAL_ATTACK_TYPE attackType, XNormalAttackTypeConstraint& constraint);

	void DefCombo(WEAPON_TYPE nWeaponType, TALENT_NORMAL_ATTACK_TYPE attackType, TALENT_NORMAL_ATTACK_TYPE enabledNextAttack);
	void DefCombo_AllWeapons(TALENT_NORMAL_ATTACK_TYPE attackType, TALENT_NORMAL_ATTACK_TYPE enabledNextAttack);
	void DefCombo_1HS(TALENT_NORMAL_ATTACK_TYPE attackType, TALENT_NORMAL_ATTACK_TYPE enabledNextAttack);
	void DefCombo_1HB(TALENT_NORMAL_ATTACK_TYPE attackType, TALENT_NORMAL_ATTACK_TYPE enabledNextAttack);
	void DefCombo_1HP(TALENT_NORMAL_ATTACK_TYPE attackType, TALENT_NORMAL_ATTACK_TYPE enabledNextAttack);
	void DefCombo_2HD(TALENT_NORMAL_ATTACK_TYPE attackType, TALENT_NORMAL_ATTACK_TYPE enabledNextAttack);
	void DefCombo_2HB(TALENT_NORMAL_ATTACK_TYPE attackType, TALENT_NORMAL_ATTACK_TYPE enabledNextAttack);
	void DefCombo_STAFF(TALENT_NORMAL_ATTACK_TYPE attackType, TALENT_NORMAL_ATTACK_TYPE enabledNextAttack);
	void DefCombo_DWP(TALENT_NORMAL_ATTACK_TYPE attackType, TALENT_NORMAL_ATTACK_TYPE enabledNextAttack);
public:
	XSpecialActionTransChecker(XMyPlayer* pMyPlayer, XMyTalent* pMyTalent);
	void Init();
	TALENT_NORMAL_ATTACK_TYPE Trans(SPECIAL_ACTION_COMMAND nSpecialActionCommand);
	bool CheckConst( TALENT_NORMAL_ATTACK_TYPE nPrevNormalAttackType, TALENT_NORMAL_ATTACK_TYPE nNextNormalAttackType);
};