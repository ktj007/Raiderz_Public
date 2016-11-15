#pragma once

class XMyPlayer;

class XMyActionDieTransChecker
{
protected:
	float		m_fDeadElapsedTime;

	bool CheckReservedHitByTalent(bool bReservedHitByTalent);
	bool CheckMotion( bool bIsHoldingSequence, wstring& strCurrMotion);
	bool CheckDead(bool bDead);
	bool CheckForceToDead( float fDeltaTime );

	bool _IsReady(float fDeltaTime, bool bIsDead, bool bReservedHitByTalent);
	bool _IsReadyForHitState(float fDeltaTime, bool bCheckMotion, bool bIsDead, bool bReservedHitByTalent, bool bIsHoldingSequence, wstring& strCurrMotion);
public:
	XMyActionDieTransChecker() : m_fDeadElapsedTime(0.0f) {}
	bool IsReady( float fDeltaTime, XMyPlayer* pOwner );
	bool IsReadyForHitState( float fDeltaTime, XMyPlayer* pOwner, bool bCheckMotion );
};
