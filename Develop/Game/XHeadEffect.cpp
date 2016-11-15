#include "stdafx.h"
#include "XHeadEffect.h"
#include "XNonPlayer.h"
#include "XEventID.h"
#include "XEffectManager.h"
#include "XCharacter.h"

void XHeadEffect::LootableEffect::CreateLootableEffect( XObject* pOwner )
{
	XCharacter* pActor = pOwner->GetActor();
	if(pActor == NULL)
		return;

	pActor->UpdateTransform();
	pActor->UpdateBoundingVolume();

	RActorNode* pActorNode = pActor->GetActorNode(MLocale::ConvUTF16ToAnsi(LOOT_EFFECT_BASE_DUMMY).c_str());
	RBoundingBox InterAABB = pActor->GetInteractionAABB();
	vec3 vCenter = InterAABB.GetCenter();

	// 본이 없는 맵오브젝트들은 InteractionAABB가 제대로 만들어지지 않는다.
	if (vCenter <= vec3::ZERO)
	{
		vCenter = pActor->GetAABB().GetCenter();

		RMatrix mat;
		pActor->GetWorldTransform().GetInverse(&mat);
		vCenter = vCenter * mat;
	}

	RMatrix matLocal;
	matLocal.MakeIdentity();

	if(pActorNode == NULL)
		matLocal.SetLocalMatrix(vCenter, vec3(1,0,0), vec3(0,0,1));

	m_uidEffect = global.emgr->AddXEffect(LOOTABLE_EFFECT_NAME, pActor, pActorNode, matLocal);
}

void XHeadEffect::LootableEffect::DeleteLootableEffect()
{
	global.emgr->DeleteXEffect(m_uidEffect);

	m_uidEffect = MUID::ZERO;
}

bool XHeadEffect::LootableEffect::CheckDuplicationLootableEffect( HEAD_EFFECT_TYPE eEffectType )
{
	if(eEffectType != HET_LOOTABLE)
		return false;

	if(m_uidEffect == MUID::ZERO)
		return false;

	return true;
}

//////////////////////////////////////////////////////////////////////////
void XHeadEffect::BPartNPCLootableEffect::CreateLootableEffect( XObject* pOwner )
{
	XCharacter* pActor = pOwner->GetActor();
	if(pActor == NULL)
		return;

	// 스케일 값 구하기
	RBoundingBox& actorBox = pActor->GetAABB();
	float fxLen = actorBox.maxx - actorBox.minx;
	float fyLen = actorBox.maxy - actorBox.miny;

	float fMaxLen = (fxLen > fyLen) ? fxLen : fyLen;
	float fScale = fMaxLen / (float)XCONST::BPART_LOOTABLE_EFFECT_MIN_SIZE;
	float fMaxScaleTest = (float)XCONST::BPART_LOOTABLE_EFFECT_MAX_SIZE / (float)XCONST::BPART_LOOTABLE_EFFECT_MIN_SIZE;

	// 스케일 1 이하는 적용하지 않는다.
	if(fScale < 1.0f)
		fScale = 1.0f;
	// Max 스케일 이상은 적용하지 않는다.
	else if(fScale > fMaxScaleTest)
		fScale = fMaxScaleTest;

	fScale *= XCONST::BPART_LOOTABLE_EFFECT_SCALE_RATIO;

	EFFECT_POST_TRANSFORM postData;
	postData.matWorldMatrix.SetTranslation(pActor->GetAABB().GetCenter());
	postData.vSize = vec3(fScale);

	XEffectInvoker effectInvoker;
	effectInvoker.Invoke(pOwner, wstring(XCONST::BPART_LOOTABLE_EFFECT_NAME), wstring(L""), wstring(L""), &m_vecEffectList, NULL, &postData);
}

void XHeadEffect::BPartNPCLootableEffect::DeleteLootableEffect()
{
	XEffectDelete::DelList(m_vecEffectList);

	m_vecEffectList.clear();
}

bool XHeadEffect::BPartNPCLootableEffect::CheckDuplicationLootableEffect( HEAD_EFFECT_TYPE eEffectType )
{
	if(eEffectType != HET_LOOTABLE)
		return false;

	if(m_vecEffectList.empty())
		return false;

	return true;
}

//////////////////////////////////////////////////////////////////////////
XHeadEffect::XHeadEffect(XObject* pOwner)
: m_pObject(pOwner)
, m_bActorLoadingComplete(false)
{
	m_vecHeadEffectList.clear();

	if(m_pObject->GetType() == XOBJ_BPART_NPC)
		m_pLootableEffect = new BPartNPCLootableEffect();
	else
		m_pLootableEffect = new LootableEffect();
}

XHeadEffect::~XHeadEffect()
{
	// 이펙트 전체 삭제
	DeleteAllHeadEffect();

	SAFE_DELETE(m_pLootableEffect);
}

