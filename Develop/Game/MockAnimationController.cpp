#include "stdafx.h"
#include "MockAnimationController.h"
#include "MockCharacter.h"

TestAniDataSet::TestAniDataSet()
{

}

TestAniDataSet::~TestAniDataSet()
{
	Clear();
}

void TestAniDataSet::AddAni( const wchar_t* szName, double dTime, bool bLoop, double dSpeed /*= 1.0f*/ )
{
	if (GetAniData(szName) != NULL) return;

	TestAniData* pData = new TestAniData();
	pData->strName = szName;
	pData->dMaxFrameTime = dTime;
	pData->dSpeed = dSpeed;
	if (bLoop) pData->nLoopType = RAniLoopType_Loop;
	else pData->nLoopType = RAniLoopType_HoldLastFrame;

	m_Data.insert(map<wstring, TestAniData*>::value_type(wstring(szName), pData));
}

TestAniData* TestAniDataSet::GetAniData( const wchar_t* szAniName )
{
	map<wstring, TestAniData*>::iterator itor = m_Data.find(wstring(szAniName));
	if (itor == m_Data.end()) return NULL;

	return (*itor).second;
}

void TestAniDataSet::Clear()
{
	for (map<wstring, TestAniData*>::iterator itor = m_Data.begin();
		itor != m_Data.end(); ++itor)
	{
		delete (*itor).second;
	}
	m_Data.clear();
}
///////////////////////////////////////////////////////////////////////////////////
MockAnimationController::MockAnimationController( XCharacter* pCharacter, MPxAniLookUpTC* pAniLookUpTransformController)
: XGameTransformControllerManager(pCharacter, pAniLookUpTransformController), 
m_pCurrAni(NULL), 
m_pAniDataSet(NULL), 
m_dSpeed(1.0), 
m_bPlayed(false), 
m_dCurrFrameTime(0)
{

}

MockAnimationController::~MockAnimationController()
{

}

void MockAnimationController::Update( float fDelta )
{
	if (m_bPlayed == false) return;
	if (m_pCurrAni == NULL) return;

	m_fElapsedTime += fDelta;

	m_dCurrFrameTime += (fDelta * m_dSpeed * m_pCurrAni->dSpeed);

	if (m_dCurrFrameTime > m_pCurrAni->dMaxFrameTime)
	{
		m_bOncePlayDone = true;

		if (m_pCurrAni->nLoopType == RAniLoopType_HoldLastFrame)
		{
			m_dCurrFrameTime = m_pCurrAni->dMaxFrameTime;

			m_bPlayed = true;
		}
		else
		{
			while (m_pCurrAni->dMaxFrameTime < m_dCurrFrameTime) 
			{
				m_dCurrFrameTime -= m_pCurrAni->dMaxFrameTime;
				if (m_dCurrFrameTime < 0) m_dCurrFrameTime = 0;
			}
		}
	}

	UpdateDummyLoc(fDelta);
}

bool MockAnimationController::SetAnimation( wstring& strAniName, bool bForce/*=false*/, bool bSameFrame /*= false*/)
{
	m_fElapsedTime = 0.0f;

	if (strAniName == m_strLastAni) 
	{
		// bForce가 true이면 프레임을 초기화한다.
		if (bForce)
		{
			m_dCurrFrameTime = 0.0f;
			m_bPlayed = true;
		}

		return true;
	}

	if (m_pAniDataSet == NULL) return false;
	TestAniData* pData = m_pAniDataSet->GetAniData(strAniName.c_str());
	if (pData == NULL) return false;

	m_strLastAni = strAniName;

	m_pCurrAni = pData;
	m_dCurrFrameTime = 0;
	m_dSpeed = 1.0;
	m_bPlayed = true;
	m_bOncePlayDone = false;

	if (m_pCurrAni && m_pCurrAni->IsExistDummyLoc())
	{
		UpdateDummyLoc(0.0f);
	}

	return true;
}

float MockAnimationController::GetAnimationSpeed()
{
	return m_dSpeed;
}

void MockAnimationController::SetAnimationSpeed( float fSpeed )
{
	m_dSpeed = fSpeed;
}

void MockAnimationController::SetAnimationFrame( unsigned int nFrame )
{
	m_dCurrFrameTime = (nFrame / 4800.0);
}

float MockAnimationController::GetMaxFrameToSecond()
{
	if (m_pCurrAni == NULL) return 0.0f;
	return m_pCurrAni->dMaxFrameTime;
}

float MockAnimationController::GetCurFrameToSecond()
{
	return (float)m_dCurrFrameTime;
}

void MockAnimationController::PauseAnimation()
{
	m_bPlayed = false;
}

void MockAnimationController::StopAnimation()
{
	m_bPlayed = false;
	m_dCurrFrameTime = 0;
	m_bOncePlayDone = false;
}

void MockAnimationController::PlayAnimation()
{
	m_dCurrFrameTime = 0;
	m_bPlayed = true;
	m_bOncePlayDone = false;
}

wstring MockAnimationController::GetAnimationName()
{
	if (m_pCurrAni != NULL)
	{
		return m_pCurrAni->strName;
	}
	return L"";
}

bool MockAnimationController::IsAnimationLoop()
{
	if (m_pCurrAni != NULL)
	{
		return (m_pCurrAni->nLoopType == RAniLoopType_Loop);
	}
	return false;
}

int MockAnimationController::GetAniFrame()
{
	return (int)(m_dCurrFrameTime * 4800);
}

int MockAnimationController::GetAniMaxFrame()
{
	if (m_pCurrAni == NULL) return 0;

	return (int)(m_pCurrAni->dMaxFrameTime * 4800);
}

void MockAnimationController::SetAniDataSet( TestAniDataSet* pAniDataSet )
{
	m_pAniDataSet = pAniDataSet;
}

bool MockAnimationController::IsPlayDone()
{
	return m_bPlayed;
}

bool MockAnimationController::IsOncePlayDone()
{
	return m_bOncePlayDone;
}

void MockAnimationController::UpdateDummyLoc( float fDelta )
{
	// MockCharacter의 업데이트 실행
	// dummy_loc 때문에...
	if(m_pActor)
	{
		MockCharacter * pMockCharacter = (MockCharacter*)m_pActor;
		pMockCharacter->MockCharacterUpdate(fDelta);

		if (m_pCurrAni && m_pCurrAni->IsExistDummyLoc())
		{
			vec3 vDummyLocPos = m_pCurrAni->GetDummyLocData(m_fElapsedTime).vPos;
			vec3 vDummyLocDir = m_pCurrAni->GetDummyLocData(m_fElapsedTime).vDir;

			pMockCharacter->SetDummyLoc(vDummyLocPos, vDummyLocDir);
		}
	}
}