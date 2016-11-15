#include "StdAfx.h"
#include "TSwordTrailSampling.h"
#include "RAnimationResource.h"
#include "RRenderHelper.h"
#include "TNaming.h"

//////////////////////////////////////////////////////////////////////////
TSwordTrailEffect::TSwordTrailEffect()
{

}

TSwordTrailEffect::~TSwordTrailEffect()
{
}


//////////////////////////////////////////////////////////////////////////
TSwordTrailSampling::TSwordTrailSampling()
{

}

TSwordTrailSampling::~TSwordTrailSampling()
{

}

void TSwordTrailSampling::StartSwordTrailSampling( string strModelName, RAnimation* pAnimation )
{
	if(pAnimation == NULL || pAnimation->GetResource() == NULL || strModelName == "")
		return;

	// 삭제
	SWORDTRAIL_MODEL* pSwordTrail_Model = NULL;
	map<string, SWORDTRAIL_MODEL>::iterator itModelFind = m_mapSwordTrailSamplingModel.find(strModelName);
	if(itModelFind != m_mapSwordTrailSamplingModel.end())
	{
		pSwordTrail_Model = &(itModelFind->second);
		map<string, SWORDTRAIL_DATA>::iterator itDataFind = itModelFind->second.mapSwordTrailSamplingData.find(pAnimation->GetAliasName());
		if(itDataFind != itModelFind->second.mapSwordTrailSamplingData.end())
		{
			// 지우고 밑에서 새로 생성한다.
			itModelFind->second.mapSwordTrailSamplingData.erase(itDataFind);
		}
	}
	else
	{
		SWORDTRAIL_MODEL newSwordTailModel;
		newSwordTailModel.strModelName = strModelName;

		m_mapSwordTrailSamplingModel.insert(map<string, SWORDTRAIL_MODEL>::value_type(strModelName, newSwordTailModel));

		map<string, SWORDTRAIL_MODEL>::iterator itModelFind2 = m_mapSwordTrailSamplingModel.find(strModelName);
		pSwordTrail_Model = &(itModelFind2->second);
	}

	m_SwordTrailEffectController.DestroySwordTrailEffect();

	// 생성
	//vector<SWORDTRAILEVENT_DATA> vecSwordTrailEvent_data;
	//CheckSwordTrailEvent(pAnimation, vecSwordTrailEvent_data);

	//if(vecSwordTrailEvent_data.empty())
	//	return;

	SWORDTRAIL_DATA newSwordTrailData;
	newSwordTrailData.strAnimationName = pAnimation->GetAliasName();
	//for(vector<SWORDTRAILEVENT_DATA>::iterator it = vecSwordTrailEvent_data.begin(); it != vecSwordTrailEvent_data.end(); it++)
	{
		SWORDTRAILSAMPLING_DATA samplingData;
		//samplingData.nStartFrame	= (*it).nStartFrame;
		//samplingData.nEndFrame		= (*it).nEndFrame;
		//samplingData.nFrameGap		= (*it).nFrameGap;
		samplingData.nStartFrame	= 0;
		samplingData.nEndFrame		= pAnimation->GetResource()->GetMaxFrame();
		samplingData.nFrameGap		= 100;

		SwordTrailSampling(pAnimation, samplingData, strModelName, "");

		newSwordTrailData.vecSamplingData.push_back(samplingData);
	}

	m_SwordTrailEffectController.CreateSwordTrailEffect(&newSwordTrailData);
	pSwordTrail_Model->mapSwordTrailSamplingData.insert(map<string, SWORDTRAIL_DATA>::value_type(pAnimation->GetAliasName(), newSwordTrailData));
}