void XHeadEffect::CreateHeadEffect( HEAD_EFFECT_TYPE eEffectType )
{
	if(m_pObject == NULL)
		return;
	
	// Actor 로딩 완료 상태인가?
	if(CheckActorLoadingComplete(eEffectType) == false)
		return;

	// 중복 사용은 안한다.
	if(CheckDuplicationHeadEffect(eEffectType) == false)
		return;

	// 붙일 노드를 찾는다.
	MUID uidEffect;
	XCharacter* pOutActor		= NULL;
	RActorNode* pOutActorNode	= NULL;

	GetHeadEffectBone(&pOutActor, &pOutActorNode);

	// 이펙트를 생성한다.
	switch(eEffectType)
	{
	case HET_STUN:
		uidEffect = CreateStunEffect(pOutActor, pOutActorNode);
		break;

	case HET_SLEEP:
		uidEffect = CreateSleepEffect(pOutActor, pOutActorNode);
		break;
	case HET_LOOTABLE:
		m_pLootableEffect->CreateLootableEffect(m_pObject);
		break;
	}

	if(uidEffect != MUID::ZERO)
	{
		// 이펙트 정보 보관
		stHeadEffectInfo stEffectInfo;
		stEffectInfo.uidEffect		= uidEffect;
		stEffectInfo.eEffectType	= eEffectType;

		m_vecHeadEffectList.push_back(stEffectInfo);
	}

	return;
}

MUID XHeadEffect::CreateStunEffect(XCharacter* pActor, RActorNode* pActorNode)
{
	// 스턴 이펙트
	MUID uidEffect = global.emgr->AddXEffect(STUN_EFFECT_NAME, pActor, pActorNode);
	
	return uidEffect;
}

MUID XHeadEffect::CreateSleepEffect(XCharacter* pActor, RActorNode* pActorNode)
{
	MUID uidEffect;
	return uidEffect;
}

void XHeadEffect::GetHeadEffectBone( XCharacter** pOutActor, RActorNode** pOutActoreNode )
{
	*pOutActor		= NULL;
	*pOutActoreNode	= NULL;

	XCharacter* pActor = m_pObject->GetActor();
	if(pActor == NULL)	return;

	// 1차는 HeadNub를 찾는다.
	RActorNode* pActorNode = pActor->GetActorNode(MLocale::ConvUTF16ToAnsi(HEAD_EFFECT_BASE_BONE).c_str());
	
	if(pActorNode == NULL)
	{
		// 2차는 더미를 찾는다.
		pActorNode = pActor->GetActorNode(MLocale::ConvUTF16ToAnsi(HEAD_EFFECT_BASE_DUMMY).c_str());
	}

	*pOutActor		= pActor;
	*pOutActoreNode	= pActorNode;
}

void XHeadEffect::DeleteHeadEffect( HEAD_EFFECT_TYPE eEffectType )
{
	// 루팅은 따로...
	if(HET_LOOTABLE == eEffectType)
	{
		m_pLootableEffect->DeleteLootableEffect();
		return;
	}

	vector<stHeadEffectInfo>::iterator it = m_vecHeadEffectList.begin();

	while(it != m_vecHeadEffectList.end())
	{
		if(eEffectType == (*it).eEffectType)
		{
			global.emgr->DeleteXEffect((*it).uidEffect);
			m_vecHeadEffectList.erase(it);
			break;
		}

		it++;
	}
}

void XHeadEffect::DeleteAllHeadEffect()
{
	vector<stHeadEffectInfo>::iterator it = m_vecHeadEffectList.begin();

	while(it != m_vecHeadEffectList.end())
	{
		global.emgr->DeleteXEffect((*it).uidEffect);

		it++;
	}

	m_vecHeadEffectList.clear();

	m_pLootableEffect->DeleteLootableEffect();
}

bool XHeadEffect::CheckDuplicationHeadEffect( HEAD_EFFECT_TYPE eEffectType )
{
	if(m_pLootableEffect->CheckDuplicationLootableEffect(eEffectType))
		return false;

	vector<stHeadEffectInfo>::iterator it = m_vecHeadEffectList.begin();

	while(it != m_vecHeadEffectList.end())
	{
		if(eEffectType == (*it).eEffectType)
		{
			return false;
		}

		it++;
	}

	return true;
}

void XHeadEffect::Update( float fDelta )
{

}

void XHeadEffect::OnLoadingComplete()
{
	m_bActorLoadingComplete = true;

	for(set<HEAD_EFFECT_TYPE>::iterator it = m_setEffectType.begin(); it != m_setEffectType.end(); ++it)
	{
		CreateHeadEffect(*it);
	}

	m_setEffectType.clear();
}

bool XHeadEffect::CheckActorLoadingComplete( HEAD_EFFECT_TYPE eEffectType )
{
	if(m_bActorLoadingComplete)
		return true;

	XCharacter* pActor = m_pObject->GetActor();
	if(pActor &&
		pActor->m_pMesh &&
		pActor->CheckLoadingCompletedFastly() == false)
	{
		m_setEffectType.insert(eEffectType);
		return false;
	}

	return true;
}