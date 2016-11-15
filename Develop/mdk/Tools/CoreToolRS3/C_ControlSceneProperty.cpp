#include "stdafx.h"
#include "C_ControlSceneProperty.h"

#include "RTreeSceneNode.h"
#include "M_ToolActor.h"
#include "M_ToolSceneNodeInstance.h"
#include "M_ToolPointLight.h"

#include "VL_SceneNodeList.h"
#include "VR_ZoneObjectNew.h"

//#include "MainFrm.h"
#include "C_ControlMediator.h"
#include "C_ControlMediatorHelperAction.h"
#include "C_XMLEditInfoFile.h"

#include "RConfiguration.h"
#include "RMesh.h"

#include "RWaterSceneNode.h"
#include "RStaticMeshSceneNode.h"

#include "RSpotLightSceneNode.h"
#include "MFileSystem.h"
#include "M_LogicObject_SceneNodes.h"
#include "M_ToolCmdMgr.h"
#include "RUtil.h"


const string NONE_NAME("");

using namespace rs3;

CControlMediator* CControlSceneProperty::m_pControlMediator = NULL;

CControlSceneProperty::CControlSceneProperty()
: m_pScaleProperty(NULL)
, m_pPosGroup(NULL)
, m_pDirGroup(NULL)
, m_pUpGroup(NULL)
{
}

CControlSceneProperty::~CControlSceneProperty()
{
}

CControlSceneProperty* CControlSceneProperty::GetInstance()
{
	static CControlSceneProperty instance;
	return &instance;
}

void CControlSceneProperty::Init(CObjectPropertyGridCtrl* pPropertyUI, rs3::RSceneNode* pSceneNode, bool bEditable)
{
	// scenenode 에서 속성값들을 가져온다
	m_nodeFileName = pSceneNode->GetNodeName().c_str();

	rs3::RVector vPos = pSceneNode->GetPosition();
	m_posX = vPos.x;
	m_posY = vPos.y;
	m_posZ = vPos.z;

	m_scale = pSceneNode->GetScale().x;

	rs3::RVector vDir = pSceneNode->GetDirection();
	m_dirX = vDir.x;
	m_dirY = vDir.y;
	m_dirZ = vDir.z;

	rs3::RVector vUp = pSceneNode->GetUp();
	m_upX = vUp.x;
	m_upY = vUp.y;
	m_upZ = vUp.z;


	CMFCPropertyGridProperty* groupCommon = new CMFCPropertyGridProperty( _T("공통속성") );

	pPropertyUI->AddPropertyItem( groupCommon, "이름", m_nodeFileName, bEditable );

	m_pPosGroup = pPropertyUI->AddPropertyGroupItem( groupCommon, "위치", bEditable );
		pPropertyUI->AddPropertyItem( m_pPosGroup, "X", m_posX, bEditable );
		pPropertyUI->AddPropertyItem( m_pPosGroup, "Y", m_posY, bEditable );
		pPropertyUI->AddPropertyItem( m_pPosGroup, "Z", m_posZ, bEditable );
	
	m_pScaleProperty = pPropertyUI->AddPropertyItem( groupCommon, "스케일", m_scale, bEditable );

	m_pDirGroup = pPropertyUI->AddPropertyGroupItem( groupCommon, "방향", bEditable );
		pPropertyUI->AddPropertyItem( m_pDirGroup, "X", m_dirX, bEditable );
		pPropertyUI->AddPropertyItem( m_pDirGroup, "Y", m_dirY, bEditable );
		pPropertyUI->AddPropertyItem( m_pDirGroup, "Z", m_dirZ, bEditable );

	m_pUpGroup = pPropertyUI->AddPropertyGroupItem( groupCommon, "UP", bEditable );
		pPropertyUI->AddPropertyItem( m_pUpGroup, "X", m_upX, bEditable );
		pPropertyUI->AddPropertyItem( m_pUpGroup, "Y", m_upY, bEditable );
		pPropertyUI->AddPropertyItem( m_pUpGroup, "Z", m_upZ, bEditable );
	m_pUpGroup->Expand(TRUE);

	pPropertyUI->AddProperty( groupCommon );
}

