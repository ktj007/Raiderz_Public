#include "stdafx.h"
#include "RSceneNodeID.h"

namespace rs3 
{
// scene manager
const char *RSID_ROOT						="ROOT";

// 씬노드
const char *RSID_SCENEINSTANCE				="SCENEINSTANCE";

// 월드 씬노드. 루트에 월드가 붙는다.
const char *RSID_WORLDSCENENODE				="WORLD";

// 카메라
const char *RSID_CAMERASCENENODE			="CAMERA";
const char *RSID_CAMERAFPSSCENENODE			="CAMERAFPS";
const char *RSID_CAMERAARCBALLSCENENODE		="CAMERAARCBALL";

// 광원
const char *RSID_LIGHTSCENENODE				="LIGHT";
const char *RSID_DIRECTIONALLIGHTSCENENODE	= "DIRLIGHT";
const char *RSID_SPOTLIGHTSCENENODE			="SPOTLIGHT";
const char *RSID_SUBLIGHTSCENENODE			="SUBLIGHT";

// 지형
const char *RSID_STATICMESHSCENENODE		="STATICMESH";
const char *RSID_TERRAINSCENENODE			="RTERRAIN";

// 메쉬
const char *RSID_ACTOR						="ACTOR";
const char *RSID_EFFECTACTOR				="EFFECTACTOR";
const char *RSID_ACTORNODE					="ACTORNODE";

// 오클루더
const char *RSID_OCCLUDER					="OCCLUDER";

// cloth
const char *RSID_CLOTH						="CLOTH";

// 나무
const char *RSID_TREE						="TREE";

// 물 표면
const char *RSID_WATERPLANE					="WATERPLANE";

//	const char *RSID_SHADOWVOLUMESCENENODE	= 600;	// 직접 만들어지지 않는다

// 더미
const char *RSID_DUMMYNODE					= "DUMMYLIST";

// 하늘
const char *RSID_SKY						= "SKY";
// 파티클
// 2d
const char *RSID_TEXTSCENENODE				="TEXT";

const char *RSID_EFFECT						="EFFECT";
const char *RSID_EMITTER					="REMITTER";
const char *RSID_TRAIL						="RTRAIL";

const char *RSID_EFFECTINSTANCE				="EFFECTINSTANCE";

const char *RSID_DECAL_PROJECTION			="DECAL_PROJECTION";
const char *RSID_DECAL_POLYGON				="DECAL_POLYGON";

// 기타 등등
const char *RSID_MANAGED					= "MANAGED";

// 기본
const char* RSID_UNKNOWN					="unknown";

}