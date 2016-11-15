#include "StdAfx.h"
#include "XMagicAreaEffect.h"
#include "XWorld.h"
#include "XGame.h"
#include "XEffectInvoker.h"
#include "XEffectManager.h"
#include "XBuffInfo.h"
#include "CSEffectInfo.h"

XMagicAreaEffect::XMagicAreaEffect()
:m_pDebugDecalEffect(NULL)
, m_pObject(NULL)
{
	m_vCenter.Set(vec3::ZERO);
	m_vDir.Set(vec3::AXISY);
	m_fSize	= 1.0f;
	m_bDebug = false;
}

XMagicAreaEffect::~XMagicAreaEffect()
{

}

void XMagicAreaEffect::Create( XObject* pMagicArea, XBuffInfo* pBuffInfo, vec3 vCenter, vec3 vDir )
{
	m_vCenter			= vCenter;
	m_vDir				= vDir;
	m_fSize				= pBuffInfo->m_TriggeredBuff.infoEffect.m_fRadius;

	m_pObject			= pMagicArea;

	// 사이즈가 최소한 1보다 작을 수 없다.
	if(m_fSize < 1.0f)
		m_fSize = 1.0f;

	if(m_vecDecalEffectData.empty() == false)
		return;

	// 버프 이펙트 정보 가져오기
	vector<XBuffEffectInfo *> vecEffectInfoByType;
	pBuffInfo->m_BuffEffect.GetEffectInfoList(BUFF_GAIN_EFFECT, vecEffectInfoByType);

	for(vector<XBuffEffectInfo *>::iterator itBuffEffect = vecEffectInfoByType.begin(); itBuffEffect != vecEffectInfoByType.end(); ++itBuffEffect)
	{
		vector<EFFECT_RESULT_DATA> vecResult;
	
		// 데칼이 있을거 같아서 무조건 입력한다.
		// 데칼이 있다면 쓸것이다. 그외는 안쓰인다.
		EFFECT_POST_TRANSFORM postData;
		postData.vDecalSize = vec3(m_fSize, m_fSize, XCONST::DECALEFFECT_DEPTH_VALUE);

		//XEffectInvoker decalInvoker;
		//decalInvoker.Invoke(m_pObject, (*itBuffEffect)->m_strEffectName, wstring(), wstring(), &vecResult, NULL, &postData);
		XEffectSinglyInvoker decalInvoker;
		decalInvoker.Invoke((*itBuffEffect)->m_strEffectName, m_vCenter, m_vDir, vec3(vec3::AXISZ), &vecResult, &postData);

		for(vector<EFFECT_RESULT_DATA>::iterator itUID = vecResult.begin(); itUID != vecResult.end(); itUID++)
		{
			XEffectBase* pEffect = global.emgr->GetEffect((*itUID).uidEffect);
			if(pEffect && pEffect->IsEffectLoop())
			{
				m_vecDecalEffectData.push_back(*itUID);
			}
			else if(itUID->type == CET_SOUND)
			{
				m_vecDecalEffectData.push_back(*itUID);
			}
		}
	}
}

void XMagicAreaEffect::Update( float fDelta )
{
	// 디버그
	DebugMagicArea();
}

void XMagicAreaEffect::Destroy()
{
	if(m_pDebugDecalEffect)
	{
		m_pDebugDecalEffect->RemoveFromParent();
		SAFE_DELETE(m_pDebugDecalEffect);
	}

	for(vector<EFFECT_RESULT_DATA>::iterator it = m_vecDecalEffectData.begin(); it != m_vecDecalEffectData.end(); ++it)
	{
		if(it->type == CET_MODEL ||
			it->type == CET_DECALEFFECT)
			global.emgr->DeleteXEffect(it->uidEffect);
		else if(it->type == CET_SOUND)
		{
			if ( global.sound)
				global.sound->StopDynamic3D( it->uidEffect);
		}
	}

	m_vecDecalEffectData.clear();
}

void XMagicAreaEffect::DebugMagicArea()
{
	if(m_bDebug)
	{
		if (m_pDebugDecalEffect == NULL)
		{
			rs3::RCollisionTree* pCollisionTree = (rs3::RCollisionTree*)gg.currentgamestate->GetWorld()->GetCollisionTree();
			m_pDebugDecalEffect = new RDecalProjectionSceneNode;
			static_cast<RDecalProjectionSceneNode*>(m_pDebugDecalEffect)->Create(pCollisionTree, "magic_circle_1.DDS", m_vCenter, m_fSize, m_fSize, m_fSize);
			global.smgr->AddSceneNode(m_pDebugDecalEffect);	
		}
	}
	else
	{
		if(m_pDebugDecalEffect)
		{
			m_pDebugDecalEffect->RemoveFromParent();
			SAFE_DELETE(m_pDebugDecalEffect);
		}
	}
}

void XMagicAreaEffect::CheckRemainTimeEffect( float fRemainTime )
{
	vector<EFFECT_RESULT_DATA>::iterator it = m_vecDecalEffectData.begin();
	while(it != m_vecDecalEffectData.end())
	{
		if(it->type == CET_MODEL ||
			it->type == CET_DECALEFFECT)
		{
			XEffectBase* pEffect = global.emgr->GetEffect(it->uidEffect);
			if(pEffect == NULL)
			{
				it = m_vecDecalEffectData.erase(it);
				continue;
			}
			else if(pEffect->GetEndVisibilityTime() > fRemainTime)
			{
				global.emgr->DeleteXEffect(it->uidEffect);
				it = m_vecDecalEffectData.erase(it);
				continue;
			}
		}

		++it;
	}
}