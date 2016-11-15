#pragma once

#include "RResourceID.h"
#include "RResourceManager.h"

#pragma push_macro ("new")
#ifdef new
#undef new
#endif
#include "Dependencies/speedtree/SpeedTreeRT.h"
#pragma pop_macro ("new")

namespace rs3 {

class RTreeResource;
class RS_API RTreeResourceMgr : public RResourceManager
{
	friend class RTreeResource;

	//////////////////////////////////////////////////////////////////////////////////////////////////
	// Speed Wind
	//
	struct SSpeedWindParams
	{
		float branchHorz1, branchHorz2, branchHorz3, branchHorz4;
		float branchVert1, branchVert2, branchVert3, branchVert4;
		float leafRock1, leafRock2, leafRock3, leafRock4;
		float leafRustle1, leafRustle2, leafRustle3, leafRustle4;

		SSpeedWindParams() : 
		branchHorz1(0), branchHorz2(0), branchHorz3(0), branchHorz4(0),
			branchVert1(0), branchVert2(0), branchVert3(0), branchVert4(0),
			leafRock1(0), leafRock2(0), leafRock3(0), leafRock4(0),
			leafRustle1(0), leafRustle2(0), leafRustle3(0), leafRustle4(0)
		{}
	};

	CSpeedWind			m_SpeedWind;					// SpeedWind object that controls wind effects for entire forest
	SSpeedWindParams	m_WindParams;

	void						SaveWindParams();
	void						LoadWindParams();

	RVector						m_vWindDirection;
	float						m_fWindStrength;

public:
	RTreeResourceMgr();
	virtual ~RTreeResourceMgr(){}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Resource Manager 구현
	//
	DECLARE_RES_MGR_ID( RRESOURCETYPE::RR_TREE );

	virtual void		Init( RSceneManager* pSceneManager );
	virtual void		Destroy();
	virtual void		Clear(){}

	CSpeedWind*			GetSpeedWind();
	void				SetWindDirection(const RVector& windDir);
	void				SetWindStrength(float windStr);				// 0(바람없음)~1(최대)
	const RVector&		GetWindDirection();
	float				GetWindFinalStrength();

private:
	void				InitWind();

	RTexture*			m_pTexGray;	// 없는 텍스쳐에 대응하는 회색 텍스쳐
	RTexture*			m_pTexBlue;	// 없는 텍스쳐에 대응하는 노말 텍스쳐(파란색)
};

//----------------------------------------------------------------------------------------------------
inline CSpeedWind* RTreeResourceMgr::GetSpeedWind() { return &m_SpeedWind; }
//----------------------------------------------------------------------------------------------------
inline float RTreeResourceMgr::GetWindFinalStrength(){ return m_SpeedWind.GetFinalStrength(); }
//----------------------------------------------------------------------------------------------------
inline const RVector& RTreeResourceMgr::GetWindDirection(){ return m_vWindDirection; }
//----------------------------------------------------------------------------------------------------
}
