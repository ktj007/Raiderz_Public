#include "stdafx.h"
#include "XSoulObject.h"
#include "XSystem.h"
#include "XEffectManager.h"
#include "XCharacter.h"

MImplementRTTI( XSoulObject, XObject);


#define	NAME_SOUL_0			L"ef_soul"
#define	NAME_SOUL_1			L"ef_soul"
#define	NAME_SOUL_2			L"ef_soul"
#define	NAME_SOUL_3			L"ef_soul"
#define NAME_SOUL_SPLASH	L"em_spread_blood"
#define NAME_SOUL_ABSORB	L"em_holy_cast_em01"
#define NAME_DUMMY_AMULET	L"dummy_shield"



const wchar_t SOUL_EFFECT_NAME[ 4][ 256] = { NAME_SOUL_0, NAME_SOUL_1, NAME_SOUL_2, NAME_SOUL_3 };

const float _PI = 3.14f;
const float _2PI = 6.28f;
const float _4PI = 12.56f;
const float _6PI = 18.84f;



XSoulObject::XSoulObject(MUID uid) : XObject(uid)
{
	m_nTypeID = ETID_PROJECTILE;

	m_pNPC = NULL;

	m_uidTarget = MUID::ZERO;
	m_uidEffectSoul = MUID::ZERO;
#ifdef SHOW_SPLASH_EFFECT
	m_uidEffectSplash = MUID::ZERO;
#endif
#ifdef SHOW_ABSORB_EFFECT
	m_uidEffectAbsorb = MUID::ZERO;
#endif

	m_vPos = vec3::ZERO;
	m_nSoulType = 0;
	m_nWaitTime = 0;
	m_bWaiting = true;
	m_bArrival = false;
	m_bBackRush = false;
}


XSoulObject::~XSoulObject()
{
}


bool XSoulObject::Create( XObject* pOwnerNpc, const MUID& uidTarget, int nType, unsigned int nWaitTime)
{
	if ( pOwnerNpc == NULL  ||  uidTarget == MUID::ZERO)
	{
		mlog( "Cannot create soul object : NPC is null.\n");

		_ASSERT( 0);
		return false;
	}


	if ( XObject::Create() == false)
	{
		mlog( "Cannot create soul object : Target is null.\n");

		_ASSERT( 0);
		return false;
	}


	// Set info
	m_pNPC = pOwnerNpc;
	m_uidTarget = uidTarget;
	m_vPos = m_vStartPos = pOwnerNpc->GetPosition();
	m_dwTime = timeGetTime();
	m_fMoveSpeed = 0.0f;
	m_bWaiting = true;
	m_bArrival = false;
	m_bBackRush = false;
	m_nSoulType = max( nType , 0);
	m_nWaitTime = min( nWaitTime , 10000);


	return true;
}


void XSoulObject::OnDestroy()
{
	if ( IsDeleteMe() == false)
		DeleteMe();


	if ( m_uidEffectSoul != MUID::ZERO)
	{
		global.emgr->DeleteXEffect( m_uidEffectSoul);
		m_uidEffectSoul = MUID::ZERO;
	}

#ifdef SHOW_SPLASH_EFFECT
	if ( m_uidEffectSplash != MUID::ZERO)
	{
		global.emgr->DeleteXEffect( m_uidEffectSplash);
		m_uidEffectSplash = MUID::ZERO;
	}
#endif

#ifdef SHOW_ABSORB_EFFECT
	if ( m_uidEffectAbsorb != MUID::ZERO)
	{
		global.emgr->DeleteXEffect( m_uidEffectAbsorb);
		m_uidEffectAbsorb = MUID::ZERO;
	}
#endif
}


void XSoulObject::GetRandomFlyingFactor()
{
	float rval = (float)( (rand() % 60) + 40) * 0.01;

	static bool bMinus = false;
	if ( bMinus == true)
	{
		rval = -rval;
		bMinus = false;
	}
	else
		bMinus = true;

	m_vFlyingFactor.x = m_vFlyingFactor.y = rval * 1.6f;



	rval = (float)( (rand() % 60) + 40) * 0.01;

	m_vFlyingFactor.z = rval * 2.2f;
}


// 타켓 캐릭터와 소울 스폰 위치가 너무 가까우면 뒤로 날라간다
void XSoulObject::GetBackRushState()
{
	m_bBackRush = false;


	XObject* pObject = gg.omgr->Find( m_uidTarget);
	if ( pObject == NULL)
		return;


	vec3 diff = pObject->GetPosition() - m_vStartPos;
	m_vStartDir = Normalize( diff);


	if ( diff.Length() < 700.0f)
		m_bBackRush = true;
}


