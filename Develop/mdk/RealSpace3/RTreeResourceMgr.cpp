#include "stdafx.h"

#include "RTreeResourceMgr.h"
#include "RTreePassRenderer.h"

namespace rs3 {

RTreeResourceMgr::RTreeResourceMgr()
: m_vWindDirection(RVector(1, 0, 0))
, m_fWindStrength(0.0f)
, m_pTexGray(NULL)
, m_pTexBlue(NULL)
{
	if (!CSpeedTreeRT::IsAuthorized())
	{
		// Speed-tree 4 Full Licesense Key
		bool bRet = CSpeedTreeRT::Authorize("05DA695E7AD8F1D4");
		if ( !bRet )
		{
			_ASSERT( !"스피드 트리 인증 실패" );
			mlog("failed speedtree authorized\n");
		}
	}
}

DWORD DirVec3fToRGB(const RVector3 &v)
{
	RVector t = v;
	t.Normalize();

	int r = static_cast<int>(255*(t.x+1.0f)*.5f);
	int g = static_cast<int>(255*(t.y+1.0f)*.5f);
	int b = static_cast<int>(255*(t.z+1.0f)*.5f);

	return D3DCOLOR_XRGB(r, g, b);
}

void RTreeResourceMgr::Init( RSceneManager* pSceneManager )
{
	m_pSceneManager = pSceneManager;

	// create default texture
	DWORD gray[4];
	for (int i=0; i<4; i++)
		gray[i] = 0x00808080;	
//		gray[i] = 0xFF808080;	// detail map 때문에 알파값이 0 이어야 함

	m_pTexGray = REngine::GetDevice().CreateTexture(2, 2, RFMT_ARGB32, RTF_LINEAR, 0, gray, "tree_gray");

	DWORD blue[4];
	blue[0] = DirVec3fToRGB(RVector3(-1, -1, 1));
	blue[1] = DirVec3fToRGB(RVector3( 1, -1, 1));
	blue[2] = DirVec3fToRGB(RVector3(-1,  1, 1));
	blue[3] = DirVec3fToRGB(RVector3( 1,  1, 1));

	m_pTexBlue = REngine::GetDevice().CreateTexture(2, 2, RFMT_ARGB32, RTF_LINEAR, 0, blue, "tree_blue");

 	// Init Wind
	InitWind();
}

void RTreeResourceMgr::InitWind()
{
	//NumWindMatricesAndLeafAngles 6 8
	//WindResponseAndLimit 10 1
	//MaxBendAngle 35
	//BranchExponent 2
	//LeafExponent 2
	//GustStrengthMinMax 0.25 1.25
	//GustDurationMinMax 2 15
	//GustFrequency 0.166667
	//BranchOscillationX_LaLsHaHs 0.3 3 6 3
	//BranchOscillationY_LaLsHaHs 0.1 4 8 4
	//LeafRocking_LaLsHaHs 2 4 5 20
	//LeafRustling_LaLsHaHs 7 3 4 40

	m_SpeedWind.SetQuantities( 6, 8 );
	m_SpeedWind.SetWindResponse(10.0f, 1.0f);
	m_SpeedWind.SetMaxBendAngle( 35 );
	m_SpeedWind.SetExponents( 2, 2 );
	m_SpeedWind.SetGusting( 0.25, 1.25, 0.166667, 2, 15 );
	m_SpeedWind.SetBranchHorizontal( 0.3, 6, 3, 3 );
	m_SpeedWind.SetBranchVertical( 0.1, 8, 4, 4 );
	m_SpeedWind.SetLeafRocking( 2, 5, 4, 20 );
	m_SpeedWind.SetLeafRustling( 7, 4, 3, 40 );

	SaveWindParams();

	CSpeedTreeRT::SetNumWindMatrices(m_SpeedWind.GetNumWindMatrices());
}

void RTreeResourceMgr::Destroy()
{
	REngine::GetDevice().DeleteTexture(m_pTexGray);
	REngine::GetDevice().DeleteTexture(m_pTexBlue);
}

void RTreeResourceMgr::SetWindDirection(const RVector& windDir)
{
	m_vWindDirection = windDir;
	if (m_vWindDirection.Length() < FLT_EPSILON)
		m_vWindDirection = RVector::AXISX;
	m_vWindDirection.Normalize();
	m_SpeedWind.SetWindStrengthAndDirection(m_fWindStrength, m_vWindDirection.x, m_vWindDirection.y, m_vWindDirection.z);
}

void RTreeResourceMgr::SetWindStrength(float windStr)
{
	if (m_fWindStrength < FLT_EPSILON)
	{
		LoadWindParams();
	}
	m_fWindStrength = windStr;
	if (m_fWindStrength < FLT_EPSILON)
	{
		SaveWindParams();
		m_SpeedWind.SetBranchHorizontal(0, 0, 0, 0);
		m_SpeedWind.SetBranchVertical(0, 0, 0, 0);
		m_SpeedWind.SetLeafRocking(0, 0, 0, 0);
		m_SpeedWind.SetLeafRustling(0, 0, 0, 0);
	}
	else
	{
		LoadWindParams();
	}
	m_SpeedWind.SetWindStrengthAndDirection(m_fWindStrength, m_vWindDirection.x, m_vWindDirection.y, m_vWindDirection.z);
	m_SpeedWind.SetGusting(0.0, 0.0, 5, 10, 15);
}

void RTreeResourceMgr::SaveWindParams()
{
	m_SpeedWind.GetBranchHorizontal(m_WindParams.branchHorz1, m_WindParams.branchHorz2, m_WindParams.branchHorz3, m_WindParams.branchHorz4);
	m_SpeedWind.GetBranchVertical(m_WindParams.branchVert1, m_WindParams.branchVert2, m_WindParams.branchVert3, m_WindParams.branchVert4);
	m_SpeedWind.GetLeafRocking(m_WindParams.leafRock1, m_WindParams.leafRock2, m_WindParams.leafRock3, m_WindParams.leafRock4);
	m_SpeedWind.GetLeafRustling(m_WindParams.leafRustle1, m_WindParams.leafRustle2, m_WindParams.leafRustle3, m_WindParams.leafRustle4);
}

void RTreeResourceMgr::LoadWindParams()
{
	m_SpeedWind.SetBranchHorizontal(m_WindParams.branchHorz1, m_WindParams.branchHorz2, m_WindParams.branchHorz3, m_WindParams.branchHorz4);
	m_SpeedWind.SetBranchVertical(m_WindParams.branchVert1, m_WindParams.branchVert2, m_WindParams.branchVert3, m_WindParams.branchVert4);
	m_SpeedWind.SetLeafRocking(m_WindParams.leafRock1, m_WindParams.leafRock2, m_WindParams.leafRock3, m_WindParams.leafRock4);
	m_SpeedWind.SetLeafRustling(m_WindParams.leafRustle1, m_WindParams.leafRustle2, m_WindParams.leafRustle3, m_WindParams.leafRustle4);
}

}
