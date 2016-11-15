#include "stdafx.h"
#include "TEffectCharacterToolSinglyInvoker.h"

TEffectCharacterToolSinglyInvoker::TEffectCharacterToolSinglyInvoker()
{
	m_pActor = NULL;
}

TEffectCharacterToolSinglyInvoker::~TEffectCharacterToolSinglyInvoker()
{

}

void TEffectCharacterToolSinglyInvoker::Invoke( string& strEffectName, vec3& vPos, vec3& vDir, vec3& vUp, float fSize /*= 0.0f*/, vector<EFFECT_RESULT_DATA>* vecOutData /*= NULL*/ )
{
	if(g_pMainApp->m_bShowEffect == false)
		return;

	XEffectInfo * pEffectInfo =  g_pMainApp->m_pEffectInfoMgr->GetEffectInfo(strEffectName);
	if(pEffectInfo)
	{
		// 입력된 정보가 없으면... 기본 정보 가져오기
		string strParam		= GetReserved_Param(strEffectName);

		float fEffectDurationTime = pEffectInfo->m_fEffectDurationTime;

		// 이벤트 발생
		for(vector<XBaseEffectInfoItem *>::iterator itEvent = pEffectInfo->m_vecEffectInfoItem.begin(); 
			itEvent != pEffectInfo->m_vecEffectInfoItem.end(); itEvent++)
		{
			map<CUSTOM_EFFECT_TYPE, EFFECTHANDLER>::iterator itFind = m_mapEffectHandlers.find((*itEvent)->eEffectType);
			if(itFind != m_mapEffectHandlers.end())
			{
				EFFECT_RESULT_DATA outData;
				EFFECT_TRANSEMISION_DATA transemisionData;
				transemisionData.pEffectInfoItem	= (*itEvent);
				transemisionData.strEventName		= strEffectName;
				transemisionData.matWorldMatrix.SetLocalMatrix(vPos, vDir, vUp);
				transemisionData.bWaterPos			= pEffectInfo->bWaterPos;
				transemisionData.vSize				= vec3(fSize, fSize, 0.0f);

				itFind->second(*this, transemisionData, string(""), strParam, fEffectDurationTime, outData);

				if(vecOutData &&
					outData.uidEffect != MUID::ZERO)
				{
					vecOutData->push_back(outData);
				}
			}
		}
	}
}

MUID TEffectCharacterToolSinglyInvoker::SetModel( EFFECT_TRANSEMISION_DATA& _data, XEffectInfoItem_Model& modelInfo, EFFECT_CREATE_RESULT_DATA& resultData )
{
	MUID uidEffect(MUID::ZERO);

	// 이펙트 이름
	if(!_data.strResourceName.empty())
	{
		vec3 vPos = vec3(_data.matWorldMatrix._41, _data.matWorldMatrix._42, _data.matWorldMatrix._43);
		vec3 vDir = vec3(_data.matWorldMatrix._21, _data.matWorldMatrix._22, _data.matWorldMatrix._23);
		vec3 vUp = vec3(_data.matWorldMatrix._31, _data.matWorldMatrix._32, _data.matWorldMatrix._33);

		if(_data.bWaterPos)
			vPos.z = SWIMMING_HEIGHT_VALUE_TEST;

		uidEffect = g_pMainApp->m_pEffectManager->AddTEffect(_data.strResourceName, vPos, vDir, vUp, modelInfo.bTerrainAlign, modelInfo.bTerrainPos, modelInfo.bLastFramePause, false, modelInfo.bImmediatlyDelete, false, modelInfo.fDurationTime, modelInfo.fStartVisibilityTime, modelInfo.fEndVisibilityTime);

		TEffect* pEffect = g_pMainApp->m_pEffectManager->GetTEffect(uidEffect);
		if(pEffect)
			resultData.bLoop = pEffect->IsEffectLoop();

		// 스케일값 적용
		g_pMainApp->m_pEffectManager->SetScaleTEffect(uidEffect, modelInfo.bAutoScale, modelInfo.vScale);
	}

	return uidEffect;
}

MUID TEffectCharacterToolSinglyInvoker::SetSound( EFFECT_TRANSEMISION_DATA& _data, XEffectInfoItem_Sound& soundInfo, EFFECT_CREATE_RESULT_DATA& resultData )
{
	if(_data.strResourceName.empty()) 
		return MUID::ZERO;

//	MUID uidSound = g_pMainApp->m_pSoundMgr.PlaySound(_data.strResourceName, _data.strBoneName);
//	return uidSound;

	//<<<rsound3
	MUID uid = g_pMainApp->m_pEffectManager->GenerateLocalUID();
	RSound* pSound = g_pMainApp->m_pSoundMgr.PlayDynamic3D( _data.strResourceName.c_str(), _data.strBoneName.c_str(), uid);
	if(pSound)
	{
		resultData.bLoop = pSound->GetLoop();
		return uid;
	}

	return MUID::ZERO;
}
