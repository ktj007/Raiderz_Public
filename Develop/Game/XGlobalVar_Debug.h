#pragma once

/// 디버그 변수
struct GlobalVar_Debug
{




	// Render 관련
	bool	bShowAABB;			// AABB 박스
	bool	bShowCollision;		// 충돌구를 보여줄것인지 여부   
	bool	bShowHitCapsule;	// 판정 캡슐을 보여줄것인지 여부
	bool	bShowHitEffectCapsule;	// 판정 이펙트 캡슐을 보여줄것인지 여부
	bool	bShowSector;		// 섹터 경계선을 보여줄 것인지 여부 (소형)
	bool	bShowSector2;		// 섹터 경계선을 보여줄 것인지 여부 (대형)
	bool	bShowSensor;		// 센서를 그려줄 지 여부
	bool	bShowMoveCollision_Real;	// 이동충돌 폴리곤 보여줄 것인지 여부
	bool	bShowMoveCollision_Poly;	// 가공된 이동충돌 폴리곤 보여줄 것인지 여부
	bool	bShowmovecollision_Down;	// 아래에 충돌된 폴리곤을 보여줄 것인지 여부
	bool	bShowMoveCollision_View;	// 주변 충돌 폴리곤 보여줄 것인지 여부
	bool	bShowMoveCollision;
	bool	bShowmovecollision_Investigate;	// 충돌 검사를 Tick 단위로 할 수 있게 해줌
	bool	bShowObjectCollision_Investigate; // 오브젝트 충돌 검사를 계산별로 해줌
	bool	bShowMagicArea;		// 마법진 영역을 보여줄것인지 여부
	bool	bShowArea;
	bool	bShowID;
	bool	bShowNpcMoveInfo;	// NPC 이동 정보(서버로부터의 정보)

	bool	bShowSoundRange;	// 사운드 영역을 보여줄 것인지 여부

	// Debug Information
	wstring		strTestMotionName;	// 테스트할 모션 이름
	MF_STATE	nTestMF;			// 테스트할 모션 팩터
	int			nTestMFWeight;		// 테스트할 모션 팩터 weight
	MUID		uidTargetNPC;		// 테스트할 NPC

	// Server Logic Debug
	bool	bWarpToPickingPos;
	map<MUID, vector<vec3> >	mapNPCMovePathList;	// NPC의 이동 경로
	vector< TD_DEBUG_POS > vecActorPos;	// NPC의 이동 경로
	map<MUID, bool >			mapNPCMovePathFailed;	
	map<MUID, vec3 >			mapNPCMovePathFailedPos;
	map<MUID, TD_DEBUG_SPHERE >	mapNPCSignalSphere;
	map<MUID, vector<RCapsule> >	mapActorCapsule;
	vector<pair<vec3, vec3> >	mapNPCPickingList;
	vector<pair<vec3, vec3> >	mapNPCValidPickingList;
	bool						bRenderMoveDebug;

	// benchmark
	bool	bAutoReportCrashDump;

	// general message
	map< wstring, wstring >		mapGeneralDebuggingMsg;

	// Show Move Collision
	int		nShowMoveCollision_InvestigateIndex;
	int		nShowObjectCollision_InvestigateIndex;

	// Camera
	bool	bCameraLock;
	MUID	uidCameraLock;
	wstring strCameraLockBoneName;
	float	fTestCameraLockRightValue;
	float	fTestCameraLockBackValue;
	float	fTestCameraLockHeightValue;

	// guard
	bool	bForceGuard;

	GlobalVar_Debug();
};
