#include "StdAfx.h"
#include "TPlayMotion.h"
#include "XMotion.h"
#include "XMotionMgr.h"
#include "RAnimationResource.h"

TPlayMotion::TPlayMotion(void)
: m_pCurrMotion(NULL)
, m_nCurrType(MT_DEFAULT)
, m_nCurrAniSequence(0)
, m_bPlayDone(false)
, m_bChanged(false)
, m_nWeaponType(WEAPON_1H_SLASH)
, m_pMotionMgr(NULL)
, m_bIgnoreLoop(false)
, m_bForceNoWeaponAni(false)
, m_bPlayAnimationList(false)
, m_pActor(NULL)
, m_fPlayAnimationListSpeed(1.0f)
{
	m_pMotionMgr		= new XMotionMgr;
}

TPlayMotion::~TPlayMotion(void)
{
	ClearAnimationSequence();

	SAFE_DELETE(m_pMotionMgr);
}

bool TPlayMotion::ChangeMotion(const char* szMotionName, XMotionTypes nType/* =MT_DEFAULT */, bool bIgnoreLoop /*= false*/)
{
	return _ChangeMotion(szMotionName, nType, bIgnoreLoop);
}

bool TPlayMotion::ChangeMotion(const char* szMotionName, const char* szNextMotionName, XMotionTypes nType/* =MT_DEFAULT */, bool bIgnoreLoop /*= false*/)
{
	bool ret = _ChangeMotion(szMotionName, nType, bIgnoreLoop);
	if(ret)
	{
		PushMotion(szNextMotionName);
	}

	return ret;
}

bool TPlayMotion::_ChangeMotion(const char* szMotionName, XMotionTypes nType, bool bIgnoreLoop /*= false*/)
{
	m_bIgnoreLoop		= bIgnoreLoop;
	m_bChanged			= true;
	m_bPlayDone			= false;
	m_nCurrType			= MT_DEFAULT;
	m_nCurrAniSequence	= 0;
	m_bForceNoWeaponAni = false;

	m_bPlayAnimationList = false;
	m_fPlayAnimationListSpeed = 1.0f;

	ResetAniTime();

	if(m_pCurrMotion == NULL)
		return false;

	ClearAnimationSequence();
	SetAnimationSequence(szMotionName);

	m_nCurrType = nType;

	if(m_pCurrMotion->m_vecAniSequence[m_nCurrType].empty()) m_nCurrType = MT_DEFAULT;

	if( SetAnimation(true) == false)
	{
		return false;
	}

	return true;
}

void TPlayMotion::PushMotion( const char* szMotionName, bool bIgnoreLoop /*= false*/ )
{
	SetAnimationSequence(szMotionName);
}