void CControlSceneProperty::OnPropertyChanged(CMFCPropertyGridProperty* propertyItem, rs3::RSceneNode* pSceneNode)
{
	USES_CONVERSION;
	string nodeName = W2A( m_nodeFileName.bstrVal );
	pSceneNode->SetNodeName( nodeName );
// 	pSceneNode->SetPosition( rs3::RVector( m_posX, m_posY, m_posZ ) );
// 	pSceneNode->SetScale( rs3::RVector( m_scale, m_scale, m_scale ) );
// 	pSceneNode->SetDirection( rs3::RVector( m_dirX, m_dirY, m_dirZ ), rs3::RVector( m_upX, m_upY, m_upZ ) );

	CLogicObject* pLogicObject = dynamic_cast<CLogicObject*>((CCoreObject*)pSceneNode->GetUserData());
	_ASSERT(pLogicObject);

	// 한번에 다 업데이트 하면 한프레임에 한번씩만 업데이트 하도록한 신노드의 정책때문에 방향,스케일이 업데이트가 안된다
	// 한번에 하나씩만 업데이트 하도록 수정됨
	if(IsPositionProperty(propertyItem))
		pLogicObject->SetPosition( rs3::RVector( m_posX, m_posY, m_posZ ) );
	if(IsDirectionProperty(propertyItem))
		pLogicObject->SetDirectionUp( rs3::RVector( m_dirX, m_dirY, m_dirZ ), rs3::RVector( m_upX, m_upY, m_upZ ) );
	if(IsScaleProperty(propertyItem))
		pLogicObject->SetScale( m_scale );

	m_pControlMediator->BroadCastMessage( CoreMessage(VIEW_MESSAGE::LOGIC_MOVE, pLogicObject) );

	// control mediator
	m_pControlMediator->GetHelperAction()->SetSelectedObjectCenterAndActionFrame();
}

//////////////////////////////////////////////////////////////////////////
// terrain property

CTerrainSceneNodeProperty* CTerrainSceneNodeProperty::GetInstance()
{
	static CTerrainSceneNodeProperty instance;
	return &instance;
}

void CTerrainSceneNodeProperty::Init( CObjectPropertyGridCtrl* pPropertyUI, rs3::RSceneNode* pSceneNode, bool bEditable /*= true*/ )
{
	CControlSceneProperty::Init(pPropertyUI, pSceneNode, bEditable);

	RTerrain* pTerrain = dynamic_cast<RTerrain*>(pSceneNode);
	_ASSERT(pTerrain);

	m_bCreateNaviMesh = pTerrain->getCreateNaviMesh();

	CMFCPropertyGridProperty* groupActor = new CMFCPropertyGridProperty( _T("Terrain 속성") );
	pPropertyUI->AddPropertyItem( groupActor, "네비메쉬생성", m_bCreateNaviMesh, bEditable );
	pPropertyUI->AddProperty( groupActor );
}

void CTerrainSceneNodeProperty::OnPropertyChanged( CMFCPropertyGridProperty* propertyItem, rs3::RSceneNode* pSceneNode )
{
	CControlSceneProperty::OnPropertyChanged(propertyItem,pSceneNode);

	RTerrain* pTerrain = dynamic_cast<RTerrain*>(pSceneNode);
	_ASSERT(pTerrain);

	pTerrain->setCreateNaviMesh(m_bCreateNaviMesh);
}

//////////////////////////////////////////////////////////////////////////
// staticmesh property
CStaticMeshSceneNodeProperty* CStaticMeshSceneNodeProperty::GetInstance()
{
	static CStaticMeshSceneNodeProperty instance;
	return &instance;
}

