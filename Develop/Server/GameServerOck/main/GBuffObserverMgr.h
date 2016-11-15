#pragma once

class GBuffObserver;
class GEntitySync;
class GBuffInfo;

class GBuffObserverMgr
{
public:
	GBuffObserverMgr(void);
	~GBuffObserverMgr(void);

	// 옵저버를 연결시킴
	void AttachObserver(GBuffObserver* pOvr);
	// 옵저버를 분리시킴
	void DetachObserver(GBuffObserver* pOvr);

public:
	// 버프에 Triggered 로 등록된 버프가 작용했을 때
	void OnTriggeredBuffAffected();
protected:
	// 버프 얻을때 효과처리
	void OnGain(MUID uidUser, GEntitySync* pTarget, GBuffInfo* pBuffInfo);
	// 버프 잃을때 효과처리
	void OnLost(MUID uidUser, GEntitySync* pTarget, GBuffInfo* pBuffInfo, bool bRoute);
	// 버프를 취소했을 때
	void OnCancelled();
	// 버프 잃을때 효과처리
	void OnPeriod();
	// 버프가 만료 됐을 때
	void OnExpired(MUID uidUser, GEntitySync* pTarget, GBuffInfo* pBuffInfo);
	// 버프가 디스펠 됐을 때
	void OnDispelled();
	// 버프가 중복되서 걸린 경우
	void OnDuplicated(MUID uidUser, GEntitySync* pTarget, GBuffInfo* pBuffInfo);
	// 버프가 최대 스택된 경우
	void OnMaxStacked();
	// 버프가 스택되서 걸린 경우
	void OnStacked(float fDurationTime, float fPeriodTime);
private:
	// 옵저버 집합
	vector<GBuffObserver*>	m_vecObservers;
};