void TSwordTrailSampling::CheckSwordTrailEvent( RAnimation* pAnimation, vector<SWORDTRAILEVENT_DATA>& vecData )
{
	if(pAnimation == NULL)
		return;

	vector<RAnimationEvent*> vecPushAnimationEvent;
	SWORDTRAILEVENT_DATA event_data;
	bool bStart = false;
	bool bEnd = true;
	string strID;
	for(vector<RAnimationEvent*>::iterator it = pAnimation->m_animationEvents.begin(); it != pAnimation->m_animationEvents.end(); it++)
	{
		if((*it)->m_strEvent == "sh_sword_trail_start")
		{
			bStart = true;

			// 앞에 있던 이벤트가 끝나지 않았다면...
			if(bEnd == false)
			{
				System::Windows::Forms::MessageBox::Show(L"검광 종료 이벤트가 없습니다. 자동 생성합니다.");

				RAnimationEvent* pAnimationEvent = new RAnimationEvent;
				pAnimationEvent->m_strEvent = (*it)->m_strEvent;
				pAnimationEvent->m_strParam3 = (*it)->m_strParam3;
				int nEndFrame;
				if(CalSwordTrailFrame(event_data.nStartFrame, (*it)->m_nFrame, event_data.nFrameGap, pAnimation->GetResource()->GetMaxFrame(), nEndFrame))
				{
					pAnimationEvent->m_nFrame = nEndFrame;
					event_data.nEndFrame = nEndFrame;
					vecData.push_back(event_data);
					vecPushAnimationEvent.push_back(pAnimationEvent);
				}
			}

			int nFrameGap = atoi((*it)->m_strParam2.c_str());
			if(nFrameGap < 1)
				nFrameGap = 1;
			if(nFrameGap > 9)
				nFrameGap = 9;

			event_data.init();
			event_data.nStartFrame	= (*it)->m_nFrame;
			event_data.nFrameGap	= nFrameGap * 50 + 50;
			event_data.strParentsBoneName = (*it)->m_strParam1;

			bEnd = false;
			strID = (*it)->m_strParam3;
		}

		if((*it)->m_strEvent == "sh_sword_trail_end")
		{
			if(bStart)
			{
				bStart = false;
				bEnd = true;

				int nCalFrame;
				if(CalSwordTrailFrame(event_data.nStartFrame, (*it)->m_nFrame, event_data.nFrameGap, pAnimation->GetResource()->GetMaxFrame(), nCalFrame) == false)
				{
					// 문제가 있다.
					continue;
				}

				if(nCalFrame != (*it)->m_nFrame)
				{
					if (System::Windows::Forms::MessageBox::Show("검광 이펙트가 잘 나오기 위해서 검광 종료 프레임을 자동 조정하겠습니까?", "안내"
						,MessageBoxButtons::YesNo) == System::Windows::Forms::DialogResult::Yes)
					{
						(*it)->m_nFrame = nCalFrame;
					}
				}

				event_data.nEndFrame = (*it)->m_nFrame;
				vecData.push_back(event_data);

				event_data.init();
			}
		}
	}

	// 이벤트 끝이 없다면 애니메이션 마지막 프레임이 끝이다.
	if(bEnd == false)
	{
		event_data.nEndFrame = pAnimation->GetResource()->GetMaxFrame();

		System::Windows::Forms::MessageBox::Show(L"검광 종료 이벤트가 없습니다. 자동 생성합니다.");

		RAnimationEvent* pAnimationEvent = new RAnimationEvent;
		pAnimationEvent->m_strEvent = "sh_sword_trail_end";
		pAnimationEvent->m_strParam3 = strID;
		int nEndFrame;
		if(CalSwordTrailFrame(event_data.nStartFrame, event_data.nEndFrame, event_data.nFrameGap, pAnimation->GetResource()->GetMaxFrame(), nEndFrame))
		{
			pAnimationEvent->m_nFrame = nEndFrame;
			event_data.nEndFrame = nEndFrame;
			vecPushAnimationEvent.push_back(pAnimationEvent);
		}

		vecData.push_back(event_data);
	}

	// 추가된 이벤트
	for(vector<RAnimationEvent*>::iterator itPush = vecPushAnimationEvent.begin(); itPush != vecPushAnimationEvent.end(); itPush++)
	{
		pAnimation->m_animationEvents.push_back(*itPush);

		GlobalObjects::g_SaveNotifier.SetSaveAnimation(true);
	}

	vecPushAnimationEvent.clear();
}

void TSwordTrailSampling::SwordTrailSampling( RAnimation* pAnimation, SWORDTRAILSAMPLING_DATA& sampling_data, string strModelName, string strParentsBoneName )
{
	if(pAnimation == NULL)
		return;

	int nMaxFrame = pAnimation->GetResource()->GetMaxFrame();
	if(sampling_data.nEndFrame > nMaxFrame)
		sampling_data.nEndFrame = nMaxFrame;

	string strSamplingName = strParentsBoneName;
	if(strModelName == HUMAN_FEMALE ||
		strModelName == HUMAN_MALE)
		strSamplingName = GetWeaponParentDummyName(pAnimation);

	RMatrix matActor;
	matActor.MakeIdentity();
	GetSamplingData(strSamplingName.c_str(), sampling_data.nStartFrame, sampling_data.nEndFrame, sampling_data.nFrameGap, sampling_data.vecMatrix, &matActor);
}