void CStaticMeshSceneNodeProperty::Init( CObjectPropertyGridCtrl* pPropertyUI, rs3::RSceneNode* pSceneNode, bool bEditable /*= true*/ )
{
	CControlSceneProperty::Init(pPropertyUI,pSceneNode, bEditable);

	RStaticMeshSceneNode* pStaticMesh = dynamic_cast<RStaticMeshSceneNode*>(pSceneNode);
	_ASSERT(pStaticMesh);

	m_bEnablePicking = pStaticMesh->getEnablePicking();

	CMFCPropertyGridProperty* groupActor = new CMFCPropertyGridProperty( _T("StaticMesh 속성") );
	pPropertyUI->AddPropertyItem( groupActor, "피킹될것인가?", m_bEnablePicking, bEditable );
	pPropertyUI->AddProperty( groupActor );
}

void CStaticMeshSceneNodeProperty::OnPropertyChanged( CMFCPropertyGridProperty* propertyItem, rs3::RSceneNode* pSceneNode )
{
	CControlSceneProperty::OnPropertyChanged(propertyItem,pSceneNode);

	RStaticMeshSceneNode* pStaticMesh = dynamic_cast<RStaticMeshSceneNode*>(pSceneNode);
	_ASSERT(pStaticMesh);

	pStaticMesh->setEnalbePicking(m_bEnablePicking);
}

//////////////////////////////////////////////////////////////////////////
// SceneNodeInstance Property 
CSceneNodeInstanceProperty* CSceneNodeInstanceProperty::GetInstance()
{
	static CSceneNodeInstanceProperty instance;
	return &instance;
}

void CSceneNodeInstanceProperty::SetNewInstance(rs3::RSceneNode* pNewSceneNode, rs3::RSceneNode* pOldSceneNode, bool _bCopyMode)
{
	RToolSceneNodeInstance* pNewScene = dynamic_cast<RToolSceneNodeInstance*>(pNewSceneNode);
	RToolSceneNodeInstance* pOldScene = dynamic_cast<RToolSceneNodeInstance*>(pOldSceneNode);
	_ASSERT(pNewScene);
	_ASSERT(pOldScene);

	// 경로+파일명+확장자 중 경로를 제거한 파일명+확장자를 얻는다
	string strFileName = MGetRemovePathName( pNewScene->GetFileName() );
	int nNumber = CXMLEditInfoFile::GetReference().GetNumbering(&strFileName);
	char szBuffer[256];
	szBuffer[0] = '\0';
	sprintf(szBuffer,"%s_%03d", (strFileName.substr( 0, strFileName.length() - 4 )).c_str(), nNumber);
	pNewScene->SetNodeName(szBuffer);
}

void CSceneNodeInstanceProperty::Init(CObjectPropertyGridCtrl* pPropertyUI, rs3::RSceneNode* pSceneNode, bool bEditable)
{
	CControlSceneProperty::Init(pPropertyUI,pSceneNode, bEditable);

	RToolSceneNodeInstance* pScene = dynamic_cast<RToolSceneNodeInstance*>(pSceneNode);
	_ASSERT(pScene);

	m_bEditable = false;

	CMFCPropertyGridProperty* pGroup = new CMFCPropertyGridProperty( _T("SceneNodeInstance 속성") );
	m_pPropertyEditable = pPropertyUI->AddPropertyItem( pGroup, "편집가능", m_bEditable, bEditable );

	pPropertyUI->AddProperty( pGroup );
}

