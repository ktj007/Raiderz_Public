#include "StdAfx.h"
#include "M_LogicObject_SceneNodes.h"
#include "M_LogicObjectCommandBuffer.h"
#include "C_ControlSceneProperty.h"
#include "VG_LogicObjectRenderer.h"
#include "V_LogicObjectRenderer.h"
#include "RActor.h"
#include "RTreeSceneNode.h"
#include "RSceneNodeInstance.h"
#include "RStaticMeshSceneNode.h"
#include "RSpotLightSceneNode.h"
#include "ROccluderSceneNode.h"
#include "RRenderHelper.h"
#include "MFileSystem.h"
#include "M_Visitors.h"

using namespace rs3;

//////////////////////////////////////////////////////////////////////////
// CLogicObject_SceneNode
CLogicObject_SceneNode::CLogicObject_SceneNode()
: m_pModelNode(NULL)
, m_pPropertyReflection(NULL)
{

}

CLogicObject_SceneNode::~CLogicObject_SceneNode()
{
	UnlinkModelSceneNode();

	if(m_pPropertyReflection)
		delete m_pPropertyReflection;
}

void CLogicObject_SceneNode::UnlinkModelSceneNode()
{
	if(m_pModelNode)
	{
		m_pModelNode->SetUserData(NULL);
		m_pModelNode = NULL;
	}
}

void CLogicObject_SceneNode::LinkModelSceneNode( rs3::RSceneNode* pSceneNode )
{
	m_pModelNode = pSceneNode;

	// 이미 무엇인가 설정되어있다
	_ASSERT( pSceneNode->GetUserData() == NULL );
	pSceneNode->SetUserData( this );

	SetLayerName( FindUserProperty("TOOL_LAYER").c_str() );

	bool bVisible = strcmp( FindUserProperty("TOOL_VISIBLE").c_str() , "false" ) != 0;
	SetVisible( bVisible );

	bool bEditable = strcmp( FindUserProperty("TOOL_EDITABLE").c_str() , "false" ) != 0;
	SetEditable( bEditable );

	SetObjectName( pSceneNode->GetNodeName().c_str() );
}

void CLogicObject_SceneNode::OnSetObjectName(const char* szName)
{
	__super::OnSetObjectName(szName);

	m_pModelNode->SetNodeName( szName );
}

int CLogicObject_SceneNode::FindUserPropertyIndex( const char* szPropertyName )
{
	for( unsigned int i=0; i<m_pModelNode->GetUserPropertyCount(); i++ )
	{
		if( strcmp(szPropertyName, m_pModelNode->GetUserPropertyName(i).c_str() ) == 0 )
		{
			return i;
		}
	}
	return -1;
}

const string& CLogicObject_SceneNode::FindUserProperty( const char* szPropertyName )
{
	int nIndex = FindUserPropertyIndex( szPropertyName );
	if( nIndex == -1 )
	{
		static const string NullString = "";
		return NullString;
	}

	return m_pModelNode->GetUserPropertyValue(nIndex);
}

void CLogicObject_SceneNode::SetProperty(const char* szPropertyName, const char* szValue)
{
	int nIndex = FindUserPropertyIndex( szPropertyName );
	if( nIndex == -1 )
		// 없으면 추가
		m_pModelNode->PushUserProperty( szPropertyName, szValue );
	else
		m_pModelNode->SetUserPropertyValue( nIndex, szValue );
}

void CLogicObject_SceneNode::RemoveProperty(const char* szPropertyName)
{
	int nIndex = FindUserPropertyIndex( szPropertyName );
	if( nIndex != -1 )
		m_pModelNode->RemoveUserProperty( nIndex );
}

// 툴 속성들을 SceneNode의 userproperty 에 기록한다. 기본값은 기록안한다
void CLogicObject_SceneNode::OnSetLayerName(const char* szName)
{
	__super::OnSetLayerName(szName);
	if( szName && strlen(szName) > 0)
		SetProperty( "TOOL_LAYER", szName );
	else
		RemoveProperty( "TOOL_LAYER" );
}

void CLogicObject_SceneNode::OnSetVisible(bool bVisible)
{
	if( bVisible == false )
		SetProperty( "TOOL_VISIBLE", "false" );
	else
		RemoveProperty( "TOOL_VISIBLE" );
}

