#include "stdafx.h"
#include "ZGameServerObject.h"
#include "ZSharedField.h"
#include "ZFieldManager.h"
#include "ZGlobal.h"
#include "ZCommandCenter.h"
#include "ZCommandTable.h"
#include "STransData.h"
#include "ZServer.h"

ZGameServerObject::ZGameServerObject(MUID uidObject, ZGameServerInfo* pGameServerInfo)
: SCommObject(uidObject), m_pGameServerInfo(pGameServerInfo)
{
	;
}

ZGameServerObject::~ZGameServerObject()
{
	;
}

bool ZGameServerObject::Init( ZFieldManager* pFieldManager )
{
	if (m_pGameServerInfo == NULL) return false;

	// 필드 인스턴스 생성
	for (size_t i = 0; i < m_pGameServerInfo->m_vecManagedSharedField.size(); i++)
	{
		ZManagedSharedField& managedSharedField = m_pGameServerInfo->m_vecManagedSharedField[i];

		int nFieldID = managedSharedField.nFieldID;

		for (size_t nChannelIndex = 0; nChannelIndex < managedSharedField.vecChannelID.size(); nChannelIndex++)
		{
			int nChannelID = managedSharedField.vecChannelID[nChannelIndex];

			bool bIsDefaultChannel = true;
			ZSharedField* pNewField = pFieldManager->CreateSharedField(nFieldID, nChannelID, m_pGameServerInfo->m_nID, bIsDefaultChannel);
			if (NULL == pNewField) return false;

			// GameServerObject의 FieldMap에도 추가한다.
			m_FieldMap.AddSharedField(pNewField);
		}
	}

	return true;
}
