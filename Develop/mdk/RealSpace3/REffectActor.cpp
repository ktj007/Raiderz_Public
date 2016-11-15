#include "stdafx.h"
#include "MProfiler.h"

#include "RActor.h"
#include "RActorAlignController.h"
#include "RRotateSettingController.h"
#include "REffectActor.h"
#include "RAnimationResource.h"
#include "RAnimation.h"
#include "RMesh.h"
#include "RXMLTemplate.h"
#include "RPostEffectDistortionParameter.h"

namespace rs3 {

MImplementRTTI(REffectActor, RActor);
MImplementRTTI(REffectActorMLeaf, REffectActor);

REffectActor::REffectActor(RRENDERPASS eRenderPass) : RActor(eRenderPass)
, m_TransformType(ETT_NONE)
, m_pTransformController(NULL)
, m_fStartTime(0.f)
, m_fElapsedAfterStart(0.0f)
, m_AniLoopType(RAniLoopType_HoldLastFrame)
, m_ePlayAnimationState(RPS_STOP)
, m_bVisibleSwitch(true)
, m_pParentEffectSceneNode(NULL)
, m_nRenderOrder(0)
, m_pEffectAnimation(NULL)
{
	SetVisible(false);
	SetAutoUpdate(true);
}

REffectActor::~REffectActor()
{
	RemoveTransformController();
}

void REffectActor::SetAlignController()
{
	RemoveTransformController();

	m_pTransformController = new RActorAlignController(this);
	_ASSERT(m_pTransformController);
	AddController(m_pTransformController);
	m_TransformType = ETT_ALIGN;
}

void REffectActor::SetAlignZFixedController()
{
	RemoveTransformController();

	m_pTransformController = new RActorZFixedAlignController(this);
	_ASSERT(m_pTransformController);
	AddController(m_pTransformController);
	m_TransformType = ETT_ALIGN_ZFIXED;
}

void REffectActor::SetRotateXController( float fRotateMin, float fRotateMax )
{
	RemoveTransformController();

	m_pTransformController = new RRotateSettingController(RVector::AXISX, fRotateMin, fRotateMax);
	_ASSERT(m_pTransformController);
	AddController(m_pTransformController);
	m_TransformType = ETT_ROTATE_X;
}

void REffectActor::SetRotateYController( float fRotateMin, float fRotateMax )
{
	RemoveTransformController();

	m_pTransformController = new RRotateSettingController(RVector::AXISY, fRotateMin, fRotateMax);
	_ASSERT(m_pTransformController);
	AddController(m_pTransformController);
	m_TransformType = ETT_ROTATE_Y;
}

void REffectActor::SetRotateZController( float fRotateMin, float fRotateMax )
{
	RemoveTransformController();

	m_pTransformController = new RRotateSettingController(RVector::AXISZ, fRotateMin, fRotateMax);
	_ASSERT(m_pTransformController);
	AddController(m_pTransformController);
	m_TransformType = ETT_ROTATE_Z;
}

bool REffectActor::GetRotateMinMax( float& fOutMin, float& fOutMax )
{
	if( IsRotateX() || IsRotateY() || IsRotateZ() )
	{
		RRotateSettingController* pController = static_cast<RRotateSettingController*>(m_pTransformController);
		fOutMin = pController->GetRotateMin();
		fOutMax = pController->GetRotateMax();
		return true;
	}

	fOutMin = 0.f;
	fOutMax = 0.f;
	return false;
}

void REffectActor::EnableTransformController()
{
	if(m_pTransformController)
	{
		m_pTransformController->Enable(true);
	}
}

void REffectActor::RemoveTransformController()
{
	if(m_pTransformController)
	{
		if(m_pTransformController->IsEnable())
			RemoveController(m_pTransformController);

		SAFE_DELETE(m_pTransformController);
	}
	m_TransformType = ETT_NONE;
}

void REffectActor::OnUpdate()
{
	RPFC_THISFUNC;

	if (m_fElapsedAfterStart < m_fStartTime)
	{
		float fElapsed = REngine::GetTimer().GetElapsedTime() / 1000.0f;
		m_fElapsedAfterStart += fElapsed;
		return;
	}

	// 로딩이 끝나지 않은 애니메이션이 로딩완료되면 PLAY한다.
	if( m_ePlayAnimationState == RPS_PLAY && GetAnimationController().GetState() != RPS_PLAY )
	{
		if( m_pEffectAnimation && m_pEffectAnimation->GetResource()->IsLoadingCompleted() )
			SetAnimation( m_pEffectAnimation );
	}

	// AutoUpdate 여부 설정
	// 부모가 있으면 항상 AutoUpdate 해준다.
	m_AutoUpdateContext.m_bMustAutoUpdate = (GetParent() != NULL);

	RActor::OnUpdate();
}

void REffectActor::OnLoadingComplete(RResource* pResource)
{
	// 로딩 성공
	if( pResource == m_pMesh && pResource->IsLoadingSucceeded() )
	{
		SetPosition(RVector(0,0,0));

		// 애니메이션 로딩
		if( m_pEffectAnimation == NULL )
		{
			string strAnimationName( MGetPureFileName(m_pMesh->GetName()) );
			strAnimationName += "EFFECT_ANI";
			RAnimation* pAnimation = m_pMesh->GetAnimationMgr()->GetAnimation(strAnimationName.c_str());
			if( pAnimation == NULL )
			{
				string anifilename = m_pMesh->GetName() + ".ani";
				m_pEffectAnimation = m_pMesh->GetAnimationMgr()->Add(strAnimationName.c_str(), anifilename.c_str(), m_pMesh->IsBackgroundCreated(), false, NULL );
				SetLoopType( m_AniLoopType );
			}
			else
			{
				m_pEffectAnimation = pAnimation;
			}
		}
	}

	RActor::OnLoadingComplete( pResource );
}

bool REffectActor::IsAlive()
{
	// 루프중에서
	if( IsLooping() )
	{
		if( m_ePlayAnimationState != RPS_STOP )
			return true;
	}
	// 루프가 아닌것 중에서 플레이 완료가 안되었다면 아직 죽지 말아야할것
	else
	if( GetAnimationController().GetState() == RPS_PLAY && IsPlayDone() == false )
	{
		return true;
	}

	return false;
}

bool REffectActor::IsLooping()
{
	if( m_AniLoopType == RAniLoopType_Loop )
		return true;

	return false;
}

void REffectActor::SetLooping(bool bLoop)
{
	if( bLoop )
	{
		SetLoopType( RAniLoopType_Loop );
		m_AniLoopType = RAniLoopType_Loop;
	}
	else
	{
		SetLoopType( RAniLoopType_HoldLastFrame );
		m_AniLoopType = RAniLoopType_HoldLastFrame;
	}
}

void REffectActor::EffectStart()
{
	if( GetVisibleSwitch() && GetVisibility() > 0.01f )
		SetVisible(true);

	m_ePlayAnimationState = RPS_PLAY;

	EnableTransformController();
	ResetStartElapsedTime();
	SetAnimation( m_pEffectAnimation );
}

void REffectActor::EffectStop()
{
	m_ePlayAnimationState = RPS_STOP;

	// 이펙트 액터는 Stop되어도 프레임이 리셋되는 경우를 원하지 않는다.
	Pause();
	SetVisible(false);
}

void REffectActor::EffectPause()
{
	m_ePlayAnimationState = RPS_PAUSE;

	Pause();
}

void REffectActor::SetLoopType(AnimationLoopType aniType)
{
	if( m_pEffectAnimation )
		m_pEffectAnimation->SetAnimationLoopType(aniType);
}

RSceneNode* REffectActor::Clone()
{
	REffectActor* pNew = new REffectActorMLeaf;

	if( m_pTransformController )
	{
		if( IsAlign() )
		{
			pNew->SetAlignController();
		}
		else if( IsAlignZFixed() )
		{
			pNew->SetAlignZFixedController();
		}
		else if( IsRotateX() || IsRotateY() || IsRotateZ() )
		{
			float fRotateMin, fRotateMax;
			GetRotateMinMax( fRotateMin, fRotateMax );

			if( IsRotateX() )
				pNew->SetRotateXController( fRotateMin, fRotateMax );
			else if( IsRotateY() )
				pNew->SetRotateYController( fRotateMin, fRotateMax );
			else if( IsRotateZ() )
				pNew->SetRotateZController( fRotateMin, fRotateMax );
		}
	}

	pNew->m_AniLoopType = m_AniLoopType;

	m_pMesh->AddRef();
	pNew->Create(m_pMesh);

	pNew->SetScale(GetScale() );

	pNew->SetStartTime( GetStartTime() );
	pNew->SetNodeName( GetNodeName() );

	pNew->SetMultiplyColor(GetMultiplyColor());

	pNew->SetDistortion( IsDistortion() );
	if( IsDistortion() )
	{
		pNew->SetDistortionParam(GetDistortionParam());
	}

	pNew->SetVisibleSwitch( GetVisibleSwitch() );

	return pNew;
}

int REffectActor::CalcPriority()
{
	return RBGWK_HIGHEST;
}

bool REffectActor::IsLoadingCompleted()
{
	if( !RActor::IsLoadingCompleted() ) return false;

	// usable 이 아니면 아직 딸린 노드들도 만들어지지 않은 상태이다.
	if( !GetUsable() ) return false;

	// 딸린 노드들도 로딩이 끝나야 한다
	for ( RSceneNodeList::iterator i = m_children.begin(); i != m_children.end(); ++i)
	{
		RSceneNode* pNode = *i;
		if(pNode->GetGroupOwner()==this && pNode->QueryAttribute(RSNA_DO_NOT_SAVE))
		{
			if(!pNode->IsLoadingCompleted())
			{
				return false;
			}
		}
	}

	// 애니메이션 로딩중이어도 로딩중이다
	if( m_pEffectAnimation && !m_pEffectAnimation->GetResource()->IsLoadingCompleted() )
		return false;

	return true;
}

void REffectActor::SaveToXML(MXmlElement &element)
{
	using namespace TXML;

	RActor::SaveToXML(element);

	if( m_pTransformController )
	{
		if( IsAlign() )
		{
			MXmlElement *pElement = new MXmlElement("ALIGN");
			element.LinkEndChild(pElement);
		}
		else if( IsAlignZFixed() )
		{
			MXmlElement *pElement = new MXmlElement("ALIGN_Z");
			element.LinkEndChild(pElement);
		}
		else if( IsRotateX() || IsRotateY() || IsRotateZ() )
		{
			MXmlElement *pElement = new MXmlElement("ROTATE");
			element.LinkEndChild( pElement );

			if( IsRotateX() )
				_SetAttribute( pElement, "AXIS", "X" );
			else if( IsRotateY() )
				_SetAttribute( pElement, "AXIS", "Y" );
			else if( IsRotateZ() )
				_SetAttribute( pElement, "AXIS", "Z" );

			float fRotateMin, fRotateMax;
			GetRotateMinMax( fRotateMin, fRotateMax );

			_SetAttribute( pElement, "MIN", fRotateMin );
			_SetAttribute( pElement, "MAX", fRotateMax );
		}
	}

	if( RAniLoopType_Loop == m_AniLoopType )
	{
		MXmlElement *pElement = new MXmlElement("LOOP");
		element.LinkEndChild(pElement);
	}

	MXmlElement *pElement = new MXmlElement("STARTTIME");
	_SetContents( pElement, m_fStartTime );
	element.LinkEndChild(pElement);

	const RVector4& color = GetMultiplyColor();
	if( color != RVector4( 1.0f, 1.0f, 1.0f, 1.0f ) )
	{
		pElement = new MXmlElement("COLOR");

		char szBuffer[256];
		sprintf(szBuffer,"%f %f %f", color.r, color.g, color.b);
		_SetContents( pElement, szBuffer );
		element.LinkEndChild(pElement);
	}

	if( GetVisibleSwitch() == false )
	{
		MXmlElement *pVisibleElement = new MXmlElement("INVISIBLE");
		element.LinkEndChild(pVisibleElement);
	}
}

bool REffectActor::CreateFromXML(MXmlElement &element, bool bBackgroundCreation)
{
	using namespace TXML;

	if( element.FirstChildElement("ALIGN") )
	{
		SetAlignController();
	}
	else
	if( element.FirstChildElement("ALIGN_Z") )
	{
		SetAlignZFixedController();
	}
	else
	if( MXmlElement* pElement = element.FirstChildElement("ROTATE") )
	{
		float fRotateMin, fRotateMax;
		_Attribute( fRotateMin, pElement, "MIN", 0.f );
		_Attribute( fRotateMax, pElement, "MAX", 0.f );

		string strAxisType;
		_Attribute( strAxisType, pElement, "AXIS", "" );
		if( strAxisType == "X" )
			SetRotateXController( fRotateMin, fRotateMax );
		else if( strAxisType == "Y" )
			SetRotateYController( fRotateMin, fRotateMax );
		else if( strAxisType == "Z" )
			SetRotateZController( fRotateMin, fRotateMax );
	}

	if( element.FirstChildElement("LOOP") )
		m_AniLoopType = RAniLoopType_Loop;
	else
		m_AniLoopType = RAniLoopType_HoldLastFrame;

	MXmlElement *pElement = element.FirstChildElement("STARTTIME");
	if(pElement)
	{
		_Contents( &m_fStartTime, pElement );
	}

	pElement = element.FirstChildElement("COLOR");
	if(pElement)
	{
		char szBuffer[256];
		_Contents( szBuffer, pElement );

		RVector4 color;
		sscanf(szBuffer,"%f %f %f",&color.r,&color.g,&color.b);
		color.a = 1.0f;

		SetMultiplyColor( color );
	}

	if( MXmlElement *pVisible = element.FirstChildElement("INVISIBLE") )
	{ 
		SetVisibleSwitch( false );
	}

	//bool bRet = RActor::CreateFromXML(element, false);
	bool bRet = RActor::CreateFromXML(element, bBackgroundCreation);

	// 이펙트 액터는 오토애니메이션을 사용하지 않는다.
	m_bAutoAnimationEnable = false;

	return bRet;
}

}