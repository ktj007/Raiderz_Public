#include "StdAfx.h"
#include "RUtil.h"
#include <assert.h>
#include <float.h>
#include <fstream>

#include "RResourceID.h"
#include "RTextureManager.h"

#include "RSceneNodeID.h"
#include "RCameraSceneNode.h"
#include "RCameraFPSSceneNode.h"
#include "RCameraArcBallSceneNode.h"
#include "RLightSceneNode.h"
#include "RSpotLightSceneNode.h"
#include "RCameraSceneNode.h"
#include "RSceneSky.h"
#include "MDebug.h"
#include "RGrassSystem.h"
#include "MRTTI.h"

#include "RRenderHelper.h"

#include "C_ControlSceneProperty.h"
#include "C_ControlMediator.h"
#include "C_ControlMediatorHelperAction.h"
#include "C_ControlMediatorHelperResource.h"

#include "M_ToolLogicObjectManager.h"

#include "M_ToolSceneManager.h"
#include "M_ToolActor.h"
#include "M_ToolSceneNodeInstance.h"
#include "M_ToolStaticMeshSceneNode.h"
#include "M_ToolTerrain.h"
#include "M_ToolPointLight.h"
#include "M_LogicObject_SceneNodes.h"
#include "M_Visitors.h"

#include "RTreeSceneNode.h"

#include "C_XMLConfig.h"
#include "C_ControlCursor.h"
#include "FileInfo.h"
#include "MFileSystem.h"

// Tree PassRenderer Lazy Init
#include "C_XMLXConst.h"
#include "C_XMLEditInfoFile.h"

#include "MainFrm.h"
#include "VR_EtcCollision.h"

