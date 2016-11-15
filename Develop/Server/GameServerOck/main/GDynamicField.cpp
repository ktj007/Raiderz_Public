#include "StdAfx.h"
#include "GDynamicField.h"
#include "GEntityPlayer.h"
#include "CCommandTable.h"
#include "GWeatherMgr.h"
#include "GCommandCenter.h"
#include "GCommand.h"
#include "GGlobal.h"
#include "GConst.h"

GDynamicField* ToDynamicField( GField* pField )
{
	VALID_RET(pField, NULL);
	if (!pField->IsDynamicField())			return NULL;
	return static_cast<GDynamicField*>(pField);
}



GDynamicField::GDynamicField(const MUID& uid, const MUID& uidGroup)
: GField(uid, new GWeatherMgr())
, m_uidGroup(uidGroup)
{
	
}

GDynamicField::~GDynamicField()
{
	SAFE_DELETE(m_pWeatherMgr);
}

bool GDynamicField::Create(const GFieldInfo* pFieldInfo)
{
	if (NULL == pFieldInfo) return false;

	bool bRet = __super::Create(pFieldInfo);

	m_pWeatherMgr->Init(pFieldInfo, GConst::CONST_ENV_WEATHER_TICK_SEC);

	return bRet;
}

void GDynamicField::Update( float fDelta )
{
	GField::Update(fDelta);

	PFI_B(83, "GDynamicField::Update - GDynamicField:Update_Weather");
	Update_Weather(fDelta);
	PFI_E(83);
}

void GDynamicField::Update_Weather(float fDelta)
{
	// ³¯¾¾°¡ ¹Ù²î¸é true ¸®ÅÏ
	if (m_pWeatherMgr->Update(fDelta))
	{
		OnWeatherChanged();
	}
}

void GDynamicField::RouteCancelExpire( vector<MUID> vecPlayers )
{
	MCommand* pNewCommand = MakeNewCommand(MC_DYNAMIC_FIELD_CANCEL_EXPIRE, 0, NULL);

	pNewCommand->AddReceiver(vecPlayers);
	gsys.pCommandCenter->PostCommand(pNewCommand);
}

void GDynamicField::AddEntity( GEntity* pEntity )
{
	__super::AddEntity(pEntity);

	if (pEntity->IsPlayer())
	{
		ToEntityPlayer(pEntity)->GetPlayerField().SetDynamicFieldGroupUID(m_uidGroup);
	}
}
