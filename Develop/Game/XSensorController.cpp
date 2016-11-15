#include "stdafx.h"
#include "XSensorController.h"
#include "XSensor.h"
#include "XSensorManager.h"
#include "XPost_Comm.h"
#include "XMyPlayer.h"

void XSensorController::Clear()
{
	EnteredSensor_PostLeave();

	m_listEnteredSensorID.clear();
	m_listEnteredSensorGroupName.clear();
}

void XSensorController::EnteredSensor_PostLeave()
{
	vec3 vMyPos = gvar.Game.pMyPlayer->GetPosition();

	if(!m_listEnteredSensorID.empty())
	{
		for (list<uint32>::iterator it = m_listEnteredSensorID.begin();
			it != m_listEnteredSensorID.end(); ++it)
		{
			uint32 nSensorID = (*it);
			XSensor* pSensor = m_pManager->Find(nSensorID);
			if (pSensor)
			{
				pSensor->OnLeave(vMyPos);
			}
		}
	}
}

void XSensorController::Update( XFieldInfo* pFieldInfo, vec3 vMyPos )
{
	CheckEnteredSensor();
	UpdateUsableSensorTrigger(vMyPos);
	UpdateUsableGroupSensorTrigger( pFieldInfo, vMyPos);
}

void XSensorController::UpdateUsableSensorTrigger(vec3& vMyPos )
{
	for (XSensorManager::XSensorMap::iterator itor = m_pManager->GetSensorMap().begin();
		itor != m_pManager->GetSensorMap().end(); ++itor)
	{
		int nID = (*itor).first;
		XSensor* pSensor = (*itor).second;
		if (pSensor)
		{
			SENSOR_RESULT nResult = pSensor->OnUpdate(IsEnteredSensor(nID), vMyPos);
			switch(nResult)
			{
				case SENSOR_RESULT_ENTER:	EnterSensor(nID);	break;
				case SENSOR_RESULT_LEAVE:	LeaveSensor(nID);	break;;
			}
		}
	}
}

void XSensorController::UpdateUsableGroupSensorTrigger( XFieldInfo* pFieldInfo, vec3& vMyPos )
{
	//TODO : 센서리스트로 바운딩 박스를 만들어서 체크할수 없을까?
	map<wstring, vector<int>>::iterator iter;
	for (iter = pFieldInfo->m_mapGroupSensor.begin();
		iter != pFieldInfo->m_mapGroupSensor.end(); ++iter)
	{
		wstring strGroupName = iter->first;
		vector<int>& list = iter->second;

		bool bEnter = false;

		for (vector<int>::iterator itor = list.begin();
			itor != list.end(); ++itor)
		{
			int nID = *itor;

			 XSensorManager::XSensorMap::iterator mitor = m_pManager->GetSensorMap().find(nID);
			 if (mitor == m_pManager->GetSensorMap().end())	break;

			 int nSensorID = (*mitor).first;
			 XSensor* pSensor = (*mitor).second;
			 if (pSensor)
			 {
				 XSensorInfo* pSensorInfo = pSensor->GetInfo();
				 if (pSensorInfo == NULL)			continue;
				 
				 float fDistance = pSensorInfo->vPoint.DistanceTo(vMyPos);
				 if (fDistance <= pSensorInfo->fRadius)
				 {
					 bEnter = true;
					 break;
				 }
			 }
		}

		XSensorManager::XSensorMap::iterator mitor = m_pManager->GetSensorMap().find(list[0]);
		if (mitor == m_pManager->GetSensorMap().end())	break;

		int nSensorID = (*mitor).first;
		XSensor* pSensor = (*mitor).second;
		if (pSensor)
		{
			if (bEnter)
			{
				if (!IsEnteredSensorGroup(strGroupName))
				{
					if(pSensor->OnEnter(vMyPos))
					{
						EnterSensorGroup(strGroupName);
					}
				}
			}
			else
			{
				if (IsEnteredSensorGroup(strGroupName))
				{
					if(pSensor->OnLeave(vMyPos))
					{
						LeaveSensorGroup(strGroupName);
					}
				}
			}
		}
	}
}


