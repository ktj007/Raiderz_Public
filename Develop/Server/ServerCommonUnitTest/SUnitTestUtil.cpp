#include "stdafx.h"
#include "SUnitTestUtil.h"
#include "MMath.h"
#include "CSDef.h"
#include "CSNameStringChecker.h"

MUID	SUnitTestUtil::m_UID	= MUID(0x00FFFFF0, 0);
MUID	SUnitTestUtil::m_nNPCUID= MUID(234234,0);
int64	SUnitTestUtil::m_nIUID	= 0;
int		SUnitTestUtil::m_nCID	= 0;
int		SUnitTestUtil::m_nID	= 10000;
uint8	SUnitTestUtil::m_nUINT8ID = 0;
uint32	SUnitTestUtil::m_nUINT32ID = 0;
int		SUnitTestUtil::m_nNumber= 0;
int		SUnitTestUtil::m_nGID	= 0;
int		SUnitTestUtil::m_nName	= 0;
double	SUnitTestUtil::m_nGuildName = pow((double)10, (int)(CSNameStringChecker::GUILD_MIN_ENGLISH-1));
int		SUnitTestUtil::m_nString= 0;
AID		SUnitTestUtil::m_nAID= 0;


int SUnitTestUtil::NewID( int nID )
{
	if (nID != INVALID_ID)
		return nID;

	return NewID();
}
