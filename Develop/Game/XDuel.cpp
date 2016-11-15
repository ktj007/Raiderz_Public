#include "StdAfx.h"
#include "XDuel.h"

XDuel::XDuel(void)
{
}

XDuel::~XDuel(void)
{
}


void XDuel::DuelClear()
{
	m_vecOpponentUID.clear();
	m_vecAllyUID.clear();
}

void XDuel::SetOpponentUID( MUID OpponentUID )
{
	m_vecOpponentUID.push_back(OpponentUID);
}

void XDuel::SetAllyUID( MUID AllyUID )
{
	m_vecAllyUID.push_back(AllyUID);
}

bool XDuel::IsOpponent(XObject* pObject)
{
	return IsOpponent(pObject->GetUID());
}

bool XDuel::IsOpponent(MUID OpponentUID)
{
	for (vector<MUID>::iterator itor = m_vecOpponentUID.begin();
		itor != m_vecOpponentUID.end(); ++itor)
	{
		if ((*itor) == OpponentUID)	return true;
	}

	return false;
}

bool XDuel::IsAlly(XObject* pObject)
{
	for (vector<MUID>::iterator itor = m_vecAllyUID.begin();
		itor != m_vecAllyUID.end(); ++itor)
	{
		if ((*itor) == pObject->GetUID())	return true;
	}

	return false;
}

void XDuel::RemoveUID(vector<MUID>& vecUID, MUID OpponentUID)
{
	for (vector<MUID>::iterator itor = vecUID.begin();
		itor != vecUID.end(); ++itor)
	{
		if ((*itor) == OpponentUID)
		{
			vecUID.erase(itor);
			break;
		}
	}
}

void XDuel::RemoveOpponentUID( MUID OpponentUID )
{
	RemoveUID(m_vecOpponentUID, OpponentUID);
}