void CLogicObject_SceneNode::OnSetEditable(bool bEditable)
{
	if( bEditable == false )
		SetProperty( "TOOL_EDITABLE", "false" );
	else
		RemoveProperty( "TOOL_EDITABLE" );
}

bool CLogicObject_SceneNode::OnGetPos_(RVector& _refVecPos) const
{
	if(!m_pModelNode) return false;

	_refVecPos = m_pModelNode->GetWorldPosition();
	return true;
}

/// 움직임/회전이 있을때 자식노드들에 연결된 logic object 도 위치 갱신하도록 세팅
void CLogicObject_SceneNode::NotifyChildrenLogicObjects()
{
	class LogicNotifyVisitor : public RSceneNodeVisitor
	{
	public:
		LogicNotifyVisitor( CControlMediator* pController ) : _pController( pController ) {};

		virtual bool OnPreOrderVisit( RSceneNode* pSceneNode ) { return true; }
		virtual void OnPostOrderVisit( RSceneNode* pSceneNode )
		{
			CLogicObject_SceneNode* pLogicObject = dynamic_cast<CLogicObject_SceneNode*>( (CCoreObject*)pSceneNode->GetUserData() );
			if(pLogicObject)
				pLogicObject->NeedUpdate();
		}
	protected:
		CControlMediator* _pController ;
	} notifyVisitor( m_pController );

	if(m_pModelNode)
		m_pModelNode->Traverse(&notifyVisitor);
}

void CLogicObject_SceneNode::GatherLogicObjectsIncluding( std::list< CLogicObject* >& listObjects )
{
	class GatherChildVisitor : public RSceneNodeVisitor
	{
	public:
		GatherChildVisitor( RSceneNode* pCaller, std::list< CLogicObject* >& outListObject ) 
			: m_pCaller( pCaller )
			, m_listObjects( outListObject ) {};

		virtual bool OnPreOrderVisit( RSceneNode* pSceneNode ) { return true; }
		virtual void OnPostOrderVisit( RSceneNode* pSceneNode )
		{
			if( m_pCaller == pSceneNode ) return;

			CLogicObject_SceneNode* pLogicObject = dynamic_cast<CLogicObject_SceneNode*>( (CCoreObject*)pSceneNode->GetUserData() );
			if(pLogicObject)
				m_listObjects.push_back( pLogicObject );
		}
	protected:
		RSceneNode*					m_pCaller;
		std::list< CLogicObject* >& m_listObjects ;
	} gatherVisitor( m_pModelNode, listObjects );

	if(m_pModelNode)
		m_pModelNode->Traverse(&gatherVisitor);
}

void CLogicObject_SceneNode::OnSetPos_(const RVector &_refUpdatePos)
{
	if(m_pModelNode)
	{
		RVector vecLocalPosition = _refUpdatePos;
		if( m_pModelNode->GetParent() )
		{
			RMatrix matInverseParent;
			m_pModelNode->GetParent()->GetWorldTransform().GetInverse( &matInverseParent );
			matInverseParent.TransformVectHomogeneous(vecLocalPosition);
		}
		m_pModelNode->SetPosition( vecLocalPosition );
	}
}

bool CLogicObject_SceneNode::OnGetDir_(RVector& _refVecDir) const
{
	if(!m_pModelNode) return false;

	_refVecDir = m_pModelNode->GetDirection();
	return true;
}

bool CLogicObject_SceneNode::OnGetUp_(RVector& _refVecUp) const
{
	if(!m_pModelNode) return false;

	_refVecUp = m_pModelNode->GetUp();
	return true;
}

void CLogicObject_SceneNode::OnSetDirUp_(const RVector &_refUpdateDir, const RVector &_refUpdateUp)
{
	if(m_pModelNode)
	{
		m_pModelNode->SetDirection( _refUpdateDir, _refUpdateUp );

		NeedUpdate();
	}
}

bool CLogicObject_SceneNode::OnGetScale_(float& _refSize) const
{
	if( m_pModelNode )
	{
		_refSize = m_pModelNode->GetScale().x;
		return true;
	}
	else
		return false;
}

void CLogicObject_SceneNode::OnSetScale_(const float fUpdateRadius)
{
	if( m_pModelNode )
	{
		m_pModelNode->SetScale( fUpdateRadius );

	}
}

