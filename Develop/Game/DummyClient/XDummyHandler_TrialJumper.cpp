#include "stdafx.h"
#include "XDummyHandler_TrialJumper.h"
#include "XBirdDummyAgent.h"
#include "XFieldInfo.h"

MCommandResult XDummyHandler_TrialJumper::OnCommand(XBirdDummyAgent* pAgent, MCommand* pCommand)
{
	XBirdClient* pClient = pAgent->GetClient();
	switch(pCommand->GetID())
	{
	case MC_FIELD_START_GAME:		
		{
			MakeFieldList();

			if (pAgent->IsStayTrialField())
			{
				// 공유필드로 퇴장
				int nFieldID = pAgent->GetPreStayFieldID();
				vec3 vPos = pAgent->GetLoginFieldStartPos();
				XBIRDPOSTCMD2(pClient, MC_GM_MOVE_REQ, MCommandParameterInt(nFieldID), MCommandParameterVector(vPos));				
			}
			else
			{
				// 트라이얼 필드 입장
				int nFieldGroupID = GetRandomFieldGroupID();
				if (nFieldGroupID == INVALID_ID)	return CR_ERROR;

				XBIRDPOSTCMD1(pClient, MC_DYNAMIC_FIELD_ENTER_REQ, MCommandParameterInt(nFieldGroupID));
			}
		}		
		return CR_TRUE;
	}

	return CR_FALSE;
}

void XDummyHandler_TrialJumper::MakeFieldList(void)
{
	if (!m_vecFieldGroupID.empty())	return;

	// FieldGroupID 수집 - 클라이언트에서 FieldGroup 정보를 관리하는 책임객체가 없다.
	// 따라서 워프센서에서 사용되는 실제 FieldGroup 정보를 수집한다.
	set<int> setFieldGroupID;

	CSFieldList& rFieldList = info.field->GetFieldList();
	for(CSFieldList::iterator it = rFieldList.begin(); it != rFieldList.end(); it++)
	{		
		// 필드파일을 읽는다.
		CSFieldFileInfo* pInfo = it->second;
		if (pInfo == NULL)			continue;
			
		int nFieldID = it->first;
		auto_ptr<XFieldInfo> pFieldInfo(info.field->NewFieldInfo(nFieldID));		
		if (pFieldInfo.get() == NULL)		continue;


		// 센서워프 정보를 통해, 이동가능한 FieldGroup을 수집한다.
		for each(SENSOR_INFO_MAP_BY_ID::value_type pairSensor in pFieldInfo->m_mapSensorByID)
		{		
			SENSOR_INFO& sensorInfo = pairSensor.second;						
			for each(SENSOR_WARP_INFO warpInfo in sensorInfo.vecWarpInfos)
			{
				int nFieldGroupID = warpInfo.nFieldGroupID;
				if (nFieldGroupID == INVALID_ID)	continue;
				
				setFieldGroupID.insert(nFieldGroupID);
			}
		}
	}

	// 수집 정리
	m_vecFieldGroupID.resize(setFieldGroupID.size());
	copy(setFieldGroupID.begin(), setFieldGroupID.end(), m_vecFieldGroupID.begin());
}

int XDummyHandler_TrialJumper::GetRandomFieldGroupID(void)
{
	if (m_vecFieldGroupID.empty()) return INVALID_ID;
	
	int nRearIndex = m_vecFieldGroupID.size() - 1;	
	int nRandomIndex = RandomNumber(0, nRearIndex);
	
	return m_vecFieldGroupID[nRandomIndex];
}
