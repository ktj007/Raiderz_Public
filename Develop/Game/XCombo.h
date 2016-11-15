#pragma once

class XCombo
{
private:
	int							m_nCurrentCombo;
	TALENT_NORMAL_ATTACK_TYPE	m_nNextAttackType;
	uint32						m_nReservedTime;
	bool						m_bComboReserved;
	bool						m_bEnable;

public:
	XCombo() : m_nNextAttackType(NA_NA), m_bComboReserved(false), m_nCurrentCombo(0), m_bEnable(true) {}

	void Init();
	void Reserve(TALENT_NORMAL_ATTACK_TYPE nNextAttackType);
	void ReleaseReserved();
	void OnAttack(TALENT_NORMAL_ATTACK_TYPE nAttackType);

	bool CheckReservedTime();
	bool CheckCanReserveCombo();

	TALENT_NORMAL_ATTACK_TYPE GetNextNormalAttack();

	bool IsReserved() { return m_bComboReserved; }
	TALENT_NORMAL_ATTACK_TYPE GetNextAttackType() { return m_nNextAttackType; }

	void SetEnable( bool m_bEnable);
};