bool TPlayMotion::SetAnimation( bool bForce /*= false*/, bool bTestPlay /*= false*/ )
{
	if( m_pCurrMotion ||
		m_pActor == NULL)
	{
		if( !m_pCurrMotion->m_vecAniSequence[m_nCurrType].empty() )
		{
			string strCurrAni;
			bool bWeaponAni = CheckWeaponAni();

			// TODO: 현재 상태에 대한 플래그 필요
			//if( m_nCurrStance == CS_BATTLE) bWeaponAni = true;


			if( bWeaponAni )
			{
				strCurrAni = string(m_pCurrMotion->m_vecAniSequence[m_nCurrType][m_nCurrAniSequence]->GetAniName((WEAPON_TYPE)m_nWeaponType));
			}
			else
			{
				strCurrAni = string(m_pCurrMotion->m_vecAniSequence[m_nCurrType][m_nCurrAniSequence]->GetAniName());
			}

			// 모드 체크
			strCurrAni = GetAniNameWithMode(strCurrAni);

			//pAnimation->SetAnimationLoopType( RAniLoopType_HoldLastFrame );
			bool ret = m_pActor->SetAnimation(strCurrAni, bForce, false, bTestPlay);
			
			// 씨퀀스에 속도가 설정되어 있으면 그 속도로 바꿔준다.
			if (ret)
			{
				float speed = m_pCurrMotion->m_vecAniSequence[m_nCurrType][m_nCurrAniSequence]->GetSpeed();

				if(m_bPlayAnimationList)
					speed = m_fPlayAnimationListSpeed;

				if(speed != 1.0f)
					m_pActor->SetAnimationSpeed(speed);
				else 
					speed = m_pActor->GetAnimationSpeed();

				if(m_bPlayAnimationList == false)
				{
					GlobalObjects::g_pMainForm->AniSpeedtrackBar->Value = (int)(speed * ANISPEED);
					GlobalObjects::g_pMainForm->Speedlabel->Text = String::Format("Speed : {0}", speed);
				}
			}
			else
			{
				RAnimation* pAnimation = m_pActor->GetAnimation((char*)strCurrAni.c_str());
				if( pAnimation == NULL)
				{
					m_pActor->InitMeshNodeNoiseRef();
					m_pActor->StopAnimation();

					mlog("에러 : TalentID = %d , 애니파일(%s)이 존재하지 않습니다.\r",
						g_pMainApp->GetTalentID(), strCurrAni.c_str());

					CurMotionClear();
					return false;
				}
			}

			//return g_pMainApp->GetActor()->SetAnimation((char*)strCurrAni.c_str());//, false);
			return true;
		}
		else
		{
			if( m_pCurrMotion->IsCustom())
			{
				string strCurrAni = GetCustomMotionAniName(m_pCurrMotion->GetName());

				return m_pActor->SetAnimation(strCurrAni, bForce, false, bTestPlay);
			}
		}
	}

	return false;
}

void TPlayMotion::OnUpdate(float fDelta)
{
	m_bChanged = false;

	if( !m_pActor) return;

	if( m_pCurrMotion)
	{
		int nSeqSize = (int)m_pCurrMotion->m_vecAniSequence[m_nCurrType].size();

		if(m_bPlayAnimationList && m_pActor->GetCurAnimation() && m_vecCustomAnimationListEndTime.size() > m_nCurrAniSequence && m_vecCustomAnimationListEndTime[m_nCurrAniSequence].bUse)
		{
			if(m_pActor->GetCurAnimation()->GetAliasName() == m_pCurrMotion->m_vecAniSequence[m_nCurrType][m_nCurrAniSequence]->GetAniName())
			{
				float fTime = m_pActor->GetCurFrameToSec();

				if(m_vecCustomAnimationListEndTime[m_nCurrAniSequence].fEndTime <= fTime)
				{
					int nAniSequence = m_nCurrAniSequence + 1;
					if(m_nCurrAniSequence >= (nSeqSize-1))
						nAniSequence = 0;
						
					SetSequence(nAniSequence);
				}
			}
		}

		if(m_nCurrAniSequence < nSeqSize-1)
		{
			int nFrameOver = m_pCurrMotion->m_vecAniSequence[m_nCurrType][m_nCurrAniSequence]->GetFrameOver();
			NextSequence(m_pActor);
			return;
		}
		
		if(m_pActor->IsOncePlayDone())
		{
			if( m_nCurrAniSequence >= (nSeqSize-1))
			{
				if(m_bPlayAnimationList)
					SetSequence(0);
			}
			else
			{
				NextSequence(m_pActor);
			}
		}
	}
}

void TPlayMotion::SetSequence( int nSequence, bool bForce /*= false*/ )
{
	m_nCurrAniSequence = nSequence;
	SetAnimation(bForce, m_bPlayAnimationList);

	if( m_strLastEvent != "")
	{
		OnEvent(m_strLastEvent.c_str());
	}
}

//속도는 뭐 필요없지...
//각 애니메이션의 속도는 무시하고 전체 모션의 속도를 일정하게 맞추는 거라...
void TPlayMotion::ResetAniTime()
{
	if(g_pMainApp->GetActor())
	{
		g_pMainApp->GetActor()->SetSpeed();//GlobalObjects::g_pMainForm->AniSpeedtrackBar->Value / 48.f);
	}
}

