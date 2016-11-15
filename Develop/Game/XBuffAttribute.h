#pragma once

#include "XModEffector.h"

#define MIN_MOVE_SPEED		0.1f

// class XBuffAttribute
class XBuffAttribute
{
private:
	
public:
	union
	{
		unsigned int	m_nAttribute;				// bit32 : 버프 속성 데이터
		struct
		{
			bool	bDisabledTalent			: 1;	// bit0  : 탈랜트 전부 금지 속성
			bool	bDisabledMelee			: 1;	// bit1  : Melee계 탈랜트 금지 속성
			bool	bDisabledRange			: 1;	// bit2  : Range계 탈랜트 금지 속성
			bool	bDisabledMagic			: 1;	// bit3  : Magic계 탈랜트 금지 속성
			bool	bInvincibled			: 1;	// bit4  : 무적 속성
			bool	bRooted					: 1;	// bit5  : Root 속성
			bool	bSleeped				: 1;	// bit6  : Sleep 속성
			bool	bTotem					: 1;	// bit7  : B-Parts 사용
		};
	};

	float m_fMoveSpeedRatio;						// 버프에 의한 이동 모션 속도 배율
	bool m_bMounted;

public:
	XBuffAttribute() : m_nAttribute( 0), m_fMoveSpeedRatio( 1.0f), m_bMounted(false) {}

	void SetAttribute( unsigned int nAttribute)			{ m_nAttribute = nAttribute;	}
	unsigned int GetAttribute()							{ return m_nAttribute;			}

	void ParseBuffAttr( const XBuffInfo* pBuffInfo, int nStackCount = 1);


	// 해당 탈랜트를 사용할 수 있는지 확인
	bool IsUsableTalent( XTalentInfo* pInfo);
	bool IsUsableTalent( int nTalentID);

	// 활동 가능한 상태인지 확인
	bool IsMesmerizable()			{ return ( bRooted || bSleeped);					}

	// Melee 공격이 가능한 상태인지 확인
	bool IsAttackableMelee()		{ return ( !IsMesmerizable() && !IsDisabledMelee());	}

	// Range 공격이 가능한 상태인지 확인
	bool IsAttackableRange()		{ return ( !IsMesmerizable() && !IsDisabledRange());	}

	// Magic 공격이 가능한 상태인지 확인
	bool IsAttackableMagic()		{ return ( !IsMesmerizable() && !IsDisabledMagic());	}

	// 이동 가능한지 확인
	bool IsMovable()				{ return !IsMesmerizable();								}

	// 파츠 무기 사용중인지
	bool IsTotemable()				{ return bTotem;}

	// 버프에 의한 이동 모션 속도 배율을 구함
	float GetMoveSpeedRatio()		{ return max( m_fMoveSpeedRatio, MIN_MOVE_SPEED);	}



	// 상태 여부 검사
	bool IsDisabledTalent()			{ return bDisabledTalent;							}
	bool IsDisabledMelee()			{ return ( bDisabledTalent || bDisabledMelee);		}
	bool IsDisabledRange()			{ return ( bDisabledTalent || bDisabledRange);		}
	bool IsDisabledMagic()			{ return ( bDisabledTalent || bDisabledMagic);		}
	bool IsInvincibled()			{ return bInvincibled;								}
};
