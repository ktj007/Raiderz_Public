#include "stdafx.h"
#include "XCalculator.h"
#include "XConst.h"

float XCalculator::CalcPlayerMoveSpeed( bool bMoveBackward, CHAR_STANCE nStance, WEAPON_TYPE nWeaponType)
{
	float fSpeed = 0.0f;

	if (!bMoveBackward)
	{
		if (nStance == CS_NORMAL) fSpeed = XCONST::MOVE_SPEED_NONE;
		else 
		{
			switch (nWeaponType)
			{
			case WEAPON_1H_SLASH:	fSpeed = XCONST::MOVE_SPEED_1HS; break;
			case WEAPON_1H_BLUNT:	fSpeed = XCONST::MOVE_SPEED_1HB; break;
			case WEAPON_1H_PIERCE:	fSpeed = XCONST::MOVE_SPEED_1HP; break;
			case WEAPON_TWO_HANDED: fSpeed = XCONST::MOVE_SPEED_2HD; break;
			case WEAPON_ARCHERY:	fSpeed = XCONST::MOVE_SPEED_ARC; break;
			case WEAPON_2H_BLUNT:	fSpeed = XCONST::MOVE_SPEED_2HB; break;
			case WEAPON_DUAL_WIELD: fSpeed = XCONST::MOVE_SPEED_DWD; break;
			case WEAPON_DUAL_PIERCE: fSpeed = XCONST::MOVE_SPEED_DWP; break;
			case WEAPON_STAFF:		fSpeed = XCONST::MOVE_SPEED_STF; break;
			default:
				{
					fSpeed = XCONST::MOVE_SPEED_1HS;
				}
			}
		}
	}
	else
	{
		// 뒤로 갈때
		if (nStance == CS_NORMAL) fSpeed = XCONST::MOVE_BACK_SPEED_NONE;
		else 
		{
			switch (nWeaponType)
			{
			case WEAPON_1H_SLASH:	fSpeed = XCONST::MOVE_BACK_SPEED_1HS; break;
			case WEAPON_1H_BLUNT:	fSpeed = XCONST::MOVE_BACK_SPEED_1HB; break;
			case WEAPON_1H_PIERCE:	fSpeed = XCONST::MOVE_BACK_SPEED_1HP; break;
			case WEAPON_TWO_HANDED: fSpeed = XCONST::MOVE_BACK_SPEED_2HD; break;
			case WEAPON_ARCHERY:	fSpeed = XCONST::MOVE_BACK_SPEED_ARC; break;
			case WEAPON_2H_BLUNT:	fSpeed = XCONST::MOVE_BACK_SPEED_2HB; break;
			case WEAPON_DUAL_WIELD: fSpeed = XCONST::MOVE_BACK_SPEED_DWD; break;
			case WEAPON_DUAL_PIERCE: fSpeed = XCONST::MOVE_BACK_SPEED_DWP; break;
			case WEAPON_STAFF:		fSpeed = XCONST::MOVE_BACK_SPEED_STF; break;
			default:
				{
					fSpeed = XCONST::MOVE_BACK_SPEED_1HS;
				}
			}
		}
	}

	return fSpeed;
}