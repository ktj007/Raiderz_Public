#include "stdafx.h"

#include "RTreeSceneNode.h"

#include "RTreeResource.h"
#include "RTreeResourceMgr.h"

#include "RTreeCollision.h"
#include "RCollisionTreeBuilder.h"

#include "RBackgroundWorker.h"
#include "RFont.h"

namespace rs3 {

MImplementRTTI(RTreeSceneNode, RSceneNode);

int RTreeSceneNode::CalcPriority()
{
	int nPriority(0);
	RCameraSceneNode* pCamera = REngine::GetSceneManager().GetPrimaryCamera();
	if (pCamera)
	{
		float fDistance = pCamera->GetWorldPosition().DistanceTo(GetWorldPosition());

		if (fDistance > DEFAULT_DISTANCE_FOR_BACKGROUND_PRIORITY)
		{
			return RBGWK_SLEEP;
		}
	}

	// 우선순위 조정
	nPriority = nPriority + (RBGWK_HIGH_OFFSET * 2);
	return nPriority;
}

// void RTreeSceneNode::TREE_LOADER::OnBackgroundWork()
// {
// 	_ASSERT( m_pTreeSceneNode != NULL );
// 	if ( m_pTreeSceneNode->m_pTreeResource->GetState() == RRS_EMPTY )
// 	{
// 		bool bRet = m_pTreeSceneNode->m_pTreeResource->Fill();
// 		if( !bRet )
// 			mlog("트리 로드 - Fill 실패\n");
// 	}
// }

void RTreeSceneNode::OnLoadingComplete(RResource* pResource)
{
	if( m_pTreeResource == pResource)
	{
		if( m_pTreeResource->GetState() == RRS_WRONG)
			mlog("트리 로드 - Load 실패\n");

		SetUsableAndUpdate_();
	}
}

float	RTreeSceneNode::m_fLodFarDist = RTREE_DEFAULT_LOD_FAR;

RTreeSceneNode::RTreeSceneNode()
: RSceneNode(RRP_SPEEDTREE)
, m_pTreeResource(NULL)
, m_bUserLOD(false)
, m_bNeedUpdateMatrix(true)
, m_fWindMatrixOffset(0.0f)
, m_fLod(1.0f)
, m_PrevSin(2.0f)
, m_PrevCos(2.0f)
, m_PrevAtan2(0.0f)
, m_fShadowing(-1.0f)
// , m_pLoadingWorkUnit(NULL)
, m_pNextRenderChain(NULL)
, m_fCosTheta(0)
, m_fSinTheta(1)
{
	m_afBoundingBox[0] = m_afBoundingBox[1] = m_afBoundingBox[2] = m_afBoundingBox[3] = m_afBoundingBox[4] = m_afBoundingBox[5] = 0.0f;

	SetUsable(false);
	SetVisible(false);

	InitSpeedTreeLODInfo();
//	SetUpdateMethod(RUM_UPDATE_MANUALLY);
}
RTreeSceneNode::~RTreeSceneNode()
{
	// background workable, delete는 null검사할 필요 없다.
// 	if (m_pLoadingWorkUnit)
// 	{
// 		SAFE_DELETE(m_pLoadingWorkUnit);
// 	}

	if( m_pTreeResource )
	{
// 		m_pTreeResource->RemoveLoadingController( this );
		REngine::GetSceneManager().ReleaseResource( m_pTreeResource );
	}
}



void RTreeSceneNode::InitSpeedTreeLODInfo( void )
{
	m_SpeedTreeLodInfo.m_nFrondActiveLod = 0;
	m_SpeedTreeLodInfo.m_nBranchActiveLod = 0;
	m_SpeedTreeLodInfo.m_fFrondAlphaTestValue = 84.0f;
	m_SpeedTreeLodInfo.m_fBranchAlphaTestValue = 84.0f;
	m_SpeedTreeLodInfo.m_fBillboardFadeOut = 0.0f;
	m_SpeedTreeLodInfo.m_anLeafActiveLods[0] = m_SpeedTreeLodInfo.m_anLeafActiveLods[1] = 0;
	m_SpeedTreeLodInfo.m_afLeafAlphaTestValues[0] = m_SpeedTreeLodInfo.m_afLeafAlphaTestValues[1] = 84.0f / 255.0f;
}


bool RTreeSceneNode::CreateFromXML(MXmlElement &element, bool bBackgroundCreation)
{
	char szBuffer[256];
	MXmlElement *pElement = element.FirstChildElement("FILENAME");
	_Contents(szBuffer,pElement);

	string fileName(szBuffer);
	return Build( fileName.c_str(), bBackgroundCreation );
}

void RTreeSceneNode::SetUsableAndUpdate_()
{
	// 트리 리소스가 로딩되어 있지 않으면 usable 설정을 하지 않는다.
	if( false == m_pTreeResource->IsLoadingSucceeded() )
		return;

	// init
	memcpy(m_afBoundingBox, m_pTreeResource->m_afBoundingBox, sizeof(float) * 6);
	SetLOD(1.0f);

	SetWindMatrixOffset(MMath::RandomNumber(0.0f, 1.0f));

	// init for scene manager
	SetUsable(true);
	SetVisible(true);

	// transform 과 aabb 를 갱신한다
	NeedUpdateTransform();
	RSceneNodeUpdateVisitor visitor;
	Traverse(&visitor);
}

bool RTreeSceneNode::Build(const char* filename, bool bBackgroundCreation)
{
	_ASSERT( m_pTreeResource == NULL && "이미 연결된 메시가 있음");

	bool bLoadMonitoring = false;
	if( bBackgroundCreation && CalcPriority() != RBGWK_SLEEP )
		bLoadMonitoring = true;

	m_pTreeResource = REngine::GetSceneManager().CreateResource< RTreeResource >( filename, bLoadMonitoring );

	m_pTreeResource->SetSpeedTreeCreationInfo();

	return REngine::GetSceneManagerPtr()->AddLoadingList( m_pTreeResource, bBackgroundCreation, this );

}

void RTreeSceneNode::OnRender( RCameraSceneNode* pCurrentCamera, int nSubMaterialID, RRENDERINGMETHOD renderMothod)
{
	// Pass Renderer 가 이 씬노드에 대한 렌더링을 결정합니다.
	_ASSERT( false && "이거 불리지 말아요." );
}


bool RTreeSceneNode::UpdateLOD(RCameraSceneNode *pCamera, float fNearLod, float fFarLod, bool bUseFogFar)
{
	RPFC_THISFUNC;
	_ASSERT( m_pTreeResource);
	_ASSERT( m_pTreeResource->m_pSpeedTree );
	_ASSERT( REngine::GetDevicePtr() );

	float fDistance = pCamera->GetDistanceTo(GetSphere());

	if ( !m_bUserLOD )
	{
		// FogFar가 FarLod보다 작으면 FogFar를 FarLod로 사용
		if( bUseFogFar && pCamera->GetFogFar() < fFarLod )
		{
			fFarLod = pCamera->GetFogFar();
		}

		if( fFarLod > 0)
		{
			// SpeedTreeRT assumes 1.0 = highest, 0.0 = lowest
			float fLod = 1.0f - (fDistance - fNearLod) / ( fFarLod -  fNearLod);
			if (fLod > 1.0f) fLod = 1.0f;
			if (fLod < 0.0f) fLod = 0.0f;
			m_fLod = fLod;
		}
		else
			m_fLod = 0;
	}

	if (m_fLod <= 0.0f)
	{
		m_SpeedTreeLodInfo.m_fBillboardFadeOut = 1.0f;
		m_SpeedTreeLodInfo.m_anLeafActiveLods[0] = m_SpeedTreeLodInfo.m_anLeafActiveLods[1] = -1;
	}
	else
	{
		CSpeedTreeRT::SLodValues sLod;
		m_pTreeResource->m_pSpeedTree->GetLodValues( sLod, m_fLod );

		// branches
		m_SpeedTreeLodInfo.m_nBranchActiveLod = char(sLod.m_nBranchActiveLod);
		m_SpeedTreeLodInfo.m_fBranchAlphaTestValue = sLod.m_fBranchAlphaTestValue;

		// fronds
		m_SpeedTreeLodInfo.m_nFrondActiveLod = char(sLod.m_nFrondActiveLod);
		m_SpeedTreeLodInfo.m_fFrondAlphaTestValue = sLod.m_fFrondAlphaTestValue;

		// billboard
		m_SpeedTreeLodInfo.m_fBillboardFadeOut = sLod.m_fBillboardFadeOut;

		// leaves
		if (sLod.m_anLeafActiveLods[0] > -1)
		{
			m_SpeedTreeLodInfo.m_anLeafActiveLods[0] = char(sLod.m_anLeafActiveLods[0]);
			m_SpeedTreeLodInfo.m_afLeafAlphaTestValues[0] = sLod.m_afLeafAlphaTestValues[0];
		}
		else
			m_SpeedTreeLodInfo.m_anLeafActiveLods[0] = -1;

		if (sLod.m_anLeafActiveLods[1] > -1)
		{
			m_SpeedTreeLodInfo.m_anLeafActiveLods[1] = char(sLod.m_anLeafActiveLods[1]);
			m_SpeedTreeLodInfo.m_afLeafAlphaTestValues[1] = sLod.m_afLeafAlphaTestValues[1];
		}
		else
			m_SpeedTreeLodInfo.m_anLeafActiveLods[1] = -1;

	}

	return true;
}

void RTreeSceneNode::OnUpdateTransform()
{
	RPFC_THISFUNC;

	m_PositionScaleMatrix.MakeIdentity();
	m_PositionScaleMatrix.SetTranslation( GetWorldPosition() );
	m_PositionScaleMatrix.SetScale( GetScale() );

	//  TODO : Z축 기준 회전 각 구해놓을것... 과연 직접 구해? 기록 된 값 없나?
	// 	RMatrix mxTmp = GetWorldTransform();
	// 	RVector vTran;
	// 	RQuaternion quatRotation;
	// 	mxTmp._41 = 0;
	// 	mxTmp._42 = 0;
	// 	mxTmp._43 = 0;
	// 	mxTmp.Decompose( vTran, quatRotation);
	// 	// 구현 되어 있지 않은 함수. DX의
	// 	// 쿼터니언으로부터 축 기준 회전 각을 알고 싶다. 하지만 구현 되어 있지 않은 함수. DX의 D3DXQuaternionToAxisAngle()로 사용
	// 	D3DXQUATERNION qRot( quatRotation.x, quatRotation.y, quatRotation.z, quatRotation.w);
	// 	D3DXVECTOR3 vAxis( 0,0,1);
	// 	FLOAT fAngle = 0;
	// 	D3DXQuaternionToAxisAngle( &qRot, &vAxis, &fAngle);
	// 	m_fAngleAxisZ = fAngle;

	// 로컬 TM이 스케일링이 제거된 상태이다... 맞지?
	RMatrix mxTmp = GetLocalTransform();
	// 트리는 Z축 회전만 들어가 있는 상태이므로
	// cos(x)  -sin(x)    0
	// sin(x)   cos(x)    0
	//      0        0    1
	m_fCosTheta = mxTmp._11;
	m_fSinTheta = mxTmp._21;
}

void RTreeSceneNode::OnUpdateAABB()
{
	RPFC_THISFUNC;

	RVector minvec(m_afBoundingBox[0], m_afBoundingBox[1], m_afBoundingBox[2]);
	RVector maxvec(m_afBoundingBox[3], m_afBoundingBox[4], m_afBoundingBox[5]);
	RVector min_y_vec(m_afBoundingBox[3], m_afBoundingBox[1],  m_afBoundingBox[5]);
	RVector max_y_vec(m_afBoundingBox[0], m_afBoundingBox[4], m_afBoundingBox[2]);

	minvec = minvec * (*m_matWorld);
	max_y_vec = max_y_vec * (*m_matWorld);

	maxvec = maxvec * (*m_matWorld);
	min_y_vec = min_y_vec * (*m_matWorld);

	RBoundingBox aabb;
	std::vector<float> vecBoxEdgy;
	vecBoxEdgy.reserve(4);
	vecBoxEdgy.push_back(minvec.x);
	vecBoxEdgy.push_back(max_y_vec.x);
	vecBoxEdgy.push_back(maxvec.x);
	vecBoxEdgy.push_back(min_y_vec.x);
	sort(vecBoxEdgy.begin(), vecBoxEdgy.end());


	aabb.vmin.x = vecBoxEdgy[0];
	aabb.vmax.x = vecBoxEdgy[3];

	vecBoxEdgy[0] = minvec.y;
	vecBoxEdgy[1] = max_y_vec.y;
	vecBoxEdgy[2] = maxvec.y;
	vecBoxEdgy[3] = min_y_vec.y;
	sort(vecBoxEdgy.begin(), vecBoxEdgy.end());

	aabb.vmin.y = vecBoxEdgy[0];
	aabb.vmax.y = vecBoxEdgy[3];

	aabb.vmin.z = minvec.z;
	aabb.vmax.z = maxvec.z;
	_ASSERT( aabb.IsValid() );
	SetAABB( aabb );
}

bool RTreeSceneNode::OnPick(RPICKINFO& pickInfo)
{
	_ASSERT( m_pTreeResource );
	_ASSERT( m_pTreeResource->m_pCollisionInfo );

	if (!GetAABB().ContainsLineSegment(pickInfo.inPickOrigin, pickInfo.inPickTo))
		return false;

	if (m_pTreeResource->m_pCollisionInfo->m_vCollisions.empty())
	{
		RMatrix invTransform;
		float d;
		if (!GetWorldTransform().GetInverse(&invTransform, &d))
			return false;

		RVector movedOrigin;
		invTransform.TransformVect(pickInfo.inPickOrigin, movedOrigin);
		RVector movedTo;
		invTransform.TransformVect(pickInfo.inPickTo, movedTo);

		RBox b = m_pTreeResource->m_Box;

		b.vmax *= 0.5;
		b.vmin *= 0.5;

		if (b.ContainsLineSegment(movedOrigin, movedTo))
		{
			float fDist;
			RVector movedDir = Normalize(movedTo-movedOrigin);
			if(b.Intersect(&fDist,movedOrigin,movedDir))
			{
				pickInfo.outPosition = (movedOrigin+fDist*movedDir) * GetWorldTransform();
				return true;
			}
		}
		return false;
	}

	bool b =m_pTreeResource->m_pCollisionInfo->GetCollision(GetWorldTransform(), pickInfo.inPickOrigin, pickInfo.inPickTo, pickInfo.outPosition);
	return b;
}

RSceneNode*	RTreeSceneNode::Clone()
{
	RTreeSceneNode* pInstance = static_cast< RTreeSceneNode* >( REngine::GetSceneManager().CreateSceneNode( RSID_TREE ) );
	pInstance->Build( m_pTreeResource->GetName().c_str());
	pInstance->SetTransform(GetLocalTransform());
	pInstance->SetScale(GetScale());
	pInstance->UpdateTransform();
	return pInstance;
}

void RTreeSceneNode::SaveToXML(MXmlElement &element)
{
	string strFilename = m_pTreeResource->GetName();
	MXmlElement *pElement = new MXmlElement("FILENAME");
	strFilename = MGetPureFileNameExt( strFilename );
	_SetContents(pElement, strFilename.c_str());	
	element.LinkEndChild(pElement);
}

void RTreeSceneNode::RenderCollisionObject()
{
	RPFC_THISFUNC;
	_ASSERT( m_pTreeResource );
	m_pTreeResource->m_pCollisionInfo->RenderCollisionObject(GetWorldTransform());
}

bool RTreeSceneNode::SelectFrustumTest(const RViewFrustum& Frustum)
{
	_ASSERT( m_pTreeResource );
	return m_pTreeResource->m_pCollisionInfo->FrustumTest(GetWorldTransform(), Frustum, GetAABB());
}

const std::string& RTreeSceneNode::GetTreeFileName()
{
	_ASSERT( m_pTreeResource );
	return m_pTreeResource->GetName();
}

void RTreeSceneNode::RegisterForMakingLightMap( RCameraSceneNode* pCurrentCamera)
{
	SetUserLOD(true);
	SetLOD(1.0f);

}

void RTreeSceneNode::CollectStaticCollisionPolygons(RSceneNodePolygonCollector* pCollector) const
{
	vector<RVector> v;
	m_pTreeResource->m_pCollisionInfo->GetCollisionPolygons(GetWorldTransform(), v);	

	for (size_t i=0; i<v.size()/3; i++)
		pCollector->OnAddPolygon(v[i*3], v[i*3+1], v[i*3+2], RCollisionTriangle::TREE, 0, 0);

}

RTreeCollisionObject* RTreeSceneNode::GetTreeCollisionInfo(RTreeCollisionType type)
{
	if (m_pTreeResource == NULL)
		return NULL;

	return m_pTreeResource->GetTreeCollisionInfo(type);
}

void RTreeSceneNode::AddToRenderingList( RRenderingList* pRenderingList )
{
	pRenderingList->AddTreeChain(this);
}

void RTreeSceneNode::RemoveFromRenderingList( RRenderingList* pRenderingList )
{
	pRenderingList->RemoveTreeChain(this);
}

bool RTreeSceneNode::CalculateLOD( RCameraSceneNode* pCamera, const RSphere* pBoundingSphere)
{
	RPFC_THISFUNC;

	if( !GetVisible() || !GetUsable() || GetOcclued() )	return false;

	if ( pCamera->GetTreeUserLODEnabled() )
	{
		SetUserLOD(true);
		SetLOD(pCamera->GetTreeUserLOD());
	}
	else
	{
		SetUserLOD(false);
	}

	return UpdateLOD( pCamera, RTREE_DEFAULT_LOD_NEAR, m_fLodFarDist/*RTREE_DEFAULT_LOD_FAR*/, pCamera->GetFogEnable());
}

bool RTreeSceneNode::IsLoadingCompleted()
{
	if( NULL == m_pTreeResource )							return false;
	if( false == m_pTreeResource->IsLoadingCompleted() )	return false;
	return true;
}

}