void CLogicObject_SceneNode::OnUpdate()
{
	// 자식노드 업데이트
	RSceneNodeUpdateVisitor visitor;
	m_pModelNode->Traverse(&visitor);

	// 자식 로직오브젝트 위치가 변경되므로 view 로 통보
	NotifyChildrenLogicObjects();
}


CLogicObject* CLogicObject_SceneNode::Clone()
{
	RSceneNode* pToNode = m_pModelNode->Clone();
	if ( pToNode == NULL ) return NULL;

	//////////////////////////////////////////////////////////////////////////
	// update
	RSceneNodeUpdateVisitor visitor;
	pToNode->Traverse(&visitor);

	RToolSceneManager* pSceneManager = m_pController->GetToolSceneManager();
	pSceneManager->AddSceneNode(pToNode, true);

	pSceneManager->CreateLogicObject(pToNode);

	CLogicObject_SceneNode* pToLogic = dynamic_cast<CLogicObject_SceneNode*>((CCoreObject*)pToNode->GetUserData());
	_ASSERT( pToLogic );
	CControlSceneProperty* pProperty = pToLogic->GetProperty();
	if(pProperty)
		pProperty->SetNewInstance(pToNode, m_pModelNode, true);


	return pToLogic;
}

ICommandBuffer* CLogicObject_SceneNode::CreateCloneNewCmdBuffer()
{
	return new CLogicObject_SceneNodeCloneNewCmdBuffer(this);;
}

ICommandBuffer* CLogicObject_SceneNode::CreateDeleteCmdBuffer()
{
	return new CLogicObject_SceneNodeDeleteCmdBuffer(this);
}

const char*	CLogicObject_SceneNode::GetTypeName()
{
	return m_pModelNode->GetNodeIDString();
}

int	CLogicObject_SceneNode::GetIconIndex()
{
	const int nSceneBaseIconIndex = 3; // coreplugin 등록시 nBaseImageListIndex

	if(stricmp(m_pModelNode->GetNodeIDString(),RSID_ACTOR)==0 )
		return nSceneBaseIconIndex;
	else if(stricmp(m_pModelNode->GetNodeIDString(),RSID_EFFECTINSTANCE)==0 )
		return nSceneBaseIconIndex+1;
	else if(stricmp(m_pModelNode->GetNodeIDString(),RSID_DIRECTIONALLIGHTSCENENODE)==0 ||
		stricmp(m_pModelNode->GetNodeIDString(),RSID_SUBLIGHTSCENENODE)==0)
		return nSceneBaseIconIndex+2;
	else if(stricmp(m_pModelNode->GetNodeIDString(),RSID_LIGHTSCENENODE)==0 )
		return nSceneBaseIconIndex+3;
	else if(stricmp(m_pModelNode->GetNodeIDString(),RSID_SPOTLIGHTSCENENODE)==0 )
		return nSceneBaseIconIndex+4;
	else if(stricmp(m_pModelNode->GetNodeIDString(),RSID_TRAIL)==0 )
		return nSceneBaseIconIndex+5;
	else if(stricmp(m_pModelNode->GetNodeIDString(),RSID_TREE)==0 )
		return nSceneBaseIconIndex+6;
	else if(stricmp(m_pModelNode->GetNodeIDString(),RSID_SCENEINSTANCE)==0 )
		return nSceneBaseIconIndex+7;

	return CLogicObject::GetIconIndex();
}


//////////////////////////////////////////////////////////////////////////
// 신노드가 보이지 않아서 대체 신노드로 렌더링되는 LogicObjectRenderer
class CLogicObjectRenderer_SceneNodeProxy : public CLogicObjectRenderer_SceneNode
{
public:
	CLogicObjectRenderer_SceneNodeProxy( CLogicObject_AABBPicking* pLogicObject, rs3::RSceneNode* pViewSceneNode, rs3::RSceneNode* pModelNode )
		: CLogicObjectRenderer_SceneNode( pLogicObject, pViewSceneNode )
		, m_pModelSceneNode(pModelNode) 
	{
	}

	void OnShow() override
	{
		__super::OnShow();
		m_pModelSceneNode->SetVisible( true );
		CVisibleSetter visibleSetter( true );
		m_pModelSceneNode->Traverse( &visibleSetter );
	}
	void OnHide() override
	{
		__super::OnHide();
		m_pModelSceneNode->SetVisible( false );
		CVisibleSetter visibleSetter( false );
		m_pModelSceneNode->Traverse( &visibleSetter );
	}

protected:
	RSceneNode*	m_pModelSceneNode;
};

