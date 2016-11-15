#pragma once

class GEntitySync;
class GBuffInfo;

class GBuffObserver
{
public:
	// 버프 얻을때 효과처리
	virtual void OnGain(MUID uidUser, GEntitySync* pTarget, GBuffInfo* pBuffInfo) {}
	// 버프 잃을때 효과처리
	virtual void OnLost(MUID uidUser, GEntitySync* pTarget, GBuffInfo* pBuffInfo, bool bRoute) {}
	// 버프를 취소했을 때
	virtual void OnCancelled() {}
	// 버프 잃을때 효과처리
	virtual void OnPeriod() {}
	// 버프가 만료 됐을 때
	virtual void OnExpired(MUID uidUser, GEntitySync* pTarget, GBuffInfo* pBuffInfo) {}
	// 버프가 디스펠 됐을 때
	virtual void OnDispelled() {}
	// 버프가 중복되서 걸린 경우
	virtual void OnDuplicated(MUID uidUser, GEntitySync* pTarget, GBuffInfo* pBuffInfo) {}
	// 버프가 최대 스택된 경우
	virtual void OnMaxStacked() {}
	// Triggered 로 등록된 버프가 작용했을 때
	virtual void OnAffected() {}
	// 버프가 스택되서 걸린 경우
	virtual void OnStacked(float fDurationTime, float fPeriodTime) {}
};