string TSwordTrailSampling::GetWeaponParentDummyName( RAnimation* pAnimation )
{
	string strWeaponName = pAnimation->GetAliasName().substr(0, pAnimation->GetAliasName().find("_"));

	if(strWeaponName == "2hd")
	{
		strWeaponName = "2hs";
	}

	// 1hb를 1hs랑 동일하게 쓰이기로 했음
	if(strWeaponName == "1hb")
		strWeaponName = "1hs";

	string strName = "dummy_" + strWeaponName;
	if(strWeaponName == "1hs" ||
		strWeaponName == "1hp")
	{
		strName += "_r";
	}

	return strName;
}

bool TSwordTrailSampling::Save()
{
	MXml xml;

	MXmlDeclaration* pDec = xml.CreateDeclaration("1.0", "UTF-8", "");
	xml.LinkEndChild(pDec);

	MXmlElement *pMaietElement = new MXmlElement(TALENT_XML_TAG_MAIET);

	// 샘플링 데어터 저장
	for(map<string, SWORDTRAIL_MODEL>::iterator itSave = m_mapSwordTrailSamplingModel.begin(); itSave != m_mapSwordTrailSamplingModel.end(); ++itSave)
	{
		MXmlElement* pSwordTrailModelElement = new MXmlElement(SWORD_TRAIL_MODEL);
		_SetAttribute(pSwordTrailModelElement, SWORD_TRAIL_NAME, itSave->second.strModelName);

		for(map<string, SWORDTRAIL_DATA>::iterator itSaveData = itSave->second.mapSwordTrailSamplingData.begin(); itSaveData != itSave->second.mapSwordTrailSamplingData.end(); itSaveData++)
		{
			SaveSwordTrailData(pSwordTrailModelElement, itSaveData->second);
		}

		pMaietElement->LinkEndChild(pSwordTrailModelElement);
	}

	xml.LinkEndChild(pMaietElement);

	return xml.SaveFile(FILENAME_SWORD_TRAIL_SAMPLING_TOOL);	
	return true;
}

void TSwordTrailSampling::SaveSwordTrailData( MXmlElement * pParentElement, SWORDTRAIL_DATA & swordTrail_data )
{
	MXmlElement *pSwordTrailDataElement = new MXmlElement(SWORD_TRAIL_DATA);

	_SetAttribute(pSwordTrailDataElement, SWORD_TRAIL_ANIMATION_NAME, swordTrail_data.strAnimationName);

	for(vector<SWORDTRAILSAMPLING_DATA>::iterator itdata = swordTrail_data.vecSamplingData.begin(); itdata != swordTrail_data.vecSamplingData.end(); itdata++)
	{
		SaveSwordTrailSamplingData(pSwordTrailDataElement, (*itdata));
	}

	pParentElement->LinkEndChild(pSwordTrailDataElement);
}

void TSwordTrailSampling::SaveSwordTrailSamplingData( MXmlElement * pParentElement, SWORDTRAILSAMPLING_DATA & swordTrailSampling_data )
{
	MXmlElement *pSwordTrailSamplingDataElement = new MXmlElement(SWORD_TRAIL_SAMPLINGDATA);

	_SetAttribute(pSwordTrailSamplingDataElement, SWORD_TRAIL_START_FRAME, swordTrailSampling_data.nStartFrame);
	_SetAttribute(pSwordTrailSamplingDataElement, SWORD_TRAIL_END_FRAME, swordTrailSampling_data.nEndFrame);
	_SetAttribute(pSwordTrailSamplingDataElement, SWORD_TRAIL_FRAME_GAP, swordTrailSampling_data.nFrameGap);

	for(vector<RMatrix>::iterator itSampling = swordTrailSampling_data.vecMatrix.begin(); itSampling != swordTrailSampling_data.vecMatrix.end(); itSampling++)
	{
		MXmlElement *pTransform = new MXmlElement(SWORD_TRAIL_SAMPLING_MATRIX);

		char szBuffer[512];
		szBuffer[0] = '\0';
		sprintf_s(szBuffer,"%f %f %f %f %f %f %f %f %f %f %f %f",
					itSampling->_11, itSampling->_12, itSampling->_13, 
					itSampling->_21, itSampling->_22, itSampling->_23,
					itSampling->_31, itSampling->_32, itSampling->_33,
					itSampling->_41, itSampling->_42, itSampling->_43);
		_SetContents(pTransform, szBuffer);

		pSwordTrailSamplingDataElement->LinkEndChild(pTransform);
	}

	pParentElement->LinkEndChild(pSwordTrailSamplingDataElement);	
}

