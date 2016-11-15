#pragma once

#include "GTestWrapper.h"
#include "GNPCInfoMgr.h"
#include "GNPCMgr.h"
#include "SUnitTestUtil.h"

class MockField;

class FBaseNPC
{
public:
	FBaseNPC()
	{

	}
	virtual ~FBaseNPC()
	{

	}

	GNPCInfo* NewNPCInfo(int nNPCId = INVALID_ID);

	GEntityNPC* NewNPC(MockField* pField, int nNPCId = INVALID_ID, const vec3& vPos=vec3::ZERO);
	template<typename T>
	static T* NewMockNPC(MockField* pField, int nNPCId=INVALID_ID, const vec3& vPos=vec3::ZERO)
	{
		return GUTHelper_NPC::NewMockNPC<T>(pField, nNPCId, vPos);
	}

	GTestMgrWrapper<GNPCInfoMgr>				m_NPCInfoMgrWrapper;
	GTestMgrWrapper<GNPCMgr>					m_NPCMgrWrapper;
};