void TPlayMotion::Clear()
{
	m_pMotionMgr->Clear();
}

bool TPlayMotion::Init()
{
	m_bPlayDone		= false;
	m_pCurrMotion	= NULL;
	m_nCurrType		= MT_DEFAULT;
	m_nCurrAniSequence = 0;

	bool bLoad = m_pMotionMgr->Init();

	InitMotion();

	return bLoad;
}

void TPlayMotion::OnEvent(const char* szEvent)
{
	if(m_pCurrMotion == NULL ) return;

	int nSeqSize = (int)m_pCurrMotion->m_vecAniSequence[m_nCurrType].size();
	if( m_nCurrAniSequence >= nSeqSize-1 ) return;

	for(int nSeqIndex = m_nCurrAniSequence; nSeqIndex < nSeqSize-1; nSeqIndex++)
	{
		if(m_pCurrMotion->m_vecAniSequence[m_nCurrType][nSeqIndex]->IsExistEvent())
		{
			if (!stricmp(m_pCurrMotion->m_vecAniSequence[m_nCurrType][nSeqIndex]->GetEventName(), szEvent))
			{
				SetSequence(nSeqIndex+1);
				m_strLastEvent = "";
				return;
			}
		}
	}

	m_strLastEvent = szEvent;
}

int TPlayMotion::GetCurTalentFrameMax()
{
	int size = (int)m_pCurrMotion->m_vecAniSequence[m_nCurrType].size();
	return GetCurTalentFrame(size);
}

int TPlayMotion::GetCurTalentFrame()
{
	return GetCurTalentFrame(m_nCurrAniSequence+1);
}

int TPlayMotion::GetCurTalentCurFrame()
{
	if(!m_pCurrMotion ||
		!m_pActor) return 0;

	int maxframe = GetCurTalentFrame(m_nCurrAniSequence);

	maxframe += m_pActor->GetAnimationFrame();

	return maxframe;
}

int TPlayMotion::GetCurTalentFrame(int AniSequenceindex)
{
	if( !m_pCurrMotion ) return 0;

	int maxframe = 0;;
	for (int j = 0; j < AniSequenceindex; ++j)
	{
		maxframe += GetTalentFrame(j);
	}

	return maxframe;
}

float TPlayMotion::GetCurTalentCurFrametime()
{
	if( !m_pCurrMotion ||
		!m_pActor ) return 0;

	float time = GetCurTalentFrametime(m_nCurrAniSequence);

	if(m_pCurrMotion->m_vecAniSequence[m_nCurrType].empty()) return 0.f;

	XMotionAniSequence* pMotionAniSequence = m_pCurrMotion->m_vecAniSequence[m_nCurrType][m_nCurrAniSequence];
	if (pMotionAniSequence == NULL) return 0.f;

	float speed = m_pActor->GetAnimationSpeed() * 1000.f;
	time += m_pActor->GetAnimationFrame()/4.8f/speed;

	return time;
}

float TPlayMotion::GetCurTalentMaxFrametime()
{
	if (m_pCurrMotion)
	{
		int size = (int)m_pCurrMotion->m_vecAniSequence[m_nCurrType].size();
		return GetCurTalentFrametime(size);
	}
	else
	{
		float timespeed = m_pActor->GetAnimationSpeed() * 1000.f;
		float motiontime = m_pActor->GetAnimationMaxFrame()/ 4.8f /timespeed;
		return motiontime;
	}
}

