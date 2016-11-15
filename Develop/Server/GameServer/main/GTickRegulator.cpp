#include "StdAfx.h"
#include "GTickRegulator.h"

GTickRegulator::GTickRegulator() 
: m_fElapsed(0.0f)
, m_fUpdateTick(0.0f)
, m_bActive(true)
, m_nTickCounter(0)
{

}

GTickRegulator::GTickRegulator( float fUpdateTick ) 
: m_fElapsed(0.0f)
, m_fUpdateTick(fUpdateTick)
, m_bActive(true)
, m_nTickCounter(0)
{

}

GTickRegulator& GTickRegulator::operator=( const GTickRegulator& other )
{
	m_bActive = other.m_bActive;
	m_fElapsed = other.m_fElapsed;
	m_fUpdateTick = other.m_fUpdateTick;
	m_nTickCounter = other.m_nTickCounter;
	return *this;
}

bool GTickRegulator::IsReady( float fDelta )
{
	if (!m_bActive) return false;

	m_fElapsed += fDelta;
	if (m_fElapsed >= m_fUpdateTick)
	{
		m_nTickCounter = (int)(m_fElapsed/m_fUpdateTick);	// 틱카운터 넣어줌
		m_fElapsed = fmod(m_fElapsed,m_fUpdateTick);		// 남은 틱은 보존
		return true;
	}
	return false;
}

void GTickRegulator::Start()
{
	m_bActive = true;
	m_fElapsed = 0.0f;
}

void GTickRegulator::Reset( bool bImmediately/*=false*/ )
{
	if (bImmediately) m_fElapsed = m_fUpdateTick;
	else m_fElapsed = 0.0f;
}

void GTickRegulator::Stop()
{
	m_bActive = false;
}

void GTickRegulator::SetElapsedTime( float fElapsedTime )
{
	m_fElapsed = fElapsedTime;
}

bool GTickRegulator::IsActive()
{
	return m_bActive;
}

void GTickRegulator::SetTickTime( float fTime )
{
	m_fUpdateTick = fTime;
}

float GTickRegulator::GetTickTime()
{
	return m_fUpdateTick;
}

float GTickRegulator::GetElapsedTime()
{
	return m_fElapsed;
}

int GTickRegulator::GetTickCounter()
{
	int nTickCounter = m_nTickCounter;
	m_nTickCounter = 0;	// 초기화
	return nTickCounter;
}