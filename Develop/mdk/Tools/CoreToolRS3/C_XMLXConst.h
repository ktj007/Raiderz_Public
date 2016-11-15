#ifndef _XCONST_H
#define _XCONST_H

#include "MTypes.h"

/// 상수값
class CORETOOL_API XCONST
{
public:
	// 설정 상수 --------------------------
	static int			CAMERA_FOV;							// FOV (각도)
	static float		CAMERA_NEAR_Z;
	static float		CAMERA_FAR_Z;

	static float		TREE_LOD_NEAR;
	static float		TREE_LOD_FAR;

	static float		GRASS_LOD_NEAR;
	static float		GRASS_LOD_FAR;


	// 게임 설정값 --------------------------
	static int			GAME_CAMERA_FOV;
	static float		GAME_CAMERA_NEAR_Z;
	static float		GAME_CAMERA_FAR_Z;

	static float		GAME_TREE_LOD_NEAR;
	static float		GAME_TREE_LOD_FAR;

	static float		GAME_GRASS_LOD_NEAR;
	static float		GAME_GRASS_LOD_FAR;


	static bool Load(const char* szFileName);
};




#endif