namespace rs3 {

char* RToolSceneManager::SSceneNodeFinder::pSzSceneNodeName = NULL;

RToolSceneManager::RToolSceneManager(CControlMediator* pControlMediator)
: /*m_pPickingIgnoreSceneNode(NULL), */m_bWireFrameDrawMode(false), m_pSceneNodeModifier(NULL)
, m_pControlMediator(pControlMediator)
{
	// 멀티 루트 씬이 고려 되지 않아 있음.
	GetRootSceneNode().SetUserData( (void*) this );

	// TODO: 마저 종속성 끊어내야 하는것들
	CControlSceneProperty::m_pControlMediator = pControlMediator;
}

RToolSceneManager::~RToolSceneManager()
{
	SAFE_DELETE(m_pSceneNodeModifier);

	// 일단 부모것을 지우고
	RSceneManager::Clear();
}

bool RToolSceneManager::LoadEditableSceneXML(const char *szFileName)
{
	// 멀티 루트 씬에서는 루트 씬 자체가 제거 되었다가 생성되고 한다.
	// 새 루트 씬이 만들어 질 시마다 SetProperty()다시 해 주어야 하는데, 
	// GetRootSceneNode()에서 새 루트 씬을 생성 하므로 이를 수행
	GetRootSceneNode().SetUserData( (void*) this );

	RWorldSceneNode* pWorldSceneNode = &GetRootSceneNode();

	return LoadEditableSceneXML( pWorldSceneNode, szFileName);
}

void RToolSceneManager::AttachLogicObjectTraverse( RSceneNode* pSceneNodeTree )
{
	CEditableNodeCollector editableNodeCollector;

	pSceneNodeTree->Traverse(&editableNodeCollector);
	for(std::vector<RSceneNode*>::iterator iter = editableNodeCollector.m_SceneNodes.begin();iter != editableNodeCollector.m_SceneNodes.end(); iter++)
	{
		RSceneNode* pSceneNode = (*iter);
		AttachLogicObject( pSceneNode);
	}
}


bool RToolSceneManager::LoadEditableSceneXML(RSceneNode *pSceneNodeToLoad, const char* szFileName)
{
	bool bRet = LoadWorldorInstanceFromRootXMLFile(pSceneNodeToLoad, szFileName, false);

	// scene 편집이 아닌경우에도 임포트 하고 삭제할수 있도록 하자
	AttachLogicObjectTraverse( pSceneNodeToLoad );
	
	if (bRet)
		UpdateRootSceneTransform();

	return bRet;
}

void RToolSceneManager::UpdateRootSceneTransform()
{
	// 로딩후 업데이트
	GetRootSceneNode().NeedUpdateTransform();
	RSceneNodeUpdateVisitor updateVisitor;
	GetRootSceneNode().Traverse(&updateVisitor);
}

CLogicObject_SceneNode* RToolSceneManager::CreateLogicObject( RSceneNode* pSceneNode )
{
	CLogicObject_SceneNode* pLogicObject = NULL;

	// SceneInstance 노드는 LoadXML함수가 원본 XML 에서 한번 불리고
	// 대상 XML 의 ROOT 노드를 읽을때 한번 더 불려서 중복된다 이경우도 두번 붙지 않도록한다
	if( stricmp(pSceneNode->GetNodeIDString(),RSID_SCENEINSTANCE)==0 && pSceneNode->GetUserData()!=NULL ) // 이미 붙어있다
		return NULL;

	if(stricmp(pSceneNode->GetNodeIDString(),RSID_LIGHTSCENENODE)==0 )
		pLogicObject = new CLogicObject_PointLight;
	else if(stricmp(pSceneNode->GetNodeIDString(),RSID_SPOTLIGHTSCENENODE)==0 )
		pLogicObject = new CLogicObject_SpotLight;
	else if(stricmp(pSceneNode->GetNodeIDString(),RSID_DIRECTIONALLIGHTSCENENODE)==0 )
		pLogicObject = new CLogicObject_DirectionalLight;
	else if(stricmp(pSceneNode->GetNodeIDString(),RSID_SUBLIGHTSCENENODE)==0 )
		pLogicObject = new CLogicObject_SubLight;
	else if(stricmp(pSceneNode->GetNodeIDString(),RSID_TREE)==0 )
		pLogicObject = new CLogicObject_Tree;
	else if(stricmp(pSceneNode->GetNodeIDString(),RSID_SCENEINSTANCE)==0 )
		pLogicObject = new CLogicObject_SceneInstance;
	else if(stricmp(pSceneNode->GetNodeIDString(),RSID_STATICMESHSCENENODE)==0 )
		pLogicObject = new CLogicObject_StaticMesh;
	else if(stricmp(pSceneNode->GetNodeIDString(),RSID_TERRAINSCENENODE)==0 )
		pLogicObject = new CLogicObject_Terrain;
	else if(stricmp(pSceneNode->GetNodeIDString(),RSID_ACTOR)==0 )
		pLogicObject = new CLogicObject_Actor;
	else if(stricmp(pSceneNode->GetNodeIDString(),RSID_WATERPLANE)==0 )
		pLogicObject = new CLogicObject_WaterPlane;
	else if(stricmp(pSceneNode->GetNodeIDString(),RSID_EFFECTINSTANCE)==0 )
		pLogicObject = new CLogicObject_EffectInstance;
	else if(stricmp(pSceneNode->GetNodeIDString(),RSID_OCCLUDER)==0 )
		pLogicObject = new CLogicObject_Occluder;

	if(pLogicObject)
	{
		pLogicObject->LinkModelSceneNode( pSceneNode );
		pLogicObject->SetController( m_pControlMediator );
	}

	return pLogicObject;
}

bool RToolSceneManager::DestroyLogicObject( RSceneNode* pSceneNode )
{
	CLogicObject* pLogicObject = GetLogicObjectLinkedTo(pSceneNode);
	if( !pLogicObject )
		return false;

	delete pLogicObject;
	pSceneNode->SetUserData( NULL );

	return true;
}

bool RToolSceneManager::AttachLogicObject( RSceneNode* pSceneNode )
{
	CLogicObject_SceneNode* pLogicObject = pLogicObject = CreateLogicObject( pSceneNode );
	if( pLogicObject == NULL ) return false;

	m_pControlMediator->GetToolLogicObjectMgr()->AddToManager(pLogicObject);

	return true;
}

bool RToolSceneManager::DetachLogicObject( RSceneNode* pSceneNode )
{
	CLogicObject* pLogicObject = GetLogicObjectLinkedTo(pSceneNode);
	if( !pLogicObject )
		return false;

	m_pControlMediator->GetToolLogicObjectMgr()->RemoveFromManager(pLogicObject);

	DestroyLogicObject(pSceneNode);

	return true;
}

void RToolSceneManager::Clear()
{
	RSceneManager::Clear();
}

void RToolSceneManager::Update()
{
	RSceneManager::Update();

	if(m_pSceneNodeModifier)
	{
		CControlMediator* pMediator = ((CMainFrame*)AfxGetMainWnd())->GetControlMediator();
		CRollupEtcCollision* pCollision = pMediator->GetViewPtr<CRollupEtcCollision>();
		if( pCollision )
			m_pSceneNodeModifier->Update( pCollision->GetCollisionTree().GetCollisionTree() );
	}
}

CLogicObject* RToolSceneManager::GetLogicObjectLinkedTo( const RSceneNode* pSceneNode )
{
	CLogicObject* pLogicObject = dynamic_cast<CLogicObject*>((CCoreObject*)pSceneNode->GetUserData());
	return pLogicObject;
}

// 드래그 선택
int RToolSceneManager::AABBPickingFrustum(RSceneNodeList *pOutList, const RSceneNodeList *pChildNodeList, RViewFrustum &kFrustum)
{
	if ( pChildNodeList->size() == 0 )
		return 0;

	int nNum = 0;
	kFrustum.MakePoints();

	for ( RSceneNodeList::const_iterator it = pChildNodeList->begin(); it != pChildNodeList->end(); ++it )
	{
		RSceneNode *pNode = *it;
		assert(pNode!=NULL);

// 		if ( pNode->GetProperty() == NULL )
// 			continue;

		// 프러스텀 피킹에서 선택안할 목록
		const char* pSzNodeId = pNode->GetNodeIDString();
		if (_stricmp(pSzNodeId, RSID_CAMERASCENENODE) == 0 ||
			_stricmp(pSzNodeId, RSID_TERRAINSCENENODE) == 0 ||
			_stricmp(pSzNodeId, RSID_DIRECTIONALLIGHTSCENENODE ) == 0 ||
			_stricmp(pSzNodeId, RSID_SUBLIGHTSCENENODE ) == 0 ||
			_stricmp(pSzNodeId, RSID_WATERPLANE ) == 0 
// 			// logicobject 로 편집하는 경우, 모든 신노드에 대해 처리하고나면 파내자
// 			|| GetLogicObjectLinkedTo(pNode)!=NULL 
			)
			continue;

		// static 메쉬면 속성을 검사해서 피킹대상이 되는지 정함
		if( _stricmp(pNode->GetNodeIDString(), RSID_STATICMESHSCENENODE) == 0 )
		{
			RStaticMeshSceneNode* pStaticMesh = (RStaticMeshSceneNode*)pNode;
			if( pStaticMesh->getEnablePicking() == false )
				continue;
		}

		// 씬노드 인스턴스의 자식은 픽킹 안함
		if ( pNode->GetChildren().size() && _stricmp(pNode->GetNodeIDString(), RSID_SCENEINSTANCE) != 0 )
			nNum += AABBPickingFrustum(pOutList, &pNode->GetChildren(), kFrustum);

		if ( pNode->GetVisible() == false )
			continue;

// 		// 피킹 무시할 씬노드 - 인도어의 바닥등 - 를 무시하게
// 		if( pNode == m_pPickingIgnoreSceneNode )
// 			continue;

		if ( _stricmp(pNode->GetNodeIDString(), RSID_LIGHTSCENENODE) == 0 )
		{
			if(kFrustum.TestBox( pNode->GetAABB() ) )
			{
				++nNum;
				pOutList->push_back(pNode);
			}
		}
		// 액터는 선택안함
		else if ( _stricmp(pNode->GetNodeIDString(), RSID_ACTOR) == 0 )
		{
		}
		else if ( MIsExactlyClass( RTreeSceneNode, pNode) )
		{
			RTreeSceneNode* pTree = (RTreeSceneNode*)pNode;

			if (pTree->SelectFrustumTest(kFrustum))
			{
				++nNum;
				pOutList->push_back(pNode);
			}
		}
		else if ( pNode->GetAABB().isInViewFrustumWithZ( kFrustum.GetPlanes() ) )
		{
			++nNum;
			pOutList->push_back(pNode);
		}
	}

	return nNum;
}

// 클릭 선택
RSceneNode* RToolSceneManager::AABBPicking(const RSceneNodeList *pNodeList, const RVector &origin, const RVector &to, RPICKINFO_MAP& _refPickInfoOut, bool _bSelectMode)
{
	if ( pNodeList->size() == 0 )
		return NULL;

	RSceneNode* pCurrentMouseNode = L3DCursor::GetInstance()->GetNode();
	RSceneNode *pPickedNode = NULL;

	for ( RSceneNodeList::const_iterator it = pNodeList->begin(); it != pNodeList->end(); ++it )
	{
		RSceneNode *pNode = *it;
		assert(pNode!=NULL);

		// 프로퍼티가 없거나 보이지 않는 물체는 선택 안되게
// 		if ( pNode->GetProperty() == NULL )
// 			continue;

		// 마우스 위에 붙어 있는 노드는 선택 안되게
		if ( pNode == pCurrentMouseNode )
			continue;

		// 무시할 씬노드들
		if (_stricmp(pNode->GetNodeIDString(), RSID_CAMERASCENENODE) == 0 ||
			_stricmp(pNode->GetNodeIDString(), RSID_TERRAINSCENENODE) == 0 
// 			// TODO: logicobject 로 편집하는 경우, 모든 신노드에 대해 처리하고나면 파내자
// 			|| GetLogicObjectLinkedTo(pNode)!=NULL 
			)
			continue;

		// static 메쉬면 속성을 검사해서 피킹대상이 되는지 정함
		if( _stricmp(pNode->GetNodeIDString(), RSID_STATICMESHSCENENODE) == 0 )
		{
			RStaticMeshSceneNode* pStaticMesh = (RStaticMeshSceneNode*)pNode;
			if( pStaticMesh->getEnablePicking() == false && _bSelectMode)
				continue;
		}

		// 자식 씬노드 순회
		if( pNode->GetChildren().size() && _stricmp(pNode->GetNodeIDString(), RSID_SCENEINSTANCE) != 0 )
		{
			pNode = AABBPicking(&pNode->GetChildren(), origin, to, _refPickInfoOut, _bSelectMode);
			if ( pNode == NULL )
				pNode = *it;
		}

		if ( pNode->GetVisible() == false )
			continue;

		// 인도어 씬노드 선택여부
		// 물체를 선택할때는 인도어 씬노드 물체(scene.xml이 있는물체)는 선택이 안되어야 한다
// 		if( pNode == m_pPickingIgnoreSceneNode && _bSelectMode == true )
// 			continue;

		if ( pNode->GetAABB().ContainsLineSegment(origin, to))
		{
			RPICKINFO kInfo(origin, to, RPM_DETAIL); //선택이든 아니든 피킹은 디테일하게 진행
			if ( pNode->Pick(kInfo))
			{
				float d = kInfo.outPosition.DistanceToSq(origin);
				_refPickInfoOut.insert( RPICKINFO_MAP::value_type( d, kInfo ) );
				pPickedNode = pNode;
			}
		}
	}
	return pPickedNode;
}

bool RToolSceneManager::Pick(const RVector &origin,const RVector &to, RPICKINFO_MAP& _refPickInfoOut, bool _bSelectMode)
{
	_ASSERT( _refPickInfoOut.empty() );
	RSceneNode *pPickedNode = AABBPicking( &GetRootSceneNode().GetChildren(), origin, to, _refPickInfoOut, _bSelectMode);
	return pPickedNode != NULL;
}

bool RToolSceneManager::PickFrustum(RSceneNodeList *pOutList, RViewFrustum &kFrustum)
{
	return ( AABBPickingFrustum(pOutList,&GetRootSceneNode().GetChildren(), kFrustum) > 0 ? true : false);
}


const RVector* RToolSceneManager::GetVerticalContactPos( const RVector& _refPos, const RSceneNode* _pSceneNodeToIgnore )
{
	static RPICKINFO_MAP mapPickInfo;
	static RVector vTerrainPos;

	RVector* pVecPos(NULL);
	mapPickInfo.clear();

	RVector vecVertical( _refPos + RVector( 0, 0, -100000));

	if( GetCurrentTerrain()  && GetCurrentTerrain()->GetPickVertical( vTerrainPos, NULL, _refPos.x, _refPos.y ) )
		pVecPos = &vTerrainPos;

	if (Pick( _refPos, vecVertical, mapPickInfo, false ) )
	{
		RPICKINFO_MAP::iterator itr = mapPickInfo.begin();
		if( itr->second.outNode == _pSceneNodeToIgnore )
			itr++;

		if( itr != mapPickInfo.end() )
		{
			if( pVecPos )
			{
				if( pVecPos->z < itr->second.outPosition.z )
					pVecPos = &(itr->second.outPosition);
			}
			else
				pVecPos = &(itr->second.outPosition);
		}
	}
	return const_cast<const RVector*>(pVecPos);
}

// 바로 위의 함수와 중복이 많다: charmke
const RVector* RToolSceneManager::GetVerticalContactPos( const RVector& _refPos, const RSceneNode* _pSceneNodeToIgnore, RVector* const _outNormal )
{
	static RPICKINFO_MAP mapPickInfo;

	// 중복을 피할 수 없는 이유.
	// 이 변수를 static으로 선언하지 않으면 코어툴의 '선택한 물체 지형에 붙이기' 기능이 제대로 동작하지 않는다.
	// 바로 위의 GetVerticalContactPos()함수도 별도의 static 지역 변수를 가지므로 일단 중복인 채로 놔둔다. : charmke
	static RVector vTerrainPos;

	RVector* pVecPos(NULL);
	mapPickInfo.clear();

	RVector vecVertical( _refPos + RVector( 0, 0, -100000));

	if( GetCurrentTerrain()  && GetCurrentTerrain()->GetPickVertical( vTerrainPos, _outNormal, _refPos.x, _refPos.y ) )
		pVecPos = &vTerrainPos;

	if (Pick( _refPos, vecVertical, mapPickInfo, false ) )
	{
		RPICKINFO_MAP::iterator itr = mapPickInfo.begin();
		if( itr->second.outNode == _pSceneNodeToIgnore )
			itr++;

		if( itr != mapPickInfo.end() )
		{
			if( pVecPos )
			{
				if( pVecPos->z < itr->second.outPosition.z )
					pVecPos = &(itr->second.outPosition);
			}
			else
				pVecPos = &(itr->second.outPosition);
		}
	}
	return const_cast<const RVector*>(pVecPos);
}

bool RToolSceneManager::SaveRootSceneToXML(const char *szFileName )
{
	return RSceneManager::SaveRootSceneToXML(szFileName);
}

void RToolSceneManager::SaveRootSceneToXML(const char *szFileName, MXml& _refMXml)
{
	MXmlDeclaration* pDec = _refMXml.CreateDeclaration("1.0", "UTF-8", "");
	_refMXml.Doc()->LinkEndChild(pDec);
	
	GetRootSceneNode().SetXMLFileName( szFileName );
	SaveXML(&GetRootSceneNode(),_refMXml.Doc());
}

RSceneNode* RToolSceneManager::CreateSceneNode(const char *szNodeName)
{
	if ( szNodeName == NULL )
		return NULL;

	RSceneNode *pNewNode = NULL;

	if(stricmp(szNodeName,RSID_SCENEINSTANCE)==0)
	{
		pNewNode = new RToolSceneNodeInstance;
// 		pNewNode->SetProperty(CSceneNodeInstanceProperty::GetInstance());
	}
	else if(stricmp(szNodeName,RSID_STATICMESHSCENENODE)==0)
	{
		pNewNode = new RToolStaticMeshSceneNode;
// 		pNewNode->SetProperty(CStaticMeshSceneNodeProperty::GetInstance());
	}
	else if(stricmp(szNodeName,RSID_TERRAINSCENENODE)==0)
	{
		RToolTerrain *pTerrain = new RToolTerrain(REngine::GetDevicePtr());
// 		pTerrain->SetProperty(CTerrainSceneNodeProperty::GetInstance());

		pNewNode = (RSceneNode*)pTerrain;
	}
	else if(stricmp(szNodeName,RSID_ACTOR)==0 )
	{
		pNewNode = new RToolActor(m_pControlMediator);
// 		pNewNode->SetProperty(CActorSceneNodeProperty::GetInstance());
	}
	else if(stricmp(szNodeName,RSID_TREE)==0 )
	{
		pNewNode = new RTreeSceneNode;
// 		pNewNode->SetProperty(CTreeSceneNodeProperty::GetInstance());
	}
	else if(stricmp(szNodeName,RSID_LIGHTSCENENODE)==0 )
	{
		pNewNode = new RToolPointLightSceneNode(m_pControlMediator);
// 		pNewNode->SetProperty(CPointLightSceneNodeProperty::GetInstance());
	}
	else if(stricmp(szNodeName,RSID_DIRECTIONALLIGHTSCENENODE)==0 ||
			stricmp(szNodeName,RSID_SUBLIGHTSCENENODE)==0)
	{
		pNewNode = RSceneManager::CreateSceneNode(szNodeName);
// 		pNewNode->SetProperty(CDirectionalLightSceneNodeProperty::GetInstance());
	}
	else if(stricmp(szNodeName,RSID_WATERPLANE)==0 )
	{
		pNewNode = RSceneManager::CreateSceneNode(szNodeName);
// 		pNewNode->SetProperty(CWaterPlaneSceneNodeProperty::GetInstance());
	}
	else if(stricmp(szNodeName,RSID_SPOTLIGHTSCENENODE)==0 )
	{
		pNewNode = RSceneManager::CreateSceneNode(szNodeName);
// 		pNewNode->SetProperty(CSpotLightSceneNodeProperty::GetInstance());
	}
	else if(stricmp(szNodeName,RSID_EFFECTINSTANCE)==0 )
	{
		// 기본 프로퍼티 안쓰도록.. TODO: CControlSceneProperty 계열을 다 파내면 없애자
		pNewNode = RSceneManager::CreateSceneNode(szNodeName);
	}
	else if(stricmp(szNodeName,RSID_OCCLUDER)==0 )
	{
		// 기본 프로퍼티 안쓰도록.. TODO: CControlSceneProperty 계열을 다 파내면 없애자
		pNewNode = RSceneManager::CreateSceneNode(szNodeName);
	}
	else
	{
		pNewNode = RSceneManager::CreateSceneNode(szNodeName);
		if(NULL==pNewNode)
		{
			_ASSERT(0);
			mlog("RSceneManager::CreateSceneNode unknown node %s\n" ,szNodeName);
			return NULL;
		}
		pNewNode->SetUserData(CControlSceneProperty::GetInstance());	// 기본 프로퍼티
	}

	//if(pNewNode)
	//	//pNewNode->m_bCreatedByManager = true;
	//	m_rootSceneNode.AddToSceneNodeGroup(pNewNode);

	return pNewNode;
}

RSceneNode* RToolSceneManager::GetSceneNodeByName(const char* _pSzSceneNodeName)
{
	SSceneNodeFinder::pSzSceneNodeName = const_cast<char*>(_pSzSceneNodeName);
	vector< RSceneNode* > vCulledSceneNode;
	GetRootSceneNode().SerializeToVector( SSceneNodeFinder::FuncGetByName, vCulledSceneNode );
	_ASSERT( vCulledSceneNode.size() <= 1);
	if (vCulledSceneNode.empty())
	{
		return NULL;
	}
	else
		return vCulledSceneNode[0];
}

void RToolSceneManager::RenderPrimaryCamera()
{
	if( m_bWireFrameDrawMode )
	{
		REngine::GetDevice().SetFillMode(RFILL_WIRE);
		RSceneManager::RenderPrimaryCamera();
		REngine::GetDevice().SetFillMode(RFILL_SOLID);
	}
	else
	{
		RSceneManager::RenderPrimaryCamera();
	}
}

void RToolSceneManager::GatherAllActor(const RSceneNode *pSceneNode, RSceneNodeList* pOutList)
{
	if ( MIsExactlyClass( RActor, pSceneNode) )
		pOutList->push_back( (RSceneNode*)pSceneNode );	// const 붙이려면 너무 일이 커져서 캐스팅하였다

	for ( RSceneNodeList::const_iterator it = pSceneNode->GetChildren().begin(); it != pSceneNode->GetChildren().end(); ++it )
	{
		RSceneNode *pChildNode = *it;
		GatherAllActor(pChildNode, pOutList);
	}
}

void RToolSceneManager::GatherAllTrees(const RSceneNode *pSceneNode, RSceneNodeList* pOutList)
{
	if ( MIsExactlyClass( RTreeSceneNode, pSceneNode) )
		pOutList->push_back( (RSceneNode*)pSceneNode );	// const 붙이려면 너무 일이 커져서 캐스팅하였다

	for ( RSceneNodeList::const_iterator it = pSceneNode->GetChildren().begin(); it != pSceneNode->GetChildren().end(); ++it )
	{
		RSceneNode *pChildNode = *it;
		GatherAllTrees(pChildNode, pOutList);
	}
}

void RToolSceneManager::ExportTerrainMaxScript(FILE* file)
{
	RToolTerrain* pTerrain = (RToolTerrain*)GetCurrentTerrain();
	if( NULL == pTerrain )
		return;

	string strBakedMapName = string(pTerrain->GetPath()) + pTerrain->GetBaseMapName();
	string strBakedMapNameMax = MGetPathReplacedBackSlashToSlash(strBakedMapName);

	string strDisMapName = string(pTerrain->GetPath()) + pTerrain->GetDisplacementMapName();
	string strDisMapNameMax = MGetPathReplacedBackSlashToSlash(strDisMapName);

	fprintf(file,"-- 지형 임포트 -----------------------------------------------------------\n" );
	fprintf(file,"ImportTerrain \"%s\" \"%s\" %.3f %.3f\n\n",
		strDisMapNameMax.c_str(), strBakedMapNameMax.c_str(), pTerrain->GetWidthSize(), pTerrain->GetHeightSize() );
}

void RToolSceneManager::ExportLightMapActorMaxScript(FILE* file)
{
	// 모든 라이트맵 액터를 모은다
	RSceneNodeList outLightMapActorList;
	GatherAllActor(&GetRootSceneNode(), &outLightMapActorList);

	// 액터를 메쉬 단위로 정렬하여 모은다
	typedef map< string , RSceneNodeList > MAXFILETOINSTANCEMAP;
	MAXFILETOINSTANCEMAP mapInstances; // max filename -> list of instance

	for ( RSceneNodeList::const_iterator it = outLightMapActorList.begin(); it != outLightMapActorList.end(); ++it )
	{
		RSceneNode *pNode = *it;
		RActor* pActor = MStaticCast( RActor, pNode );

		// RefferenceMesh는 출력하지 않는다
		if ( stricmp(pActor->GetNodeName().c_str(), "RefferenceMesh") == 0 )
			continue;
		
		MAXFILETOINSTANCEMAP::iterator itrInstances;
		itrInstances = mapInstances.find( pActor->GetMeshName());
		if(itrInstances != mapInstances.end())
			itrInstances->second.push_back( pNode );
		else
		{
			RSceneNodeList listInstances;
			listInstances.push_back( pNode );
			
			mapInstances.insert( MAXFILETOINSTANCEMAP::value_type(pActor->GetMeshName(), listInstances) );
		}
	}

	fprintf(file,"-- Actor 임포트 ----------------------------------------------------------\n" );

	// 각 메쉬에 대해서 인스턴스로 복사해서 붙이는 스크립트 출력
	for ( MAXFILETOINSTANCEMAP::iterator itMap = mapInstances.begin(); itMap!= mapInstances.end(); ++itMap)
	{
		char szMaxFileName[256];
		ReplaceExtension(szMaxFileName,itMap->first.c_str(),"max");
		fprintf(file, "maxfile = getFilesRecursive pathXRefSearch \"%s\"\n", szMaxFileName);
		fprintf(file, "bSuccess = XRefMAXFile maxfile\n");

		fprintf(file, "if(bSuccess) then (\n");
		
		RSceneNodeList& listNodes = itMap->second;

		for( RSceneNodeList::iterator itNode = listNodes.begin(); itNode != listNodes.end(); ++itNode)
		{
			// 마지막 노드이면 선택을 직접 쓰고 아니면 인스턴스로 복사
			if(*itNode == listNodes.back() )
			{
				fprintf(file, "\tnewNodes = selection\n");
			}else
				fprintf(file, "\tmaxOps.CloneNodes selection cloneType:#instance newNodes:&newNodes\n");

			const RMatrix& matInstance = (*itNode)->GetWorldTransform();
			fprintf(file, "\ttm = (matrix3 ");
			for( int iRow = 0; iRow<4; iRow++)
			{
				fprintf(file, "[%3.3f,%3.3f,%3.3f] ", 
					matInstance._m[iRow][0],
					matInstance._m[iRow][1],
					matInstance._m[iRow][2] );
			}
			fprintf(file, "\t)\n\tfor t in newNodes do (\n");
			fprintf(file, "\t\tif(t.parent == undefined) then (t.transform = t.transform * tm) \n");
			fprintf(file, "\t)\n");
		}
		fprintf(file, ")\n");
	}
	fprintf(file, "\n");
}

void RToolSceneManager::ExportSpeedTreeMaxScript(FILE* file)
{
	// SpeedTree 빌보드가 DirLight에 Align되도록 더미를 놓을 오프셋 값을 구한다.
	RVector lightDir;
	RDirectionalLightSceneNode* pDirLight = NULL;
	if( NULL != GetCurrentRootSceneNode(false) )
	{
		pDirLight = GetCurrentRootSceneNode(false)->GetDirectionalLight();
	}

	if(pDirLight)
	{
		lightDir = -pDirLight->GetDirection();
	}
	else
	{
		// 라이트가 없으면 Z+를 라이트 방향으로
		lightDir = RVector::AXISZ;
	}

	RSceneNodeList outLightmapTreeList;
	GatherAllTrees(&GetRootSceneNode(), &outLightmapTreeList);

	fprintf(file,"-- SpeedTree 임포트 ------------------------------------------------------\n" );

	for(RSceneNodeList::iterator it = outLightmapTreeList.begin(); it != outLightmapTreeList.end(); ++it)
	{
		RTreeSceneNode* pTreeSceneNode = MStaticCast( RTreeSceneNode, (*it) );

		fprintf(file, "(\n");

			const RMatrix& matInstance = pTreeSceneNode->GetWorldTransform();
			fprintf(file, "\tlocal tm = matrix3 ");
			for( int iRow = 0; iRow<4; iRow++)
			{
				fprintf(file, "[%3.3f,%3.3f,%3.3f] ", 
					matInstance._m[iRow][0],
					matInstance._m[iRow][1],
					matInstance._m[iRow][2] );
			}
			fprintf(file, "\n");

			fprintf(file, "\timportSpeedTree \"%s\" \"%s\" tm\n", pTreeSceneNode->GetTreeFileName().c_str(), pTreeSceneNode->GetNodeName().c_str() );

		fprintf(file, ")\n");
	}

	fprintf(file, "\n");
}

void RToolSceneManager::ExportMaxScript(const char* szMaxScriptFileName)
{
	FILE* file = fopen( szMaxScriptFileName, "w+" );

	fprintf(file, "global strCurrentFilePath = getFilenamePath ( getSourceFileName() )\n" );
	fprintf(file, "fileIn ( strCurrentFilePath + \"../../../Dev/MAIETExporters/zone.ms\" )\n\n" );

	fprintf(file,"/*\n" );
	fprintf(file,"*/\n\n" );

	//////////////////////////////////////////////////////////////////////////
	// 지형 익스포트
	ExportTerrainMaxScript(file);

	//////////////////////////////////////////////////////////////////////////
	// Actor 익스포트
	ExportLightMapActorMaxScript(file);

	//////////////////////////////////////////////////////////////////////////
	// SpeedTree 라이트맵 익스포트
	ExportSpeedTreeMaxScript(file);

	//////////////////////////////////////////////////////////////////////////
	// target- directional light 생성
	RDirectionalLightSceneNode* pDirLight = GetDefaultLight();
	if(pDirLight)
	{
		fprintf(file,"-- Directional light 임포트 ----------------------------------------------\n" );

		fprintf(file, "c = TargetDirectionallight pos:[0,0,0] target:(targetObject pos:[%3.3f,%3.3f,%3.3f])\n",
			pDirLight->GetDirection().x, pDirLight->GetDirection().y, pDirLight->GetDirection().z );
		fprintf(file, "c.overShoot = true\n");
	}

	fprintf(file, "\n");
	fprintf(file,"-- 에러 메시지 출력 ; 주석 처리 하지 마세요-------------------------------\n" );
	fprintf(file, "displayNotExistSpeedFiles()\n");

	fprintf(file, "\n\n");
	fclose(file);
}

void RToolSceneManager::SaveResourceListToXML( const char *szFileName )
{
	typedef std::map< std::string, std::pair< int,std::vector<std::string> > > RES_MAP;
	RES_MAP res_instance_map;

	// managed resource
	for(int i = 0; i < RRESOURCETYPE::RR_MAX; ++i)
	{
		RResourceManager* pManager = GetResourceMgrFromID<RRESOURCETYPE>(i);
		if (!pManager)
			continue;

		std::string strResType = RRESOURCETYPE::GetResoruceIDString(i);
		res_instance_map[strResType].first = i;

		const RRESOURCEHASHMAP& res_mgr_map = pManager->GetResourceMap();
		for (RRESOURCEHASHMAP::const_iterator itr = res_mgr_map.begin(); itr != res_mgr_map.end(); ++itr)
		{
			if (itr->first.find(":\\") != std::string::npos)
				continue;

			res_instance_map[strResType].second.push_back(itr->first);
		}
	}

	// unmanaged resource : texture
	std::string strTextureType("texture");
	res_instance_map[strTextureType].first = -1;
	const RTextureManager* pTextureManager = REngine::GetDevice().GetTextureManager();
	const RRESOURCEHASHMAP& res_tex_mgr_map = pTextureManager->GetResourceMap();
	for (RRESOURCEHASHMAP::const_iterator itr = res_tex_mgr_map.begin(); itr != res_tex_mgr_map.end(); ++itr)
	{
		if (itr->first.find(":\\") != std::string::npos)
			continue;

		res_instance_map[strTextureType].second.push_back(itr->first);
	}

	MXml xml;
	MXmlDeclaration* pDec = xml.CreateDeclaration("1.0", "UTF-8", "");
	xml.Doc()->LinkEndChild(pDec);

	MXmlElement* pRoot = new MXmlElement("ROOT");
	xml.Doc()->LinkEndChild(pRoot);

	for (RES_MAP::iterator itr = res_instance_map.begin(); itr != res_instance_map.end(); ++itr)
	{
		MXmlElement* pResElement = new MXmlElement("RESOURCE");

		_SetAttribute(pResElement, "type", itr->first.c_str());
		_SetAttribute(pResElement, "id", itr->second.first);

		int cnt = 0;
		for (std::vector<std::string>::iterator itr_res_name = itr->second.second.begin(); itr_res_name != itr->second.second.end(); ++itr_res_name)
		{
			MXmlElement* pListElement = new MXmlElement("li");
			string resFileName = MGetPureFileNameExt( *itr_res_name );
			_SetAttribute(pListElement, "name", resFileName.c_str());
			pResElement->LinkEndChild(pListElement);
			++cnt;
		}

		_SetAttribute(pResElement, "cnt", cnt);

		pRoot->LinkEndChild(pResElement);
	}

	xml.SaveFile(szFileName);
}

bool RToolSceneManager::MakeSceneNodeInstanceEditable(RSceneNodeInstance* pSceneInstance, RSceneNodeList* pOutList)
{
	CLogicObject* pLogicObjectSceneInstance = GetLogicObjectLinkedTo( pSceneInstance );
	string strLayerName = pLogicObjectSceneInstance->GetLayerName();

	DetachLogicObject( pSceneInstance );

	RSceneNodeList nodeList;
	RToolSceneNodeInstance::ConvertToAsCopy( pSceneInstance, &nodeList );

 	for(RSceneNodeList::iterator itr = nodeList.begin(); itr != nodeList.end(); ++itr )
 	{
 		// attach logic object
 		rs3::RSceneNode* pNode =  *itr;

		CEditableNodeCollector editableNodeCollector;
		pNode->Traverse(&editableNodeCollector);

		for(std::vector<RSceneNode*>::iterator iter = editableNodeCollector.m_SceneNodes.begin();iter != editableNodeCollector.m_SceneNodes.end(); iter++)
		{
			RSceneNode* pSceneNode = (*iter);
			CLogicObject* pLogicObject = CreateLogicObject( pSceneNode );
			if(pLogicObject)
				pLogicObject->SetLayerName( strLayerName.c_str() );

			m_pControlMediator->GetToolLogicObjectMgr()->AddToManager(pLogicObject);
		}
 	}

	// 신이 바뀌었다는것을 통보,
	m_pControlMediator->BroadCastMessage( VIEW_MESSAGE::SCENE_BREAK_INSTANCE );
	
	// 자식노드 리스트 복사
	if(pOutList)
		*pOutList = nodeList;

	return true;
}

void RToolSceneManager::ImportScene(const char *szFileName)
{
	using namespace rs3;

	RWorldSceneNode& rootSceneNode = GetRootSceneNode();
	std::string strOrgPath = rootSceneNode.GetXMLFileName();

	RToolSceneNodeInstance* pSceneInstance = static_cast<RToolSceneNodeInstance*>(CreateSceneNode(RSID_SCENEINSTANCE));
	if( pSceneInstance->Create( szFileName, false ) )
	{
		AttachLogicObjectTraverse( pSceneInstance );
		UpdateRootSceneTransform();

		// 경로+파일명+확장자 중 경로를 제거한 파일명+확장자를 얻는다
		string strFileName = MGetRemovePathName( szFileName );
		int nNumber = CXMLEditInfoFile::GetReference().GetNumbering(&strFileName);
		char szBuffer[256];
		szBuffer[0] = '\0';
		sprintf(szBuffer,"%s_%03d", (strFileName.substr( 0, strFileName.length() - 4 )).c_str(), nNumber);
		pSceneInstance->SetNodeName(szBuffer);

		rootSceneNode.AddChild( pSceneInstance );
		rootSceneNode.AddToSceneNodeGroup( pSceneInstance );
	}

	rootSceneNode.SetXMLFileName(strOrgPath);

	m_pControlMediator->GetHelperResource()->SetAutoAnimationProperty();
	m_pControlMediator->GetHelperResource()->SetMediatorAfterSceneLoaded();

	m_pControlMediator->BroadCastMessage(VIEW_MESSAGE::SCENE_IMPORTED);
}

// 확장자 제거한 이펙트 이름 얻기
string RToolSceneManager::GetEffectNameFromFileName( const string& strEffectFileName )
{
	const char* szToRemove = ".effect.scene.xml";

	unsigned int nExtPos = strEffectFileName.find( szToRemove );
	if( nExtPos != string::npos )
		return strEffectFileName.substr(0, nExtPos );
	
	return strEffectFileName;
}

bool RToolSceneManager::LoadSceneNodeFromFile(RSceneNode* pSceneNode,const char* szFileName)
{
	_ASSERT( pSceneNode );
	_ASSERT( szFileName );

	const char* szNodeIDString = pSceneNode->GetNodeIDString();
	bool bRet = false;

	// TODO: WorldSceneNode, SceneInstance, EffectInstance 등이 Scene.xml 로 부터 생성하는 같은 로직이므로
	// 인터페이스를 통일하고 싶다, base class 가 있어도 좋을듯

	if ( strcmp(szNodeIDString,RSID_WORLDSCENENODE) == 0 )
	{
		bRet = LoadSceneNodeFromRootXMLFile(pSceneNode,szFileName,false);
	}
	else if  ( strcmp(szNodeIDString,RSID_SCENEINSTANCE) == 0 ) 
	{
		bRet = static_cast<RSceneNodeInstance*>(pSceneNode)->Create( szFileName, false );
	}
	else if ( strcmp(szNodeIDString,RSID_EFFECTINSTANCE) == 0 )
	{
		string strPureEffectName = GetEffectNameFromFileName( szFileName );

		bRet = static_cast<REffectInstanceSceneNode*>(pSceneNode)->Create(strPureEffectName.c_str(), false);
	}
	else if ( strcmp(szNodeIDString,RSID_ACTOR) == 0 )
	{
		bRet = static_cast<RActor*>(pSceneNode)->Create(szFileName, false);
	}
	else if ( strcmp(szNodeIDString,RSID_TREE) == 0 )
	{
		bRet = static_cast< RTreeSceneNode* >(pSceneNode)->Build(szFileName, false);
	}
	else
	{
		mlog( "%s 은 xml 파일을 씬노드 개별 특성에 맞게 열것입니다.\n", szFileName);
		MXml xml;
		xml.LoadFile(szFileName);
		MXmlElement* xmlElement = xml.Doc()->FirstChildElement(pSceneNode->GetNodeIDString());
		bRet = LoadXML( pSceneNode, *xmlElement, false);
	}

	return bRet;

}

} // end of namepsace