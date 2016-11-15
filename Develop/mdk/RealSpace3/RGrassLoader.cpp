#include "StdAfx.h"
#include "RGrassLoader.h"
#include "RSpeedGrass.h"
#include "RGrassSystem.h"
#include "RTerrain.h"

namespace rs3 {

const DWORD	GRS_VERSION3 = 0x00000003;
const DWORD GRS_VERSION4 = 0x00000004;
const DWORD GRS_VERSION5 = 0x00000005;
const DWORD GRS_VERSION6 = 0x00000006;

const DWORD GRS_VERSION = GRS_VERSION6;

// 풀 흔들림 크기를 파일에 저장된 값을 무시하고 랜덤설정 - charmke
// TODO: 파일 구조를 바꾸던가.. 깔끔하게 정리가 필요. - charmke, 어떻게 하죠 ? - rOseria
class RGrassLoaderImpl_V4 : public RGrassLoader
{
public:
	virtual bool Load(MFile& file, int bc, RSpeedGrass* pGrass, RGrassSystem* pGrassSystem, RTerrain* pTerrain)
	{
		// packed-blade to blade
		RGrassBlade b;

		CSpeedGrassRT::SPackedBlade4* PackedBlade = new CSpeedGrassRT::SPackedBlade4[bc];
		file.Read( PackedBlade, sizeof(CSpeedGrassRT::SPackedBlade4)*bc );

		for (int i=0; i<bc; i++)
		{
			PackedBlade[i].Unpack(b);
			b.m_fThrow = RandomNumber( c_fMinBladeThrow, c_fMaxBladeThrow );
			// 노말 구축
			// float[]을 RVector로 바로 넘겨도 된다.
			if( pTerrain)
				pTerrain->GetNormalVector( (RVector*)b.m_afPos, (RVector*)b.m_afNorm);
			pGrassSystem->AddBlade(b);
		}
		delete[] PackedBlade;
		return true;
	}
};

class RGrassLoaderImpl_V5 : public RGrassLoader
{
public:
	virtual bool Load(MFile& file, int bc, RSpeedGrass* pGrass, RGrassSystem* pGrassSystem, RTerrain* pTerrain)
	{
		// packed-blade to blade
		RGrassBlade b;

		CSpeedGrassRT::SPackedBlade5* PackedBlade = new CSpeedGrassRT::SPackedBlade5[bc];
		file.Read( PackedBlade, sizeof(CSpeedGrassRT::SPackedBlade5)*bc );

		for (int i=0; i<bc; i++)
		{
			PackedBlade[i].Unpack(b);
			b.m_fThrow = RandomNumber( c_fMinBladeThrow, c_fMaxBladeThrow);
			// 노말 구축
			// float[]을 RVector로 바로 넘겨도 된다.
			if( pTerrain)
				pTerrain->GetNormalVector( (RVector*)b.m_afPos, (RVector*)b.m_afNorm);
			pGrassSystem->AddBlade(b);
		}

		delete [] PackedBlade;
		return true;
	}
};

// 노말 저장 추가
class RGrassLoaderImpl_V6 : public RGrassLoader
{
public:
	virtual bool Load(MFile& file, int bc, RSpeedGrass* pGrass, RGrassSystem* pGrassSystem, RTerrain* pTerrain)
	{
		// packed-blade to blade
		RGrassBlade b;

		CSpeedGrassRT::SPackedBlade6* PackedBlade = new CSpeedGrassRT::SPackedBlade6[bc];
		file.Read( PackedBlade, sizeof(CSpeedGrassRT::SPackedBlade6)*bc );

		for (int i=0; i<bc; i++)
		{
			PackedBlade[i].Unpack(b);
			b.m_fThrow = RandomNumber( c_fMinBladeThrow, c_fMaxBladeThrow);
			pGrassSystem->AddBlade(b);
		}

		delete [] PackedBlade;
		return true;
	}
};

RGrassLoader* RGrassLoader::CreateLoader( DWORD dwVersion )
{
	if ( dwVersion != GRS_VERSION )
		mlog("grass version is not lastest\n");

	if ( dwVersion == GRS_VERSION4 )
		return new RGrassLoaderImpl_V4;
	else if ( dwVersion == GRS_VERSION5 )
		return new RGrassLoaderImpl_V5;
	else if ( dwVersion == GRS_VERSION6 )
		return new RGrassLoaderImpl_V6;

	mlog("grass version error\n");
	return NULL;
}

DWORD RGrassLoader::GetCurrentVersion()
{
	return GRS_VERSION;
}

}