//////////////////////////////////////////////////////////////////////////
// CLogicObject_PointLight

const DWORD COLOR_FAR = 0xFFFFFF00;
const DWORD COLOR_FAR2 = 0xFFA0A000;
const DWORD COLOR_NEAR = 0xFF8080FF;

CLogicObjectRenderer* CLogicObject_PointLight::CreateRenderer( CControlMediator* pControlMediator_ )
{
	class PointLightRenderer : public CLogicObjectRenderer_SceneNodeProxy
	{
	public:
		PointLightRenderer( CLogicObject_AABBPicking* pLogicObject, rs3::RSceneNode* pSceneNode, rs3::RPointLightSceneNode* pPointLight )
			: CLogicObjectRenderer_SceneNodeProxy( pLogicObject, pSceneNode, pPointLight )
			, m_pPointLight(pPointLight) {}

		void OnRenderSelection()
		{
			//if(m_pPointLight->IsAreaLight())
			if(1)
			{
				float fRadius = m_pPointLight->GetRadius();

				RRenderHelper::SetRenderState();
				RMatrix matTM;
				// far
				matTM.SetLocalMatrix( m_pPointLight->GetWorldPosition(), RVector(1,0,0), RVector(0,0,1) );
				RRenderHelper::RenderCircle( matTM, fRadius, COLOR_FAR);
				matTM.SetLocalMatrix( m_pPointLight->GetWorldPosition(), RVector(0,1,0), RVector(0,0,1) );
				RRenderHelper::RenderCircle( matTM, fRadius, COLOR_FAR);
				matTM.SetLocalMatrix( m_pPointLight->GetWorldPosition(), RVector(0,0,1), RVector(0,1,0) );
				RRenderHelper::RenderCircle( matTM, fRadius, COLOR_FAR);

				// near
				fRadius = m_pPointLight->GetAttenuationStart();
				matTM.SetLocalMatrix( m_pPointLight->GetWorldPosition(), RVector(1,0,0), RVector(0,0,1) );
				RRenderHelper::RenderCircle( matTM, fRadius, COLOR_NEAR);
				matTM.SetLocalMatrix( m_pPointLight->GetWorldPosition(), RVector(0,1,0), RVector(0,0,1) );
				RRenderHelper::RenderCircle( matTM, fRadius, COLOR_NEAR);
				matTM.SetLocalMatrix( m_pPointLight->GetWorldPosition(), RVector(0,0,1), RVector(0,1,0) );
				RRenderHelper::RenderCircle( matTM, fRadius, COLOR_NEAR);
			}
			else
			{
				RSphere sphere = m_pPointLight->GetSphere();
				rs3::RRenderHelper::RenderSphere(sphere);
			}
		}

	protected:
		rs3::RPointLightSceneNode* m_pPointLight;
	};


	//return new CLogicObjectRenderer_SceneNode(this, pViewActor);

	RActor* pViewActor = new RActor;
	pViewActor->Create( "omni.elu" );
	pViewActor->SetCollisionCreationEnable(false);
	pViewActor->SetScale(30);
	rs3::RPointLightSceneNode* pPointLight= MDynamicCast(RPointLightSceneNode, m_pModelNode);
	return new PointLightRenderer(this, pViewActor, pPointLight);
}

//////////////////////////////////////////////////////////////////////////
// SceneNodeInstance 를 뷰로 가지는 CLogicObject_SceneNode

CLogicObject* CLogicObject_SceneNodeWithSceneInstanceView::OnPickForSelection_(float& _rPrevPickedSqLength, const RVector &origin, const RVector &to)
{
	RPICKINFO pickInfo(origin, to, RPM_DETAIL);
	if( m_pViewSceneNode->Pick( pickInfo ) )
	{
		float fLSq = pickInfo.outDistanceSq;
		if ( fLSq < _rPrevPickedSqLength )
		{
			_rPrevPickedSqLength = fLSq;
			return this;
		}
	}

	return NULL;
}

void CLogicObject_SceneNodeWithSceneInstanceView::OnPickForSelection_(std::vector<CLogicObject*>& _refVecLogicObject, RViewFrustum& _refFrustum)
{
	if ( m_pViewSceneNode->GetAABB().isInViewFrustumWithZ( _refFrustum.GetPlanes() ) )
		_refVecLogicObject.push_back(this);
}