float TPlayMotion::GetCurTalentFrametime(int nSeqIndex)
{
	if( !m_pCurrMotion ||
		!m_pActor) return 0;

	bool bWeaponAni = false;

	if( m_nWeaponType != 0)
		bWeaponAni = true;

	if (g_pMainApp->IsPlayer())
	{
		bWeaponAni = true;
	}

	if(nSeqIndex > GetCurAniMaxSequence())
		return 0.0f;

	float timesum = 0.0f;
	for (int j = 0; j < nSeqIndex; ++j)
	{
		RAnimation* animation = NULL;
		if( bWeaponAni)
			animation = m_pActor->GetAnimation((char*)m_pCurrMotion->m_vecAniSequence[m_nCurrType][j]->GetAniName((WEAPON_TYPE)m_nWeaponType));
		else
			animation = m_pActor->GetAnimation((char*)m_pCurrMotion->m_vecAniSequence[m_nCurrType][j]->GetAniName());
		if( animation)
		{
			float fMotionSpeed = m_pCurrMotion->m_vecAniSequence[m_nCurrType][j]->GetSpeed();
			float speed = (fMotionSpeed != 1.0) ? fMotionSpeed * 1000.f : animation->GetSpeed() * 1000.f;
			float timespeed = m_pCurrMotion->m_vecAniSequence[m_nCurrType][j]->GetFrameOver()/4.8f/speed;
			if( timespeed == 0)
				timespeed = animation->GetResource()->GetMaxFrame()/4.8f/speed;

			timesum += timespeed;
		}
	}

	return timesum;
}

void TPlayMotion::SetFrame(int frame)
{
	if( !m_pActor) return;

	for (int j = 0; j < (int)m_pCurrMotion->m_vecAniSequence[m_nCurrType].size(); ++j)
	{
		if(GetCurTalentFrame(j + 1) >= frame)
		{
			SetSequence(j);
			m_pActor->SetAnimationFrame(frame - GetCurTalentFrame(j));
			break;
		}
	}
}

bool TPlayMotion::ChangeCustomMotion( const char* szMotionName, const char* szAnimationName, XMotionTypes nType /*= MT_DEFAULT*/, bool bIgnoreLoop /*= false*/, bool bNormalStance /*= false*/ )
{
	m_vCustomMotionAniName.clear();

	WEAPON_TYPE eWeaponType = (WEAPON_TYPE)m_nWeaponType;
	if(bNormalStance)
		eWeaponType = WEAPON_NONE;
	m_vCustomMotionAniName.push_back(SCustomAniName(szMotionName, MakeFullPlayerTalentAniName(string(szAnimationName), eWeaponType)));

	return ChangeMotion(szMotionName, nType, bIgnoreLoop);
}

void TPlayMotion::SetWeaponType( WEAPON_TYPE weapontype )
{
	m_nWeaponType = weapontype;
}

string TPlayMotion::GetCustomMotionAniName( const string& szMotionName )
{
	for (size_t i=0; i<m_vCustomMotionAniName.size(); i++)
	{
		if (m_vCustomMotionAniName[i].m_strMotion == szMotionName)
		{
			return m_vCustomMotionAniName[i].m_strAni;
		}
	}
	return "";
}

void TPlayMotion::PushCustomMotion( const char* szMotionName, const char* szAnimationName, bool bIgnoreLoop /*= false*/, bool bNormalStance /*= false*/ )
{
	WEAPON_TYPE eWeaponType = (WEAPON_TYPE)m_nWeaponType;
	if(bNormalStance)
		eWeaponType = WEAPON_NONE;
	m_vCustomMotionAniName.push_back(SCustomAniName(szMotionName, MakeFullPlayerTalentAniName(string(szAnimationName), eWeaponType)));
	PushMotion(szMotionName, bIgnoreLoop);
}

bool TPlayMotion::IsAnimationLoop( TCharacter* pActor )
{
	RAnimation* pAni = pActor->GetCurAnimation();
	bool CheckBool = (pAni && pAni->GetAnimationLoopType() == RAniLoopType_Loop) ? true : false;
	return CheckBool;
}

string TPlayMotion::MakeFullPlayerTalentAniName( string& szAniName, WEAPON_TYPE nCurrWeaponType )
{
	if(g_pMainApp->IsPlayer())
		return string(XStrings::WeaponType(nCurrWeaponType)) + string("_") + string(szAniName);

	return szAniName;
}