bool XSoulObject::MakeSoulUpdateInfo( SoulUpdateInfo& info)
{
	info.pTarget = gg.omgr->Find( m_uidTarget);
	if ( info.pTarget == NULL)
	{
		_ASSERT( 0);

		DeleteMe();

		return false;
	}

	info.dest_pos = info.pTarget->GetPosition();
	info.pTarget->GetActor()->GetActorNodePos(NAME_DUMMY_AMULET, info.dest_pos, E_TS_WORLD);

	info.effect_pos = m_vPos;
	info.curr_time = timeGetTime();
	info.elipsed = info.curr_time - m_dwTime;
	info.radian = (float)info.elipsed * 0.006f;

	return true;
}


void XSoulObject::OnUpdate( float fDelta)
{
	if ( IsDeleteMe())
		return;


	// Create soul info
	SoulUpdateInfo info;
	if ( MakeSoulUpdateInfo( info) == false)
		return;


	// Waiting
	if ( OnWaiting( fDelta, info) == false)
		return;


	// Update move of soul position
	if ( OnUpdateMoveSoul( fDelta, info) == false)
		return;

	
	// Update effect
	if ( OnUpdateEffect( fDelta, info) == false)
		return;


	// Update soul effect
	if ( m_uidEffectSoul != MUID::ZERO)
	{
		XEffect* pEffect = global.emgr->GetXEffect( m_uidEffectSoul);
		if ( pEffect)
		{
			pEffect->GetEffect()->SetTransform( info.effect_pos, vec3( 1, 0, 0));

			// 소울 이펙트 중지 - 위치 상 안좋지만 그나마 가장 이쁘게 보인다. =_=
			if ( m_bArrival == true)
				pEffect->GetEffect()->Stop();
		}
	}


#ifdef SHOW_ABSORB_EFFECT
	// Update absorb effect
	if ( m_uidEffectAbsorb != MUID::ZERO)
	{
		XEffect* pEffect = global.emgr->GetXEffect( m_uidEffectAbsorb);
		if ( pEffect)
			pEffect->GetEffect()->SetTransform( info.dest_pos, vec3( 1, 0, 0));
	}
#endif


	// Delete object
	if ( (m_bArrival == true)  &&  (info.elipsed > 3000))
	{
#ifdef TEST_SOUL
		RebirthTest();
#else
		DeleteMe();
#endif
	}
}


bool XSoulObject::OnWaiting( float fDelta, SoulUpdateInfo& info)
{
	if ( m_bWaiting == false)
		return true;

	if ( info.elipsed < m_nWaitTime)
		return false;


	m_bWaiting = false;
	m_dwTime = info.curr_time;
	info.elipsed = 0.0f;
	info.radian = 0.0f;


	// Get start position
	if (m_pNPC->GetActor()->GetActorNodePos(L"dummy_ef_body", m_vStartPos, E_TS_WORLD))
		m_vPos = m_vStartPos;


	// Add soul effect
	if ( m_uidEffectSoul == MUID::ZERO)
	{
		RMatrix matLocal;
		matLocal.SetLocalMatrix(m_vPos, vec3(1,0,0), vec3(0,0,1));
		m_uidEffectSoul = global.emgr->AddXEffect( SOUL_EFFECT_NAME[ m_nSoulType], NULL, NULL, matLocal);
		if ( m_uidEffectSoul == MUID::ZERO)
		{
			mlog( "Cannot create soul effect.\n");
			_ASSERT( 0);

			DeleteMe();

			return false;
		}
	}


	// Add splash effect
#ifdef SHOW_SPLASH_EFFECT
	RMatrix matSplashLocal;
	matSplashLocal.SetLocalMatrix(m_vPos, vec3(1,0,0), vec3(0,0,1));
	m_uidEffectSplash = global.emgr->AddXEffect( NAME_SOUL_SPLASH, NULL, NULL, matSplashLocal);
#endif


	// Get random flying factor
	GetRandomFlyingFactor();


	// Get back rush state
	GetBackRushState();


	return true;
}