rs3::RSceneNode* CLogicObject_SceneNodeWithSceneInstanceView::CreateSceneInstanceView(const char* szSceneFileName)
{
	RToolSceneManager* pSceneManager = m_pController->GetToolSceneManager();

	// scene instance 를 로딩하기 위한 절차 -_- L3DCursor::SetNodeFile 에서 복사
	// TODO: 꼭 이런식으로 로딩해야 할까? 개선하자
	string sceneXMLPath = pSceneManager->GetRootSceneNode().GetXMLFilePath();
	const string* pSceneXMLPath = NULL;
	if( sceneXMLPath.length() )
		pSceneXMLPath = &sceneXMLPath;

	// 씬 로드
	RSceneNode* pNode = pSceneManager->CreateSceneNode(RSID_SCENEINSTANCE);
	bool bRet = pSceneManager->LoadSceneNodeFromFile(pNode, szSceneFileName);
	if ( !bRet )
	{
		delete pNode;
		return NULL;
	}

	pNode->SetScale( 10 );
	m_pViewSceneNode = pNode;

	return pNode;
}


//////////////////////////////////////////////////////////////////////////
// CLogicObject_SpotLight

CLogicObjectRenderer* CLogicObject_SpotLight::CreateRenderer( CControlMediator* pControlMediator_ )
{
	class SpotRenderer : public CLogicObjectRenderer_SceneNodeProxy
	{
	public:
		SpotRenderer( CLogicObject_AABBPicking* pLogicObject, rs3::RSceneNode* pSceneNode, rs3::RSpotLightSceneNode* pSpotLight )
			: CLogicObjectRenderer_SceneNodeProxy( pLogicObject, pSceneNode, pSpotLight )
			, m_pSpotLight(pSpotLight) {}

		void OnRenderSelection()
		{
			RRenderHelper::SetRenderState();

			// near circle
			float fAttenuationStart = m_pSpotLight->GetAttenuationStart();
			RMatrix matTrans;
			matTrans.SetTranslationMatrix( 0, fAttenuationStart, 0 );

			RMatrix matNearAtten;
			matNearAtten = matTrans * m_pSpotLight->GetWorldTransform() ;

			float fRadius = fAttenuationStart * tanf( 0.5f * DegreeToRadian( m_pSpotLight->GetFov() ) ) ;
			rs3::RRenderHelper::RenderCircle( matNearAtten, fRadius, COLOR_NEAR, true );

			// far circle
			matTrans.SetTranslationMatrix( 0, m_pSpotLight->GetFar(), 0 );
			RMatrix matFarAtten;
			matFarAtten = matTrans * m_pSpotLight->GetWorldTransform() ;
			fRadius = m_pSpotLight->GetFar() * tanf( 0.5f * DegreeToRadian( m_pSpotLight->GetFov() ) ) ;
			rs3::RRenderHelper::RenderCircle( matFarAtten, fRadius, COLOR_FAR, true );

			float fRadius2 = m_pSpotLight->GetFar() * tanf( 0.5f * DegreeToRadian( m_pSpotLight->GetFov() + 1 ) );
			rs3::RRenderHelper::RenderCircle( matFarAtten, fRadius2, COLOR_FAR2, false );

			// lines
			const int SEGMENT = 4;
			RVt_pos_clr vecLines[ SEGMENT * 4 ];
			for( int i=0; i<SEGMENT; i++ )
			{
				float theta = (2*MMath::PI*i)/(SEGMENT);

				// 바깥원
				vecLines[i*4+0].vPos = m_pSpotLight->GetWorldPosition();
				vecLines[i*4+1].vPos = fRadius2 * RVector( sin(theta), 0.f, cos(theta)) * matFarAtten;
				vecLines[i*4+0].dwColor = vecLines[i*4+1].dwColor = COLOR_FAR2;

				// 안쪽원
				vecLines[i*4+2].vPos = m_pSpotLight->GetWorldPosition();
				vecLines[i*4+3].vPos = fRadius * RVector( sin(theta), 0.f, cos(theta)) * matFarAtten;
				vecLines[i*4+2].dwColor = vecLines[i*4+3].dwColor = COLOR_FAR;
			}

			REngine::GetDevice().SetTransform( RST_WORLD, RMatrix::IDENTITY );
			REngine::GetDevice().SetFvF( RVt_pos_clr::FVF );
			REngine::GetDevice().SetTextureStageState(0, RTSS_COLOROP, RTOP_SELECTARG1);
			REngine::GetDevice().SetTextureStageState(0, RTSS_COLORARG1, RTA_DIFFUSE);

			REngine::GetDevice().DrawPrimitiveUP(RPT_LINELIST, SEGMENT*2, vecLines,sizeof(RVt_pos_clr));
		}

	protected:
		rs3::RSpotLightSceneNode* m_pSpotLight;
	};

	RSceneNode* pNode = CreateSceneInstanceView("spot_light.scene.xml");
	if( pNode )
	{
		rs3::RSpotLightSceneNode* pSpotLight = MDynamicCast(RSpotLightSceneNode, m_pModelNode);
		return new SpotRenderer(this, pNode, pSpotLight);
	}
	else
		return NULL;
}