void TPlayMotion::InitMotion()
{
	if(m_pMotionMgr == NULL)
		return;

	m_pCurrMotion = m_pMotionMgr->GetMotion(MOTION_NAME_CUSTOM);
}

void TPlayMotion::ClearAnimationSequence()
{
	// Delete가 아니다.
	if( m_pCurrMotion )
	{
		for(int i = 0; i < MT_COUNT; i++)
		{
			m_pCurrMotion->m_vecAniSequence[i].clear();
		}
	}

	for(vector<XMotionAniSequence*>::iterator it = m_vCustomAnimationSequence.begin(); it != m_vCustomAnimationSequence.end(); it++)
	{
		SAFE_DELETE(*it);
	}

	m_vCustomAnimationSequence.clear();
	m_vecCustomAnimationListEndTime.clear();
}

void TPlayMotion::SetAnimationSequence( const char* szMotionName )
{
	XMotion* pPushMotion = m_pMotionMgr->GetMotion(szMotionName);

	if(pPushMotion == NULL)
		return;

	if(pPushMotion->IsCustom())
	{
		// 없으면... 커스텀이다.
		string strCurrAni = GetCustomMotionAniName(szMotionName);
		SetCustomAnimaitonSequence(strCurrAni.c_str());
	}
	else
	{
		m_bForceNoWeaponAni = !pPushMotion->IsWeaponAni();
		for(int i = 0; i < MT_COUNT; i++)
		{
			for(vector<XMotionAniSequence*>::iterator itAni = pPushMotion->m_vecAniSequence[i].begin(); itAni != pPushMotion->m_vecAniSequence[i].end(); itAni++)
			{
				m_pCurrMotion->m_vecAniSequence[i].push_back(*itAni);
			}
		}
	}
}

void TPlayMotion::SetCustomAnimaitonSequence( const char* szAnionName )
{
	XMotionAniSequence* pNewSequence = new XMotionAniSequence;
	pNewSequence->SetAniName(szAnionName, false);
	m_vCustomAnimationSequence.push_back(pNewSequence);
	m_pCurrMotion->m_vecAniSequence[MT_DEFAULT].push_back(pNewSequence);
}

int TPlayMotion::GetTalentFrame( int nAniSequenceindex )
{
	if( !m_pActor) return 0;

	if(m_pCurrMotion->m_vecAniSequence[m_nCurrType].size() <= nAniSequenceindex)
		return 0;

	RAnimation* animation = NULL;
	if(CheckWeaponAni())
		animation = m_pActor->GetAnimation((char*)m_pCurrMotion->m_vecAniSequence[m_nCurrType][nAniSequenceindex]->GetAniName((WEAPON_TYPE)m_nWeaponType));
	else
		animation = m_pActor->GetAnimation((char*)m_pCurrMotion->m_vecAniSequence[m_nCurrType][nAniSequenceindex]->GetAniName());

	if( animation)
	{
		int frame = m_pCurrMotion->m_vecAniSequence[m_nCurrType][nAniSequenceindex]->GetFrameOver();
		if( frame == 0)
			frame = animation->GetResource()->GetMaxFrame();

		return frame;
	}

	return 0;
}

void TPlayMotion::CurMotionClear()
{
	m_bIgnoreLoop		= false;
	m_bChanged			= false;
	m_bPlayDone			= false;
	m_nCurrType			= MT_DEFAULT;
	m_nCurrAniSequence	= 0;

	ResetAniTime();

	ClearAnimationSequence();
}

