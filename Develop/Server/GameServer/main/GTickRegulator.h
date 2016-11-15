#pragma once

// MRegulator와 유사함
// IsReady()에서 ElapsedTime을 0으로 바꾸는 대신,
// 업데이트틱을 기준으로 틱을 계산하고 나머지값을 저장한다.
class GTickRegulator
{
private:
	bool	m_bActive;
	float	m_fElapsed;
	float	m_fUpdateTick;
	int		m_nTickCounter;
public:
	GTickRegulator();
	GTickRegulator(float fUpdateTick);
	GTickRegulator& operator=(const GTickRegulator& other);

	void	SetTickTime(float fTime);
	float	GetTickTime();
	float	GetElapsedTime();
	bool	IsReady(float fDelta);
	void	Start();
	void	Reset(bool bImmediately=false);
	void	Stop();
	void	SetElapsedTime(float fElapsedTime);
	bool	IsActive();
	int		GetTickCounter();	
};