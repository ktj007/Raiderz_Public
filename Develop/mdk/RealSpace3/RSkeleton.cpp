#include "StdAfx.h"
#include "RSkeleton.h"
#include "RAnimation.h"
#include "RMeshNode.h"

// 디버깅을 위해 include
#include "MDebug.h"
#include "RRenderHelper.h"

namespace rs3 {


///////////////////////////////////////
// 상수 & static

struct RBASICBONEINFO {
	const char* szName;
	RBIPID		parentID;
	float		baseImportance;
};

RBASICBONEINFO RBIPINFO[] = {
	"Bip01",			RBIPID_UNKNOWN,		1,
	"Bip01 Pelvis",		RBIPID_ROOT,		1,	
	"Bip01 Spine",		RBIPID_PELVIS,		1,	
	"Bip01 L Thigh",	RBIPID_SPINE,		1,	
	"Bip01 L Calf",		RBIPID_LTHIGH,		10,	
	"Bip01 L Foot",		RBIPID_LCALF,		1,	
	"Bip01 L Toe0",		RBIPID_LFOOT,		1,	
	"Bip01 R Thigh",	RBIPID_SPINE,		1,	
	"Bip01 R Calf",		RBIPID_RTHIGH,		10,	
	"Bip01 R Foot",		RBIPID_RCALF,		1,	
	"Bip01 R Toe0",		RBIPID_RFOOT,		1,	
	"Bip01 Spine1",		RBIPID_SPINE,		1,	
	"Bip01 Spine2",		RBIPID_SPINE1,		1,	
	"Bip01 Neck",		RBIPID_SPINE2,		1,	
	"Bip01 Head",		RBIPID_NECK,		1,	
	"Bip01 L Clavicle",	RBIPID_NECK,		1,	
	"Bip01 L UpperArm",	RBIPID_LCLAVICLE,	1,	
	"Bip01 L Forearm",	RBIPID_LUPPERARM,	1,	
	"Bip01 L Hand",		RBIPID_LFOREARM,	1,	
	"Bip01 R Clavicle",	RBIPID_NECK,		1,	
	"Bip01 R UpperArm",	RBIPID_RCLAVICLE,	1,	
	"Bip01 R Forearm",	RBIPID_RUPPERARM,	1,	
	"Bip01 R Hand",		RBIPID_RFOREARM,	1,	
	"Bip01 L Finger0",	RBIPID_LHAND,		1,	
	"Bip01 L Finger01",	RBIPID_LFINGER0,	1,	
	"Bip01 L Finger02",	RBIPID_LFINGER01,	1,	
	"Bip01 L Finger1",	RBIPID_LHAND,		1,	
	"Bip01 L Finger11",	RBIPID_LFINGER1,	1,	
	"Bip01 L Finger12",	RBIPID_LFINGER11,	1,	
	"Bip01 L Finger2",	RBIPID_LHAND,		1,	
	"Bip01 L Finger21",	RBIPID_LFINGER2,	1,	
	"Bip01 L Finger22",	RBIPID_LFINGER21,	1,	
	"Bip01 R Finger0",	RBIPID_RHAND,		1,	
	"Bip01 R Finger01",	RBIPID_RFINGER0,	1,	
	"Bip01 R Finger02",	RBIPID_RFINGER01,	1,	
	"Bip01 R Finger1",	RBIPID_RHAND,		1,	
	"Bip01 R Finger11",	RBIPID_RFINGER1,	1,	
	"Bip01 R Finger12",	RBIPID_RFINGER11,	1,	
	"Bip01 R Finger2",	RBIPID_RHAND,		1,	
	"Bip01 R Finger21",	RBIPID_RFINGER2,	1,	
	"Bip01 R Finger22",	RBIPID_RFINGER21,	1,	
	"Bip01 Ponytail1",	RBIPID_HEAD,		1,	
	"Bip01 Ponytail11",	RBIPID_PONYTAIL1,	1,	
	"Bip01 Ponytail2",	RBIPID_HEAD,		1,	
	"Bip01 Ponytail21",	RBIPID_PONYTAIL2,	1,	
	"Bip01 Tail",		RBIPID_SPINE,		1,	
	"Bip01 Tail1",		RBIPID_TAIL,		1,	
	"Bip01 Tail2",		RBIPID_TAIL1,		1,	
	"Bip01 Tail3",		RBIPID_TAIL2,		1,	
	"Bone01",			RBIPID_HEAD,		1,	
	"Bone03",			RBIPID_HEAD,		1,	
	"Bip01 Prop1",		RBIPID_ROOT,		1,	
};
											
const char* RSkeleton::GetBipName(RBIPID id)
{
	return RBIPINFO[id].szName;
}

RBIPID RSkeleton::GetBipID(const char* szName)
{
	//TODO: map 을 쓰면 좋겠다.
	for(int i=RBIPID_ROOT;i<RBIPID_END;i++)
	{
		if(stricmp(szName,RBIPINFO[i].szName)==0) 
			return (RBIPID)i;
	}
	return RBIPID_UNKNOWN;
}

RBIPID RSkeleton::GetBipParent(RBIPID id)
{
	if(id==RBIPID_UNKNOWN) return RBIPID_UNKNOWN;
	return RBIPINFO[id].parentID;
}

float RSkeleton::GetBaseBoneImportance(RBIPID id)
{
	if (id < 0 || id >= RBIPID_END) return 0.0f;

	return RBIPINFO[id].baseImportance;
}

}