void TPlayMotion::CalAnimationDummyLocLastMatrixList( TCharacter* pActor, vector<RMatrix>& vecOutMatrixList )
{
	vecOutMatrixList.clear();

	if(GetMotion() == NULL)
		return;

	int nMaxFrame = 0;
	int nAniSequence = GetMotion()->m_vecAniSequence[m_nCurrType].size();
	for(int i = 0; i < nAniSequence; i++)
	{
		int nTextMaxFrame = GetTalentFrame(i);
		nMaxFrame += nTextMaxFrame;
		SetFrame(nMaxFrame);

		vec3 vAnimPos = vec3::ZERO;
		vec3 vAnimDir = vec3::AXISY;

		if(pActor->IsExistActorNode("dummy_loc"))
		{
			pActor->GetMovingDummyPosition(vAnimPos);
			pActor->GetMovingDummyDirection(vAnimDir);
		}
		else
		{
			vAnimDir = -vAnimDir;
		}

		MMatrix tm;
		tm.SetLocalMatrix(vAnimPos, -vAnimDir, vec3::AXISZ);

		vecOutMatrixList.push_back(tm);
	}

	SetFrame(0);
}

rs3::RMatrix TPlayMotion::GetAnimationDummyLocLastMatrix( int nAniSequence, vector<RMatrix>& vecMatrixList )
{
	RMatrix matResult;
	matResult.MakeIdentity();

	for(int i = 0; i < nAniSequence; i++)
	{
		if(vecMatrixList.size() <= i)
			break;

		matResult *= vecMatrixList[i];
	}

	return matResult;
}

void TPlayMotion::ChangeCustomAnimationList( vector<string>& vecAnimaionList )
{
	m_bIgnoreLoop		= true;
	m_bChanged			= true;
	m_bPlayDone			= false;
	m_nCurrType			= MT_DEFAULT;
	m_nCurrAniSequence	= 0;
	m_bForceNoWeaponAni = true;

	m_bPlayAnimationList = true;

	ResetAniTime();

	if(m_pCurrMotion == NULL)
		return;

	ClearAnimationSequence();

	for(vector<string>::iterator it = vecAnimaionList.begin(); it != vecAnimaionList.end(); it++)
	{
		XMotionAniSequence* pNewSequence = new XMotionAniSequence;
		pNewSequence->SetAniName((*it).c_str(), false);
		m_vCustomAnimationSequence.push_back(pNewSequence);
		m_pCurrMotion->m_vecAniSequence[m_nCurrType].push_back(pNewSequence);

		TPlayMotion::stEndTime endTime;
		m_vecCustomAnimationListEndTime.push_back(endTime);
	}

	if(CheckExistCurAnimation() == false)
	{
		// 없는 애니메이션이네...
		CurMotionClear();
		return;
	}

	if( SetAnimation(false, m_bPlayAnimationList) == false)
	{
		return;
	}
}

void TPlayMotion::ChangeCustomAnimationListSpeed(float fSpeed)
{
	if(m_bPlayAnimationList == false ||
		m_pActor == NULL)
		return;

	m_fPlayAnimationListSpeed = fSpeed;

	m_pActor->SetAnimationSpeed(fSpeed);
}

bool TPlayMotion::CheckWeaponAni()
{
	bool bWeaponAni = false;

	if(m_nWeaponType != 0)//WEAPON_NONE)
		bWeaponAni = true;

	if(g_pMainApp->IsPlayer())
		bWeaponAni = true;
	else
		bWeaponAni = false;

	if(m_pCurrMotion->IsWeaponAni() == false)
		bWeaponAni = false;

	if(m_bForceNoWeaponAni)
		bWeaponAni = false;

	return bWeaponAni;
}

bool TPlayMotion::CheckExistCurAnimation()
{
	bool bExist = true;
	int nAniCount = m_pCurrMotion->m_vecAniSequence[m_nCurrType].size();
	vector<string> vecErrAniName;
	for(int i = 0; i < nAniCount; i++)
	{
		string strCurrAni;
		bool bWeaponAni = CheckWeaponAni();

		if( bWeaponAni )
		{
			strCurrAni = string(m_pCurrMotion->m_vecAniSequence[m_nCurrType][m_nCurrAniSequence]->GetAniName((WEAPON_TYPE)m_nWeaponType));
		}
		else
		{
			strCurrAni = string(m_pCurrMotion->m_vecAniSequence[m_nCurrType][m_nCurrAniSequence]->GetAniName());
		}

		RAnimation* pAnimation = g_pMainApp->GetActor()->GetAnimation((char*)strCurrAni.c_str());
		if(pAnimation == NULL)
		{
			bExist = false;

			vecErrAniName.push_back(strCurrAni);
		}
	}

	if(bExist == false)
	{
		string strErr;
		char chErr[256] = {0, };
		sprintf_s(chErr, "TalentID = %d\r", g_pMainApp->GetTalentID());
		strErr = chErr;

		for(vector<string>::iterator itErr = vecErrAniName.begin(); itErr != vecErrAniName.end(); itErr++)
		{
			strErr += "\t 애니파일(" + *itErr +")이 존재하지 않습니다.\r";
		}

		System::Windows::Forms::MessageBox::Show(gcnew String(strErr.c_str()), "확인");
	}

	return bExist;
}

