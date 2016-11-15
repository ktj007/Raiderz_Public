#include "StdAfx.h"
#include "GPlayerSit.h"

GPlayerSit::GPlayerSit(void)
: m_bNowSit(false)
{
}

GPlayerSit::~GPlayerSit(void)
{
}

bool GPlayerSit::IsNowSit() const
{
	return m_bNowSit;
}

void GPlayerSit::Sit()
{
	m_bNowSit = true;
}

void GPlayerSit::Rise()
{
	m_bNowSit = false;
}
