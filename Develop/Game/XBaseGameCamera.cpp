#include "stdafx.h"
#include "XBaseGameCamera.h"
#include "XGame.h"
#include "XWorld.h"
#include "XCollisionTree.h"
#include <math.h>

XBaseGameCamera::XBaseGameCamera( void )
{
	m_vOldPos.Set(0, 0, 0);
	m_vOldDir.Set(0, 0, 0);
	m_fCameraSmoothMoveTime		= 0.0f;

	m_bSmoothMove				= false;
	m_bColTerrain				= false;
}

XBaseGameCamera::~XBaseGameCamera( void )
{

}

bool XBaseGameCamera::CheckTerrainCollision( vec3& eye, vec3& tar, vec3& outPos )
{
	if(gg.currentgamestate->GetWorld()->GetTerrain() &&
		gg.currentgamestate->GetWorld()->GetTerrain()->GetHeightFieldPtr() == NULL)
		return false;

	if ( gg.currentgamestate )
	{
		_ASSERT( gg.currentgamestate->GetWorld() != NULL );
		m_Collision.SetTargetCollisionTree(gg.currentgamestate->GetWorld()->GetCollisionTree());
	}
	
	return m_Collision.CameraMoveTest(tar, eye, outPos, NULL);
}

void XBaseGameCamera::UpdateCameraSmoothMove( vec3& eye, vec3& tar, vec3& dir, vec3& outPos, vec3& outDir, float fDelta, bool bColTerrain )
{
	// 지형 충돌에 의해서 카메라 이동을 완만하게 한다.
	if(m_vOldPos == vec3(0, 0, 0))
	{
		m_vOldPos = eye;
	}

	// 지형 충돌인가?
	if(bColTerrain)
	{
		// 지형 충돌이 바로 걸린거면... 스무스하게 하자.
		if(m_bColTerrain == false)
		{
			m_fCameraSmoothMoveTime = 0.0f;
			m_bSmoothMove			= true;
		}
		else
		{
			// 계속 충돌 중이라면... 거리비교해서 스무스하게 하자.
			// 이전과의 거리 비교
			float fPreDist			= (tar - m_vOldPos).Length();
			float fNewDist			= (tar - eye).Length();
			if(fabs(fNewDist - fPreDist) > 30.0f)
			{
				// 거리차가 30 이상일때만 한다.
				m_bSmoothMove		= true;
			}
		}

		m_bColTerrain = true;
	}
	else
	{
		// 이전에 지형 충돌이 일어났으면... 스무스하게 하자
		if(m_bColTerrain == true)
		{
			m_fCameraSmoothMoveTime = 0.0f;
			m_bSmoothMove = true;
		}

		m_bColTerrain = false;
	}

	//------------------------------------------------------------------------
	// 카메라 스무스 이동 계산
	if(m_bSmoothMove == true)
	{
		// 부드럽게 이동중이라면...
		m_fCameraSmoothMoveTime += fDelta;

		// 스무스 이동은 0.5초 이후에 꺼진다.
		float fCurrentSmoothMoveTimeRatio = 1.0f - (m_fCameraSmoothMoveTime / 0.5f);
		if(fCurrentSmoothMoveTimeRatio < 0.0f)
		{
			// 완료
			fCurrentSmoothMoveTimeRatio = 0.0f;
			m_bSmoothMove				= false;
		}
		else
		{
			outPos		= m_vOldPos.GetInterpolated(eye, fCurrentSmoothMoveTimeRatio);
			outDir		= m_vOldDir.GetInterpolated(dir, fCurrentSmoothMoveTimeRatio);
		}
	}

	m_bSmoothMove = false;

	//------------------------------------------------------------------------
	// 검사 결과가 스무스 이동이 아니라면...
	if(m_bSmoothMove == false)
	{
		outPos = eye;
		outDir = dir;
	}

	m_vOldPos = outPos;
	m_vOldDir = outDir;
}

float XBaseGameCamera::BezierCurve( float fStart, float fP1, float fP2, float fEnd, float fRatio )
{
	return ((1 - fRatio) * (1 - fRatio) * (1 - fRatio) * fStart) + 
		(3 * fRatio * (1 - fRatio) * (1 - fRatio) * fP1) + 
		(3 * fRatio * fRatio * (1 - fRatio) * fP2) + 
		(fRatio * fRatio * fRatio * fEnd);
}

float XBaseGameCamera::GetCameraRadiusNearZ( float fNearZ, float fFoveX, float fFoveY )
{
	return sqrt(pow(fNearZ, 2) + pow(tan(fFoveX / 2.0f) * fNearZ, 2) + pow(tan(fFoveY / 2.0f) * fNearZ, 2));
}

void XBaseGameCamera::SetCollisionRadius( float fNearZ, float fFoveX, float fFoveY )
{
	m_Collision.SetRadius(GetCameraRadiusNearZ(fNearZ, fFoveX, fFoveY) + 10.0f);
}