void TPlayMotion::NextSequence( TCharacter* m_pActor )
{
	if(m_pCurrMotion->m_vecAniSequence[m_nCurrType].empty()) return;

	int nFrameOver = m_pCurrMotion->m_vecAniSequence[m_nCurrType][m_nCurrAniSequence]->GetFrameOver();
	if ((m_pActor->IsOncePlayDone()) || (nFrameOver > 0 && m_pActor->GetAnimationFrame() >= nFrameOver))
	{
		bool bForce = CheckNextSequenceAnimation(m_nCurrAniSequence + 1);
		SetSequence(++m_nCurrAniSequence, bForce);
		return;
	}
}

bool TPlayMotion::CheckNextSequenceAnimation( int nNextSequence )
{
	if(m_pCurrMotion == NULL)
		return false;

	string strCurAniName = m_pCurrMotion->m_vecAniSequence[m_nCurrType][m_nCurrAniSequence]->GetAniName();
	string strNextAniName = m_pCurrMotion->m_vecAniSequence[m_nCurrType][nNextSequence]->GetAniName();

	if(strCurAniName == strNextAniName)
		return true;

	return false;
}

string TPlayMotion::GetAniNameWithMode( string strAniName )
{
	RAnimation* pAnimation = m_pActor->GetAnimation(strAniName.c_str());
	if(pAnimation == NULL)
	{
		if(m_pCurrMotion->IsAutoPrefix())
		{
			for(int i = 1; i < NPC_MODE_MAX; ++i)
			{
				char chModeID[16] = {0,};
				sprintf_s(chModeID, "%d", i);
				string strAniFullName = "mode" + string(chModeID) + "_" + strAniName;
				RAnimation* pModeAnimation = m_pActor->GetAnimation(strAniFullName.c_str());
				if(pModeAnimation)
				{
					return strAniFullName;
				}
			}
		}
	}

	return strAniName;
}

void TPlayMotion::GetCurMotionAnimationList( vector<string>& vecList )
{
	if(m_pCurrMotion == NULL)
		return;

	bool bWeaponAni = CheckWeaponAni();

	for(int i = 0; i < GetCurAniMaxSequence(); ++i)
	{
		XMotionAniSequence* pSeq = m_pCurrMotion->GetAniSequence(m_nCurrType, i);
		if(pSeq)
		{
			string strAni;
			if( bWeaponAni )
			{
				strAni = string(pSeq->GetAniName((WEAPON_TYPE)m_nWeaponType));
			}
			else
			{
				strAni = string(pSeq->GetAniName());
			}

			vecList.push_back(strAni);
		}
	}
}

bool TPlayMotion::CheckMotionInfoModify()
{
	if(m_pMotionMgr)
	{
		m_pMotionMgr->Clear();
		return Init();
	}

	return false;
}

void TPlayMotion::SetCustomAnimationListEndTime( int nIndex, float fEndTime )
{
	if(m_vecCustomAnimationListEndTime.empty())
		return;

	if(nIndex > m_vecCustomAnimationListEndTime.size() || fEndTime <= 0.0f)
		return;

	m_vecCustomAnimationListEndTime[nIndex].bUse = true;
	m_vecCustomAnimationListEndTime[nIndex].fEndTime = fEndTime;
}