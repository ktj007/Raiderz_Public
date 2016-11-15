#include "stdafx.h"
#include "CSMotionFactorHelper.h"


int CSMotionFactorHelper::g_nPriority[MF_PRIORITY_SIZE] = {0, };

int CSMotionFactorHelper::GetMFPriority(MF_STATE nState)
{
	_ASSERT(nState < MF_PRIORITY_SIZE);
	if (nState >= MF_PRIORITY_SIZE) 
	{
		return MF_SIZE;
	}

	if (g_nPriority[0] == 0)
	{
		for (int i = 0; i < MF_PRIORITY_SIZE; i++)
		{
			for (int j = 0; j < MF_PRIORITY_SIZE; j++)
			{
				if (i == MotionFactorPriority[j]) 
				{
					g_nPriority[i] = j+1;
					break;
				}
			}
		}
	}

	return g_nPriority[nState];
}

bool CSMotionFactorHelper::HasMotionFactor( MF_STATE nState )
{
	if (nState == MF_NONE)			return false;

	return true;
}

bool CSMotionFactorHelper::IsMovingMotionFactor( MF_STATE nState )
{
	if (nState == MF_KNOCKBACK ||
		nState == MF_THROWUP ||
		nState == MF_UPPERED ||
		nState == MF_DRAG) 
		return true;

	return false;
}

bool CSMotionFactorHelper::IsFakeMotionFactor( MF_STATE nState )
{
	switch (nState)
	{
	case MF_FAKE_BEATEN:
	case MF_FAKE_KNOCKBACK:
		{
			return true;
		}
	}
	return false;
}

bool CSMotionFactorHelper::IsNoneOrFakes( MF_STATE nState )
{
	if (nState == MF_NONE || IsFakeMotionFactor(nState)) return true;

	return false;
}

bool CSMotionFactorHelper::IsNoneOrFakeBeaten( MF_STATE nState )
{
	if (nState == MF_NONE || nState == MF_FAKE_BEATEN) return true;
	return false;
}