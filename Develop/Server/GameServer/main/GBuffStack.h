#pragma once

class GBuff;

class GBuffStack : public MTestMemPool<GBuffStack>
{
public:
	GBuffStack(GBuff* pBuff);
	~GBuffStack(void);

	// 업데이트
	void Update_Expired( float fDelta );

	// 스택 추가
	void Increase(float fDurationTime);
	// 스택 소멸
	void Decrease(bool bRoute=true);
	// 소멸되는지 여부
	bool IsExpired() const;

	// 중첩갯수 반환
	int GetCount() const;

private:
	void RouteStackIncrease();
	void RouteStackDecrease();
private:
	GBuff*		m_pOwner;
	int			m_nCount;
	deque<float> m_qDurations;
};
