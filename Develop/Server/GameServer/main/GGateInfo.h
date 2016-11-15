#pragma once

#include "MTypes.h"
#include "MUID.h"
#include "STransData_M2G.h"

/// 필드 이동 정보
struct GGateInfo
{
	int					nFieldID;
	vec3				vPosition;
	vec3				vDirection;
	bool				bDynamicField;

	// shared field
	int					nChannelID;

	// dynamic field
	MUID				uidDynamicFieldGroup;

	GGateInfo()
		: nFieldID(0)
		, vPosition(vec3::ZERO)
		, vDirection(vec3::AXISY)
		, bDynamicField(false)	
		, nChannelID(INVALID_CHANNELID)
		, uidDynamicFieldGroup(MUID::ZERO)
	{
	}

	void Assign(TD_GATE_INFO& tdGateInfo)
	{
		this->nFieldID = tdGateInfo.nFieldID;
		this->vPosition = tdGateInfo.vPosition;
		this->vDirection = tdGateInfo.vDirection;
		this->bDynamicField = tdGateInfo.bDynamicField;
		this->nChannelID = tdGateInfo.nChannelID;
		this->uidDynamicFieldGroup = tdGateInfo.uidDynamicFieldGroup;
	}

	void Export(TD_GATE_INFO& outTdGateInfo) const
	{
		outTdGateInfo.nFieldID = this->nFieldID;
		outTdGateInfo.vPosition = this->vPosition;
		outTdGateInfo.vDirection = this->vDirection;
		outTdGateInfo.bDynamicField = this->bDynamicField;
		outTdGateInfo.nChannelID = this->nChannelID;
		outTdGateInfo.uidDynamicFieldGroup = this->uidDynamicFieldGroup;
	}
};

struct GATE_INFO_DYNAMIC
{
	DYNAMIC_FIELD_TYPE	eType;
	int					nFieldGroupID;
	bool				bSingleEnter;

	int					nFieldID;
	vec3				vecDestPos;
	vec3				vecDestDir;

	GATE_INFO_DYNAMIC()
		: eType(DFT_NONE)
		, nFieldGroupID(0)
		, bSingleEnter(false)
		, nFieldID(0)
		, vecDestPos(vec3::ZERO)
		, vecDestDir(vec3(0.0f, 1.0f, 0.0f))
	{
	}

	GATE_INFO_DYNAMIC(DYNAMIC_FIELD_TYPE eType, int nFieldGroupID, int nFieldID, vec3 vecDestPos, vec3 vecDestDir, bool bIsSingleEnter = false)
		: eType(eType)
		, nFieldGroupID(nFieldGroupID)
		, nFieldID(nFieldID)
		, vecDestPos(vecDestPos)
		, vecDestDir(vecDestDir)
		, bSingleEnter(bIsSingleEnter)
	{
		if (DFT_TRIAL != eType)
			bSingleEnter = false;
	}
};