//////////////////////////////////////////////////////////////////////////
// CLogicObject_DirectionalLight

CLogicObjectRenderer* CLogicObject_DirectionalLight::CreateRenderer( CControlMediator* pControlMediator_ )
{
	RSceneNode* pNode = CreateSceneInstanceView("directional_light.scene.xml");
	if( pNode )
		return new CLogicObjectRenderer_SceneNode(this, pNode);
	else
		return NULL;
}


//////////////////////////////////////////////////////////////////////////
// CLogicObject_SubLight

CLogicObjectRenderer* CLogicObject_SubLight::CreateRenderer( CControlMediator* pControlMediator_ )
{
	RSceneNode* pNode = CreateSceneInstanceView("directional_light.scene.xml");
	if( pNode )
		return new CLogicObjectRenderer_SceneNode(this, pNode);
	else
		return NULL;
}

//////////////////////////////////////////////////////////////////////////
// CLogicObject_EffectInstance

CLogicObjectRenderer* CLogicObject_EffectInstance::CreateRenderer( CControlMediator* pControlMediator_ )
{
	RSceneNode* pNode = CreateSceneInstanceView("omni.scene.xml");
	if( pNode )
		return new CLogicObjectRenderer_SceneNode(this, pNode);
	else
		return NULL;
}

//////////////////////////////////////////////////////////////////////////
// CLogicObject_VisibleSceneNode
CLogicObjectRenderer* CLogicObject_VisibleSceneNode::CreateRenderer( CControlMediator* pControlMediator_ )
{
	return new CLogicObjectRenderer_SceneNodeItself(this, m_pModelNode);
}

CLogicObject* CLogicObject_VisibleSceneNode::OnPickForSelection_(float& _rPrevPickedSqLength, const RVector &origin, const RVector &to)
{
	RPICKINFO pickInfo(origin, to, RPM_DETAIL);
	if( m_pModelNode->Pick( pickInfo ) )
	{
		float fLSq = pickInfo.outDistanceSq;
		if ( fLSq < _rPrevPickedSqLength )
		{
			_rPrevPickedSqLength = fLSq;
			return this;
		}
	}

	return NULL;
}

void CLogicObject_VisibleSceneNode::OnPickForSelection_(std::vector<CLogicObject*>& _refVecLogicObject, RViewFrustum& _refFrustum)
{
	if ( m_pModelNode->GetAABB().isInViewFrustumWithZ( _refFrustum.GetPlanes() ) )
		_refVecLogicObject.push_back(this);
}


//////////////////////////////////////////////////////////////////////////
// CLogicObject_Tree

CControlSceneProperty*	CLogicObject_Tree::GetProperty()
{
	return CTreeSceneNodeProperty::GetInstance();
}

CLogicObject* CLogicObject_Tree::OnPickForSelection_(float& _rPrevPickedSqLength, const RVector &origin, const RVector &to)
{
	RTreeSceneNode* pNode = dynamic_cast< RTreeSceneNode* > ( m_pModelNode );
	if( pNode == NULL ) return NULL;

	RPICKINFO kInfo(origin, to, RPM_DETAIL); //선택이든 아니든 피킹은 디테일하게 진행
	if ( pNode->Pick(kInfo))
	{
		float fLSq = kInfo.outPosition.DistanceToSq(origin);
		if ( fLSq < _rPrevPickedSqLength )
		{
			_rPrevPickedSqLength = fLSq;
			return this;
		}
	}
	return NULL;
}

