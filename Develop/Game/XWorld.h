#ifndef _XWORLD_H
#define _XWORLD_H

#include "XGlobal.h"
#include "XFieldInfo.h"
#include "XWorldEnv.h"

#include "RTerrain.h"
#include "RCollisionTree.h"

typedef RCollisionTriangle CollisionAttribute;

using namespace rs3;
namespace rs3 {
class RActorNode;
class RPointLightSceneNode;
}

class XCollisionTree;
class XZone;

/// 월드 초기화 파라메타
struct XWorldInitParam
{
	int		nFieldID;
	wstring	strZoneName;

	bool	bBackgroundLoading;
	bool	bLoadPhysx;

	XWorldInitParam() : nFieldID(0), bBackgroundLoading(false), bLoadPhysx(true) {}
	void SetField(int fieldID, bool backgroundLoading)
	{
		nFieldID = fieldID;
		bBackgroundLoading = backgroundLoading;
	}
	void SetZone(const wchar_t* szZoneName, bool loadPhyx, bool backgroundLoading)
	{
		strZoneName = szZoneName;
		bBackgroundLoading = backgroundLoading;
		bLoadPhysx = loadPhyx;
	}
};

/// 필드, 스카이박스 등 월드의 배경을 담당
class XWorld
{
public:
	XWorld();
	virtual ~XWorld();

	//////////////////////////////////////////////////////////////////////////
	// 생성 / 소멸 / 초기화
	//
	virtual bool				Init(XWorldInitParam& worldInitParam);
	void						Final(bool bForChange=false);
	void						FinalForChange();
	bool						InitForChange(XWorldInitParam& worldInitParam);
	void						OnWorldLoadingComplete();
	bool						IsNowLoading() { return !m_bWorldLoadingComplete; }


	//////////////////////////////////////////////////////////////////////////
	// 업데이트, 주로 환경을 업데이트 한다.
	//
	void						Update(float fDelta);


	//////////////////////////////////////////////////////////////////////////
	// 환경관련 콜백메쏘드, XCmdHandler_Field 에 매핑되어 있다.
	//
	bool						OnCmdSetEnv(GAME_TIME_TYPE nTime, GAME_WEATHER_TYPE nWeather, bool bFade = false, float fFadingTime = 0.0f);
	bool						OnCmdSetEnv(const wchar_t* szEnvname, GAME_TIME_TYPE nTime, GAME_WEATHER_TYPE nWeather, bool bFade = false, float fFadingTime = 0.0f);
	void						OnCmdSetTime(GAME_TIME_TYPE nTime, bool bFade = false, float fFadingTime = 0.0f);
	void						OnCmdSetWeather(GAME_WEATHER_TYPE nWeather, bool bFade = false, float fFadingTime = 0.0f);
	bool						IsEnvSet(string szEnvname );

	//////////////////////////////////////////////////////////////////////////
	// 필드 관련 사운드 출력
	//
	void						PlayFieldSound();


	//////////////////////////////////////////////////////////////////////////
	// 섹터관련, XGame에서 ObjectCache에서 사용
	//
	MPoint						GetSectorIndex(float fPosX, float fPosY);
	static bool					IsManagedSector(int tar_sx, int tar_sy, int my_sx, int my_sy, const int nSectorSize);
	MRect						GetPickZPartialArea();


	//////////////////////////////////////////////////////////////////////////
	// 충돌관련. 여기저기서 불린다.
	//
	float						GetGroundZ(vec3& vPos, float fPickAddOriginZ = 10.0f);	/// 해당 위치의 바닥 높이를 알아온다.
	bool						PickCollision(vec3 vOrigin, vec3 vTo, vec3* pvOutPos=NULL, vec3* pvOutNormal=NULL, DWORD dwAttribute = ~CollisionAttribute::WATER);
	RMatrix						GetTerrainMatrix(const vec3& vPos, const vec3& vDir, const vec3& vUp, bool bUseAlign);

	wstring						GetMaterialName(vec3 vEntityPosition);


	//////////////////////////////////////////////////////////////////////////
	// 월드관련 정보 Getter
	//
	XFieldInfo*					GetInfo()			{ return m_pInfo; }
	XWorldEnv*					GetWorldEnv()		{ return &m_Env;}
	RTerrain*					GetTerrain()		{ return m_pTerrain; }
	XCollisionTree*				GetCollisionTree();
	RWorldSceneNode*			GetWorldSceneNode()	{ return m_pWorldSceneNode; }

	virtual bool				IsTest() { return false; }

private:
	bool						m_bWorldLoadingComplete;

protected:
	XWorldInitParam				m_WorldInitParam;
	RTerrain*					m_pTerrain;
	XFieldInfo*					m_pInfo;
	XWorldEnv					m_Env;							// 주위 환경을 담당(하늘, 날씨 등)

	RWorldSceneNode*			m_pWorldSceneNode;
	XZone*						m_pZone;

	void						SetupTerrainAndTree();
	void						InitDevice();
	bool						InitFieldInfo( int nFieldID, const wchar_t* szZoneName=NULL );

	void						_InitEnv( wstring &strZoneEnvFileName );
	void						_InitWorldScene( wstring &strZoneFileName, bool bBackgroundLoading );

	void						LoadObjSound();
	bool						LoadZoneSound(const wchar_t* szFileName);
};
//----------------------------------------------------------------------------------------------------

#endif // _XWORLD_H