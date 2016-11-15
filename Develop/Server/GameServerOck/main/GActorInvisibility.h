#pragma once

#include "GActorObserver.h"

class GEntityPlayer;
class GPercentDice;

class GActorInvisibility : private GActorObserver, public MTestMemPool<GActorInvisibility>
{
public:
	GActorInvisibility(GEntityActor* pOwner);
	~GActorInvisibility(void);
	
	// 업데이트 처리
	void OnUpdate( float fDelta );

	const vector<MUID>& GetBlinders()		{ return m_vecBlinders; }
	const vector<MUID>& GetDetectors()		{ return m_vecDetectors; }

	// 백분율 주사위 설정
	void SetDice(GPercentDice* pDice);
	// 탐지 확률 반환
	bool CheckDetectChance(GEntityActor* pActor);
	// 탐지 확률 반환
	float CalcDetectChancePercent(GEntityActor* pActor);
	// 탐지 거리 반환
	int CalcDetectDistance(GEntityActor* pActor);
	// 레벨팩터 확률 반환
	int CalcDetectLevelPercent(GEntityActor* pActor);
	// 갑옷팩터 확률 반환
	float CalcDetectArmorPercent();
	// 거리팩터 확률 반환
	float CalcDetectDistancePercent(GEntityActor* pActor);
	// 지각 범위 안에 들어왔는가?
	bool IsInSight(GEntityActor* pTarget);
private:
	// 찾기 관련 업데이트 처리
	void UpdateDetection( float fDelta );
	// 찾기 관련 처리
	void OnTryDetect();
	// 숨겨진 액터를 찾은 처리
	void OnDetected( const vector<MUID>& vecEffectees );
	// 숨겨진 액터를 못찾은 처리
	void OnBlinded( const vector<MUID>& vecEffectees );
	// 새로운 디텍터들을 찾기
	void CalcNewBlindersAndDetectors(vector<GEntityActor*>& vecBlinders, vector<GEntityActor*>& vecDetecteors);
	// 투명화 보이기
	void OnShow();
	// 투명화 숨기기
	void OnHide();


	// 버프를 얻었을때 이벤트
	virtual void OnGainBuff(int nBuffID) override;
	// 버프를 잃었을때 이벤트
	virtual void OnLostBuff(int nBuffID) override;
	

private:
	GEntityActor*	m_pOwner;
	MRegulator		m_rgrDetection;
	vector<MUID>	m_vecBlinders;
	vector<MUID>	m_vecDetectors;
	GPercentDice*	m_pDice;
	float			m_fInvisibilityPower; // 버프 특수속성값으로 전달된 투명화성능값
};
