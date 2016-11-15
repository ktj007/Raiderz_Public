#include "StdAfx.h"
#include "GNullGluePlayer.h"

GNullGluePlayer* GNullGluePlayer::m_pNullObj = NULL;

GNullGluePlayer::GNullGluePlayer(void)
: GGluePlayer(NULL)
{
	 
}

GNullGluePlayer::~GNullGluePlayer(void)
{
	
}

GNullGluePlayer* GNullGluePlayer::GetNull()
{
	return m_pNullObj;
}

void GNullGluePlayer::Init()
{
	m_pNullObj = new GNullGluePlayer;
}

void GNullGluePlayer::FIni()
{
	SAFE_DELETE(m_pNullObj);
}