void XSensorController::CheckNowFocusedSensorIsInvalid( SENSOR_INFO_MAP_BY_ID &mapSensorByID )
{
	// 포커스된 센서가 유효하지 않다면, 제거한다.
	list<uint32>::reverse_iterator it = m_listEnteredSensorID.rbegin();
	while(it != m_listEnteredSensorID.rend())
	{
		uint32 nSensorID = *it;

		// 유효성 검사
		if (nSensorID != TRIGGER_INVALID_SENSOR_ID)
		{
			if (mapSensorByID.find(nSensorID) != mapSensorByID.end())
			{
				break;
			}
		}

		// 제거 - 이터레이터 케스팅: rit -> it -> rit
		it = list<uint32>::reverse_iterator(m_listEnteredSensorID.erase((++it).base()));
	}
}

void XSensorController::EnterSensor(uint32 nSensorID)
{
	if (nSensorID == TRIGGER_INVALID_SENSOR_ID)	return;
	if (IsEnteredSensor(nSensorID))				return;

	//mlog("센서 입장 = %d\n", nSensorID);

	m_listEnteredSensorID.push_back(nSensorID);	
}

void XSensorController::LeaveSensor(uint32 nSensorID)
{
	if (!IsEnteredSensor(nSensorID)) return;

	//mlog("센서 퇴장 = %d\n", nSensorID);

	list<uint32>::iterator it = find(m_listEnteredSensorID.begin(), m_listEnteredSensorID.end(), nSensorID);
	m_listEnteredSensorID.erase(it);
}

bool XSensorController::IsEnteredSensor(uint32 nSensorID)
{
	list<uint32>::iterator it = find(m_listEnteredSensorID.begin(), m_listEnteredSensorID.end(), nSensorID);
	return it != m_listEnteredSensorID.end();
}

void XSensorController::EnterSensorGroup(tstring strGroupName)
{
	if (strGroupName.empty())					return;
	if (IsEnteredSensorGroup(strGroupName))		return;

	m_listEnteredSensorGroupName.push_back(strGroupName);	
}

void XSensorController::LeaveSensorGroup(tstring strGroupName)
{
	if (!IsEnteredSensorGroup(strGroupName)) return;

	list<tstring>::iterator it = find(m_listEnteredSensorGroupName.begin(), m_listEnteredSensorGroupName.end(), strGroupName);
	m_listEnteredSensorGroupName.erase(it);
}

bool XSensorController::IsEnteredSensorGroup(tstring strGroupName)
{
	list<tstring>::iterator it = find(m_listEnteredSensorGroupName.begin(), m_listEnteredSensorGroupName.end(), strGroupName);
	return it != m_listEnteredSensorGroupName.end();
}

uint32 XSensorController::GetFocusedSensorID(void)
{
	if (m_listEnteredSensorID.empty())	return TRIGGER_INVALID_SENSOR_ID;
	
	// 가장 최근에 추가된 센서가 포커스된 센서 - 인터랙션 센서
	return m_listEnteredSensorID.back();
}

vector<uint32> XSensorController::GetEnteredSensorID(void)
{
	// 가장 최근에 추가된 센서부터 역순으로 내보낸다
	// 센서 활성화 여부를 서버에서 검사하기때문에 일단 다 보내야한다.
	vector<uint32> vecSensorID(m_listEnteredSensorID.size());
	copy(m_listEnteredSensorID.rbegin(), m_listEnteredSensorID.rend(), vecSensorID.begin());

	return vecSensorID;
}

void XSensorController::CheckEnteredSensor()
{
	if(!m_listEnteredSensorID.empty())
	{
		//mlog("EnterSensorID : ");

		for (list<uint32>::iterator it = m_listEnteredSensorID.begin();
			it != m_listEnteredSensorID.end(); ++it)
		{
			uint32 nSensorID = (*it);
			//mlog("%d, ", nSensorID);

			if (m_pManager->Find(nSensorID) == NULL)
			{
				m_listEnteredSensorID.erase(it);
				break;
			}
		}

		//mlog("\n");
	}
}