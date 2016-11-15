#include "stdafx.h"
#include "WS_CutSceneObjectEntityActor.h"

CCutSceneObjectEntityActor::CCutSceneObjectEntityActor(CControlMediator* pControlMediator, const std::string& _rStrType)
: CCutSceneObjectEntity(pControlMediator)
{
	m_pCutSceneActor_ = NULL;
	ResetEntityActorSceneNode(_rStrType.c_str());
}

CCutSceneObjectEntityActor::~CCutSceneObjectEntityActor()
{
	ResetEntityActorSceneNode(NULL);
}

void CCutSceneObjectEntityActor::ResetUpdate()
{
	if (m_pCutSceneActor_)
	{
		m_pCutSceneActor_->Stop();
	}
}

void CCutSceneObjectEntityActor::SetTransform(const MMatrix& mat, bool bUsingCollision)
{
	if (!m_pCutSceneActor_)
		return;

	vec3 vPos(mat._41, mat._42, mat._43);
	vec3 vDir(-mat._21, -mat._22, -mat._23);
	m_pCutSceneActor_->SetTransform(vPos, vDir);
	m_pCutSceneActor_->UpdateTransform();
}

void CCutSceneObjectEntityActor::SetAnimation(const std::string& _rAnimationName)
{
	if (m_pCutSceneActor_)
	{
		m_pCutSceneActor_->SetAnimation(_rAnimationName.c_str());
	}
}

rs3::RCameraSceneNode* CCutSceneObjectEntityActor::GetRelatedCamera()
{
	return m_pControlMediator_->GetToolCamera();
}

rs3::RSceneNode* CCutSceneObjectEntityActor::GetSceneNodeToRender()
{
	return m_pCutSceneActor_;
}

void CCutSceneObjectEntityActor::ResetEntityActorSceneNode(const char* _pSzActorType)
{
	using namespace rs3;
	if (m_pCutSceneActor_)
	{
		m_pCutSceneActor_->RemoveFromParent();
		delete m_pCutSceneActor_;
		m_pCutSceneActor_ = NULL;
	}

	if (NULL == _pSzActorType)
		return;

	int nId = XCutSceneObjectActor::GetActorIdFromEntityInfo(_pSzActorType);
	SNpcInfo* pNpcInfo = NULL;
	float fScale = 1.f;
	std::string strMeshName;
	if (nId > 0)
	{
		pNpcInfo = m_pControlMediator_->GetToolLogicObjectMgr()->GetNpcInfoFromId( nId );
		if (NULL == pNpcInfo)
		{
			CString message;
			message.Format("CutScene Actor Loading Error : %s", _pSzActorType);
			AfxMessageBox(message);
			return;
		}

		strMeshName = pNpcInfo->m_strNpcMeshName;
		fScale = pNpcInfo->m_fScale;

	}
	else
	{
		// player °æ¿ì
		strMeshName = CXMLConfig::GetInstance()->strDataFullPath + "Model\\Player\\hf\\hf.elu";
	}

	m_pCutSceneActor_ = (RActor*) REngine::GetSceneManager().CreateSceneNode( rs3::RSID_ACTOR );
	m_pControlMediator_->GetToolSceneManager()->LoadSceneNodeFromFile(m_pCutSceneActor_, strMeshName.c_str());
	m_pCutSceneActor_->SetScale(fScale);
}