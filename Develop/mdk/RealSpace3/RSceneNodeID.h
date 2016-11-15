#pragma once

namespace rs3 {

RS_API extern const char *RSID_SCENEINSTANCE;

// 월드 씬노드 - 다른 씬노드를 붙이는 월드
RS_API extern const char *RSID_WORLDSCENENODE;

// 카메라
RS_API extern const char *RSID_CAMERASCENENODE;
RS_API extern const char *RSID_CAMERAFPSSCENENODE;
RS_API extern const char *RSID_CAMERAARCBALLSCENENODE;

// 광원
RS_API extern const char *RSID_LIGHTSCENENODE;
RS_API extern const char *RSID_DIRECTIONALLIGHTSCENENODE;
RS_API extern const char *RSID_SPOTLIGHTSCENENODE;
RS_API extern const char *RSID_SUBLIGHTSCENENODE;

// 지형
RS_API extern const char *RSID_STATICMESHSCENENODE;
RS_API extern const char *RSID_TERRAINSCENENODE;

// 메쉬
RS_API extern const char *RSID_ACTOR;
RS_API extern const char *RSID_EFFECTACTOR;
RS_API extern const char *RSID_ACTORNODE;

// 오클루더
RS_API extern const char *RSID_OCCLUDER;

// cloth
RS_API extern const char *RSID_CLOTH;

// 나무
RS_API extern const char *RSID_TREE;

// 물 표면
RS_API extern const char *RSID_WATERPLANE;
RS_API extern const char *RSID_WATEROBJECT;

//	extern const char *RSID_SHADOWVOLUMESCENENODE	= 600;	// 직접 만들어지지 않는다

// 더미
RS_API extern const char *RSID_DUMMYNODE;

// 하늘
RS_API extern const char *RSID_SKY;

// 파티클
// 2d
RS_API extern const char *RSID_TEXTSCENENODE;

RS_API extern const char *RSID_EFFECT;
RS_API extern const char *RSID_EMITTER;
RS_API extern const char *RSID_TRAIL;

RS_API extern const char *RSID_EFFECTINSTANCE;

RS_API extern const char *RSID_DECAL_PROJECTION;	
RS_API extern const char *RSID_DECAL_POLYGON;

// 기타 등등
RS_API extern const char *RSID_MANAGED;

}