void CLogicObject_Tree::OnPickForSelection_(std::vector<CLogicObject*>& _refVecLogicObject, RViewFrustum& _refFrustum)
{
	RTreeSceneNode* pNode = dynamic_cast< RTreeSceneNode* > ( m_pModelNode );
	if( pNode == NULL) return;

	if (pNode->SelectFrustumTest(_refFrustum))
		_refVecLogicObject.push_back(this);
}

//////////////////////////////////////////////////////////////////////////
// CLogicObject_SceneInstance

CControlSceneProperty*	CLogicObject_SceneInstance::GetProperty()
{
	return CSceneNodeInstanceProperty::GetInstance();
}

//////////////////////////////////////////////////////////////////////////
// CLogicObject_StaticMesh

CControlSceneProperty*	CLogicObject_StaticMesh::GetProperty()
{
	return CStaticMeshSceneNodeProperty::GetInstance();
}

CLogicObject* CLogicObject_StaticMesh::OnPickForSelection_(float& _rPrevPickedSqLength, const RVector &origin, const RVector &to)
{
	RStaticMeshSceneNode* pNode = dynamic_cast< RStaticMeshSceneNode* > ( m_pModelNode );
	if( pNode == NULL ) return NULL;
	if( pNode->getEnablePicking() == false ) return NULL;

	return CLogicObject_VisibleSceneNode::OnPickForSelection_( _rPrevPickedSqLength, origin, to );
}

void CLogicObject_StaticMesh::OnPickForSelection_(std::vector<CLogicObject*>& _refVecLogicObject, RViewFrustum& _refFrustum)
{
	RStaticMeshSceneNode* pNode = dynamic_cast< RStaticMeshSceneNode* > ( m_pModelNode );
	if( pNode == NULL ) return;
	if( pNode->getEnablePicking() == false ) return;

	CLogicObject_VisibleSceneNode::OnPickForSelection_( _refVecLogicObject, _refFrustum );
}

//////////////////////////////////////////////////////////////////////////
// terrain
CControlSceneProperty*	CLogicObject_Terrain::GetProperty()
{
	return CTerrainSceneNodeProperty::GetInstance();
}

//////////////////////////////////////////////////////////////////////////
// water plane
CControlSceneProperty*	CLogicObject_WaterPlane::GetProperty()
{
	return CWaterPlaneSceneNodeProperty::GetInstance();
}

CLogicObjectRenderer* CLogicObject_Occluder::CreateRenderer( CControlMediator* pControlMediator_ )
{
	class OccluderRenderer : public CLogicObjectRenderer
	{
	public:
		OccluderRenderer( CLogicObject_AABBPicking* pLogicObject, rs3::RSceneNode* pSceneNode, rs3::ROccluderSceneNode* pOccluderSceneNode )
			:m_pOccluderSceneNode(pOccluderSceneNode)
		{
		}

		void OnRender() override
		{
			ROccluderPointsSet* pOccluderPointSet = m_pOccluderSceneNode->GetWorldTransformedOccluderPointsSet();
			if( pOccluderPointSet )
			{
				ROccluderUtil::RenderOcclusionPointsSet(*pOccluderPointSet,0xFFFF0000);
				//RRenderHelper::RenderBox(RMatrix::IDENTITY,m_pOccluderSceneNode->GetAABB(),0xFFFFFFFF); //AABB를 그리고 싶으면 주석을 풀어라
			}
		}

	protected:
		rs3::ROccluderSceneNode* m_pOccluderSceneNode;
	};

	rs3::ROccluderSceneNode* pOccluderSceneNode = MDynamicCast(ROccluderSceneNode, m_pModelNode);

	return new OccluderRenderer(this, NULL, pOccluderSceneNode);
}

CLogicObject* CLogicObject_Occluder::OnPickForSelection_( float& _rPrevPickedSqLength, const rs3::RVector &origin, const rs3::RVector &to )
{
	if(  GetVisible() && GetEditable() && m_pModelNode->GetAABB().ContainsLineSegment(origin, to) 
		)
	{
		float fLSq = (origin - m_pModelNode->GetAABB().GetCenter() ).LengthSq();
		if ( fLSq < _rPrevPickedSqLength )
		{
			_rPrevPickedSqLength = fLSq;
			return this;
		}
	}

	return NULL;
}
