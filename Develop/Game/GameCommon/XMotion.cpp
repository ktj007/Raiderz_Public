#include "StdAfx.h"
#include "XMotion.h"
#include "CSStrings.h"
#include "XDef.h"

XMotionAniSequence::XMotionAniSequence() : m_fTimeOut(0.0f)
{
	m_bWeaponAni = false;
	m_szAniName[0] = 0;
	bExistEvent = false;
	szEvent[0] = 0;
	m_fSpeed = 1.0f;
	m_nFrameOver = 0;
}

void XMotionAniSequence::SetAniName(const TCHAR* szAniName, bool bWeaponAni)
{ 
	_tcscpy_s(m_szAniName, szAniName); 
	if (bWeaponAni)
	{
		m_WeaponAniName.resize(WEAPON_MAX);
		for (int i = 0; i < WEAPON_MAX; i++)
		{
			m_WeaponAniName[i] = tstring(CSStrings::WeaponType(WEAPON_TYPE(i))) +
				_T("_") + tstring(m_szAniName);
		}
	}
	m_bWeaponAni = bWeaponAni;
}

const TCHAR* XMotionAniSequence::GetAniName( WEAPON_TYPE nWeaponType )
{
	if (!m_bWeaponAni) return m_szAniName;

	return m_WeaponAniName[nWeaponType].c_str();
}
///////////////////////////////////////////////////////////////////////////////
XMotion::XMotion() : m_bLoop(false), 
					 m_bCancelEnabled(true), 
					 m_nStanceType(MS_ALL), 
					 m_bCustomAni(false), 
					 m_bWeaponAni(true),
					 m_bExistTrigger(false),
					 m_bAddPostfix(false),
					 m_pEventLister(NULL),
					 m_bAutoPrefix(true)
{
	SetCameraLook(false);
}

XMotion::~XMotion()
{
	for (int i = 0; i < MT_COUNT; i++)
	{
		int nSize = (int)m_vecAniSequence[i].size();
		for (int j = 0; j < nSize; j++)
		{
			delete m_vecAniSequence[i][j];
		}
		m_vecAniSequence[i].clear();
	}
	if (m_pEventLister)
	{
		SAFE_DELETE(m_pEventLister);
	}
}

XMotionAniSequence*	XMotion::GetAniSequence(XMotionTypes nMotionType, int index)
{
	if ((index < 0) || (m_vecAniSequence[nMotionType].size() <= (size_t)index)) return NULL;
	return m_vecAniSequence[nMotionType][index]; 
}

void XMotion::Cook()
{
	CheckTrigger();
}

void XMotion::CheckTrigger()
{
	bool bExist = false;

	for (int i = 0; i < MT_COUNT; i++)
	{
		int nSize = (int)m_vecAniSequence[i].size();
		for (int j = 0; j < nSize; j++)
		{
			XMotionAniSequence* pSeq = m_vecAniSequence[i][j];
			if (pSeq->m_PostTrigger.IsValid()) 
			{
				bExist = true;
				break;
			}
		}
		if (bExist) break;
	}

	m_bExistTrigger = bExist;
}

void XMotion::SetCameraLook( bool b )
{
	for (int i = 0; i < MS_MAX; i++)
	{
		m_bCameraLook[i] = b;
	}
}