void CSceneNodeInstanceProperty::OnPropertyChanged(CMFCPropertyGridProperty* propertyItem, rs3::RSceneNode* pSceneNode)
{
	CControlSceneProperty::OnPropertyChanged(propertyItem,pSceneNode);

	RToolSceneNodeInstance* pScene = dynamic_cast<RToolSceneNodeInstance*>(pSceneNode);
	_ASSERT(pScene);

	if( propertyItem == m_pPropertyEditable)
	{
		// editable -> not editable 로 갈때는 경고
		if( m_bEditable )
		{
			int nYesNo = MessageBox(NULL, "수정하면 되돌릴수 없습니다. 계속할까요?", "경고", MB_YESNO);
			if (  nYesNo == IDYES )
			{
				RToolSceneManager* pSceneManager = m_pControlMediator->GetToolSceneManager();
				
				RSceneNodeList nodeList;
				pSceneManager->MakeSceneNodeInstanceEditable( pScene, &nodeList );

				CToolCmdMgr* pCommandManager = m_pControlMediator->GetToolCmdMgr();
				pCommandManager->ResetCommandManager();
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// tree property
CTreeSceneNodeProperty* CTreeSceneNodeProperty::GetInstance()
{
	static CTreeSceneNodeProperty instance;
	return &instance;
}

void CTreeSceneNodeProperty::SetNewInstance(rs3::RSceneNode* pNewSceneNode, rs3::RSceneNode* pOldSceneNode, bool _bCopyMode)
{
	RTreeSceneNode* pTreeInstance = dynamic_cast< RTreeSceneNode* >(pNewSceneNode);
	_ASSERT(pTreeInstance);

	string strFileName(pTreeInstance->GetTreeFileName());
	int nNumber = CXMLEditInfoFile::GetReference().GetNumbering(&strFileName);
	char szBuffer[256];
	szBuffer[0] = '\0';
	sprintf(szBuffer,"%s_%03d", (strFileName.substr( 0, strFileName.length() - 4 )).c_str(), nNumber);

	pTreeInstance->SetNodeName(szBuffer);
}

void CTreeSceneNodeProperty::Init(CObjectPropertyGridCtrl* pPropertyUI, rs3::RSceneNode* pSceneNode, bool bEditable)
{
	CControlSceneProperty::Init(pPropertyUI,pSceneNode, bEditable);

	RTreeSceneNode* pTreeInstance = dynamic_cast< RTreeSceneNode* >(pSceneNode);
	_ASSERT(pTreeInstance);
}

void CTreeSceneNodeProperty::OnPropertyChanged(CMFCPropertyGridProperty* propertyItem, rs3::RSceneNode* pSceneNode)
{
	CControlSceneProperty::OnPropertyChanged(propertyItem,pSceneNode);

	RTreeSceneNode* pTreeInstance = dynamic_cast< RTreeSceneNode* >(pSceneNode);
	_ASSERT(pTreeInstance);
}


//////////////////////////////////////////////////////////////////////////
// waterplane property

CWaterPlaneSceneNodeProperty* CWaterPlaneSceneNodeProperty::GetInstance()
{
	static CWaterPlaneSceneNodeProperty instance;
	return &instance;
}

void CWaterPlaneSceneNodeProperty::Init(CObjectPropertyGridCtrl* pPropertyUI, rs3::RSceneNode* pSceneNode, bool bEditable)
{
	RWaterPlane* pWaterInstance = dynamic_cast<RWaterPlane*>(pSceneNode);

	CControlSceneProperty::Init(pPropertyUI,pSceneNode, bEditable);

	m_fScale = pWaterInstance->GetWaterPlaneProperty()->m_fScale;
	m_fAmplitude1 = pWaterInstance->GetWaterPlaneProperty()->m_fAmplitude1;
	m_fPhase1 =pWaterInstance->GetWaterPlaneProperty()->m_fPhase1;
	m_fUnderWaterDepth =pWaterInstance->GetWaterPlaneProperty()->m_fUnderWaterDepth;
	m_fBlending = pWaterInstance->GetWaterPlaneProperty()->m_fBlending;
	m_strShoreTexture = pWaterInstance->GetWaterPlaneProperty()->m_strShoreTexture.c_str();
	m_fShoreIntensity = pWaterInstance->GetWaterPlaneProperty()->m_fShoreIntensity;
	m_fShoreUScale = pWaterInstance->GetWaterPlaneProperty()->m_vShoreUVScale.x;
	m_fShoreVScale = pWaterInstance->GetWaterPlaneProperty()->m_vShoreUVScale.y;
	m_fRefractAttenuationDist = pWaterInstance->GetWaterPlaneProperty()->m_fRefractAttenuationDist;
	m_fRefractAttenuationFactor = pWaterInstance->GetWaterPlaneProperty()->m_fRefractAttenuationFactor;

	CMFCPropertyGridProperty* groupWater = new CMFCPropertyGridProperty( _T("Water Plane") );
	pPropertyUI->AddPropertyItem( groupWater, "블렌딩", m_fBlending, bEditable );
	m_pShoreTexProperty = pPropertyUI->AddPropertyFileItem(groupWater, "물가 텍스쳐", m_strShoreTexture, bEditable);
	pPropertyUI->AddPropertyItem(groupWater, "물가 텍스쳐 세기", m_fShoreIntensity, bEditable);
	pPropertyUI->AddPropertyItem( groupWater, "물가 텍스쳐 UV Scale(U)", m_fShoreUScale, bEditable );
	pPropertyUI->AddPropertyItem( groupWater, "물가 텍스쳐 UV Scale(V)", m_fShoreVScale, bEditable );
	pPropertyUI->AddPropertyItem( groupWater, "굴절 감쇠 거리", m_fRefractAttenuationDist, bEditable );	
	pPropertyUI->AddPropertyItem( groupWater, "굴절 감쇠율", m_fRefractAttenuationFactor, bEditable );	
	pPropertyUI->AddProperty( groupWater );

	CMFCPropertyGridProperty* groupVertexWave = new CMFCPropertyGridProperty( _T("Vertex Wave") );
	pPropertyUI->AddPropertyItem( groupVertexWave, "전체크기", m_fScale, bEditable );
	pPropertyUI->AddPropertyItem( groupVertexWave, "높이", m_fAmplitude1, bEditable );
	pPropertyUI->AddPropertyItem( groupVertexWave, "속도", m_fPhase1, bEditable );
	pPropertyUI->AddProperty( groupVertexWave );

	CMFCPropertyGridProperty*  groupUnderWater = new CMFCPropertyGridProperty( _T("UnderWater") );
	pPropertyUI->AddPropertyItem( groupUnderWater, "수중효과 적용 깊이", m_fUnderWaterDepth, bEditable );
	pPropertyUI->AddProperty( groupUnderWater );
}

void CWaterPlaneSceneNodeProperty::OnPropertyChanged(CMFCPropertyGridProperty* propertyItem, rs3::RSceneNode* pSceneNode)
{
	RWaterPlane* pWaterInstance = dynamic_cast<RWaterPlane*>(pSceneNode);

	CControlSceneProperty::OnPropertyChanged(propertyItem,pSceneNode);

	pWaterInstance->GetWaterPlaneProperty()->m_fScale =m_fScale;
	pWaterInstance->GetWaterPlaneProperty()->m_fAmplitude1 =m_fAmplitude1;
	pWaterInstance->GetWaterPlaneProperty()->m_fPhase1 =m_fPhase1;
	pWaterInstance->GetWaterPlaneProperty()->m_fUnderWaterDepth =m_fUnderWaterDepth;
	pWaterInstance->GetWaterPlaneProperty()->m_fBlending =m_fBlending;
	pWaterInstance->GetWaterPlaneProperty()->m_fShoreIntensity =m_fShoreIntensity;
	pWaterInstance->GetWaterPlaneProperty()->m_vShoreUVScale =RVector2(m_fShoreUScale, m_fShoreVScale);
	pWaterInstance->GetWaterPlaneProperty()->m_fRefractAttenuationDist = max((float)m_fRefractAttenuationDist, 1.0f);
	pWaterInstance->GetWaterPlaneProperty()->m_fRefractAttenuationFactor = m_fRefractAttenuationFactor;
	
	if(propertyItem==m_pShoreTexProperty)
	{
		m_strShoreTexture = propertyItem->GetValue();
		char strFileName[128];
		SplitPath(m_strShoreTexture, NULL, strFileName);

		pWaterInstance->GetWaterPlaneProperty()->m_strShoreTexture = strFileName;
		pWaterInstance->LoadShoreTexture();
	}
}