void TSwordTrailSampling::GetSamplingData( const char* pNodeName, int nStartFrame, int nEndFrame, int nSampleInterval, std::vector< RMatrix >& _rOUt, const RMatrix* pBaseCoordIfUse )
{
	int nNodeTreeCount = 0;

	vector<RMatrix> ResultSamplingData;
	vector<RMatrix> SamplingData;

	int nPreFrame = nStartFrame - nSampleInterval;
	if(nPreFrame < 0)
		nPreFrame = 0;
	int nNextFrame = nEndFrame + nSampleInterval;

	g_pMainApp->GetActor()->GetSamplingActorNodeData(pNodeName, nPreFrame, nNextFrame, nSampleInterval, ResultSamplingData);

	int nDateCount = (int)ResultSamplingData.size();
	RMatrix matTemp;
	pBaseCoordIfUse->GetInverse(&matTemp);
	for(int i = 0; i < nDateCount; i++)
	{
		RMatrix matMove = *pBaseCoordIfUse;

		_rOUt.push_back(ResultSamplingData[i] * matTemp * matMove);
	}
}

void TSwordTrailSampling::DeleteSwordTrailSampling( string strModelName, RAnimation* pAnimation )
{
	if(pAnimation == NULL)
		return;

	map<string, SWORDTRAIL_MODEL>::iterator itModelFind = m_mapSwordTrailSamplingModel.find(strModelName);
	if(itModelFind != m_mapSwordTrailSamplingModel.end())
	{
		map<string, SWORDTRAIL_DATA>::iterator itDataFind = itModelFind->second.mapSwordTrailSamplingData.find(pAnimation->GetAliasName());
		if(itDataFind != itModelFind->second.mapSwordTrailSamplingData.end())
		{
			itModelFind->second.mapSwordTrailSamplingData.erase(itDataFind);
		}
	}
}

void TSwordTrailSampling::UpdateSwordTrailSampling( float fDelta )
{
	if(g_pMainApp->GetActor() == NULL ||
		g_pMainApp->GetActor()->GetCurAnimation() == NULL)
		return;

	string strModelName = TNaming::GetMeshAliasName(g_pMainApp->GetActor());
	string strAnimatioName = g_pMainApp->GetActor()->GetCurAnimation()->GetAliasName();
	SWORDTRAIL_DATA* pSwordData = Get(strModelName, strAnimatioName);

	if(pSwordData == NULL)
	{
		m_SwordTrailEffectController.DestroySwordTrailEffect();
		return ;
	}

	if(m_SwordTrailEffectController.CheckSwordTrailEffectAnimation(strAnimatioName) == false)
	{
		m_SwordTrailEffectController.DestroySwordTrailEffect();
		m_SwordTrailEffectController.CreateSwordTrailEffect(pSwordData);
	}

	m_pCurrentSwordTrail = NULL;

	int nCurrFrame = g_pMainApp->GetActor()->GetAnimationFrame();
	m_SwordTrailEffectController.UpdateSwordTrailEffect(fDelta, nCurrFrame, pSwordData);
}


bool TSwordTrailSampling::CalSwordTrailFrame( int nStartFrame, int nEndFrame, int nFrameGap, int nAnimationEndFrame, int& nCalEndFrame )
{
	nCalEndFrame = nEndFrame;

	// 문제 있어...
	if(nStartFrame > nEndFrame ||
		nFrameGap <= 0)
		return false;

	int nFrameGapCount	= (nEndFrame - nStartFrame) / nFrameGap;
	int nNewEndFrame	= nStartFrame + nFrameGapCount * nFrameGap;
	int nRemainFrame	= nEndFrame - nNewEndFrame;

	float fRate			= (float)nRemainFrame / (float)nFrameGap;
	if(fRate >= 0.5f)	
		nFrameGapCount++;

	nCalEndFrame		= nStartFrame + nFrameGapCount * nFrameGap;

	if(nCalEndFrame > nAnimationEndFrame)
		nCalEndFrame	= nStartFrame + (nFrameGapCount - 1) * nFrameGap;

	return true;
}

void TSwordTrailSampling::RenderSwordTrailSampling()
{
	m_SwordTrailEffectController.RenderSwordTrailEffect();
}

void TSwordTrailSampling::DestroySwordTrailSampling()
{
	m_SwordTrailEffectController.DestroySwordTrailEffect();
}