bool XSoulObject::OnUpdateMoveSoul( float fDelta, SoulUpdateInfo& info)
{
	// 이미 도착 상태
	if ( m_bArrival == true)
	{
		m_vPos = info.dest_pos;

		return true;
	}


	// 아직 움직여야 하는 상태
	vec3 diff = info.dest_pos - m_vPos;
	vec3 dir = Normalize( diff);
	info.toler = dir * ( fDelta * m_fMoveSpeed);
	m_fMoveSpeed = min( m_fMoveSpeed + fDelta * 1000.0f,  5000.0f);


	// Back rush 상태
	if ( m_bBackRush == true)
	{
		if ( info.radian > _2PI)
		{
			m_bBackRush = false;

			m_fMoveSpeed = 300.0f;
		}
		else
		{
			m_vPos.x = m_vStartPos.x + 200.0f * m_vStartDir.x * ( cos( info.radian) - 1.0f);
			m_vPos.y = m_vStartPos.y + 200.0f * m_vStartDir.y * ( cos( info.radian) - 1.0f);
			m_vPos.z += info.toler.z;

			return true;
		}
	}


	// 목적지 도착인지 확인
	float dist1 = info.toler.LengthSq();
	float dist2 = diff.LengthSq();

	if ( dist1 > dist2)
	{
		// 목적지 도착
		m_vPos = info.dest_pos;

		m_bArrival = true;
		m_dwTime = info.curr_time;


#ifdef SHOW_ABSORB_EFFECT
		// 흡수 이펙트 생성
		RMatrix matLocal;
		matLocal.SetLocalMatrix(m_vPos, vec3(1,0,0), vec3(0,0,1));
		m_uidEffectAbsorb = global.emgr->AddXEffect( NAME_SOUL_ABSORB, NULL, NULL, matLocal);
#endif

		return true;
	}


	// 아니면 위치만 업데이트
	m_vPos += info.toler;


	return true;
}


bool XSoulObject::OnUpdateEffect( float fDelta, SoulUpdateInfo& info)
{
	info.effect_pos = m_vPos;

	if ( m_bArrival == true)
		return true;


	vec3 diff = info.dest_pos - m_vPos;


	// Get distance ratio
	float dist_ratio =  (m_bBackRush == true)  ?  75.0f  :  min( diff.Length() * 0.1f,  100.0f);


	// 수평 업데이트
	vec3 dir = diff;
	XMath::NormalizeZ0( dir);
	vec3 cross = CrossProduct( dir, vec3( 0.0f, 0.0f, 1.0f));
	info.effect_pos.x += dist_ratio * cross.x * m_vFlyingFactor.x * sin( info.radian);
	info.effect_pos.y += dist_ratio * cross.y * m_vFlyingFactor.y * sin( info.radian);


	// 수직 업데이트
	if ( info.radian < _2PI)
		info.effect_pos.z += dist_ratio * m_vFlyingFactor.z * ( -cos( info.radian) + 1.0f);
	else
		info.effect_pos.z += dist_ratio * 0.3f * sin( info.radian * 2.0f);


	return true;
}



#ifdef TEST_SOUL

void XSoulObject::RebirthTest()
{
	m_vPos = m_vStartPos;
	m_fMoveSpeed = 0.0f;
	m_bArrival = false;
	m_dwTime = timeGetTime();

	GetRandomFlyingFactor();

	GetBackRushState();


	if ( m_uidEffectSoul != MUID::ZERO)
	{
		global.emgr->DeleteXEffect( m_uidEffectSoul);
		m_uidEffectSoul = MUID::ZERO;
	}

	RMatrix matLocal;
	matLocal.SetLocalMatrix(m_vStartPos, vec3(1,0,0), vec3(0,0,1));
	m_uidEffectSoul = global.emgr->AddXEffect( SOUL_EFFECT_NAME[ 0], NULL, NULL, m_vStartPos);


#ifdef SHOW_SPLASH_EFFECT
	if ( m_uidEffectSplash != MUID::ZERO)
	{
		global.emgr->DeleteXEffect( m_uidEffectSplash);
		m_uidEffectSplash = MUID::ZERO;
	}
	RMatrix matSplashLocal;
	matSplashLocal.SetLocalMatrix(m_vPos, vec3(1,0,0), vec3(0,0,1));
	m_uidEffectSplash = global.emgr->AddXEffect( NAME_SOUL_SPLASH, NULL, NULL, matSplashLocal);
#endif


#ifdef SHOW_ABSORB_EFFECT
	if ( m_uidEffectAbsorb != MUID::ZERO)
	{
		global.emgr->DeleteXEffect( m_uidEffectAbsorb);
		m_uidEffectAbsorb = MUID::ZERO;
	}
#endif
}

#endif


