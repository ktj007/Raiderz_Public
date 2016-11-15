#pragma once

#include "GGlobal.h"
#include "MockField.h"
#include "GUTHelper.h"
#include "GFieldMgr.h"
#include "GNPCInfoMgr.h"
#include "GTestWrapper.h"

struct CommonFixture_FN
{
	CommonFixture_FN();
	~CommonFixture_FN();

	GUTHelper		m_Helper;
	MockField*		m_pField;
	GNPCInfo*		m_pNPCInfo;
	GFieldInfo*		m_pFieldInfo;

	GTestMgrWrapper<GFieldMgr>					m_FieldMgr;
	GTestMgrWrapper<GNPCInfoMgr>				m_NPCInfoMgr;
};
