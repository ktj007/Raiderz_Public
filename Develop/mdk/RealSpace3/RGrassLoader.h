/**
@brief .grs 파일 버젼별 로더를 모아놓은 곳
*/

#pragma once

#include "MFileSystem.h"

namespace rs3 {

class RSpeedGrass;
class RGrassSystem;

class RGrassLoader
{
public:
	RGrassLoader() {}

	virtual bool Load(MFile& file, int bc, RSpeedGrass* pGrass, RGrassSystem* pGrassSystem, RTerrain* pTerrain) = 0;

	static RGrassLoader*	CreateLoader( DWORD dwVersion );
	static DWORD			GetCurrentVersion();
};

}