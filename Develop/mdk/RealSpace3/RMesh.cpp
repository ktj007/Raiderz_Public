#include "stdafx.h"
#include <stdio.h>
#include <math.h>
#include <tchar.h>

#include "MXml.h"

#include "RApplication.h"
#include "RDeviceD3D.h"

#include "RMesh.h"
#include "RMeshMgr.h"
#include "RMeshNode.h"

#include "MDebug.h"
#include "MProfiler.h"

#include "RMeshAnimationMgr.h"
#include "RPointLightSceneNode.h"
//#include "RActorMgr.h"

#include "MFileSystem.h"
#include "fileinfo.h"

//#include "RShaderMgr.h"

#include "RUtil.h"

#include "RAnimationNode.h"
#include "RMeshCollision.h"

#include "RMaterialResource.h"
#include "RMaterialResourceMgr.h"
#include "RProfileInfo.h"
#include "RAnimationResource.h"
#include "RFile.h"

//#ifndef _PUBLISH
//
//#define PFI_B(i,n)	MBeginProfile(i,n);
//#define PFI_E(i)		MEndProfile(i);
//
//#else
//
//#define PFI_B(i,n) ;
//#define PFI_E(i) ;
//
//#endif

#include "RMeshNodeLoadImpl.h"
#include "RRule.h"

namespace rs3 {

//////////////////////////////////////////////////////////////////////////////////
// RMesh Class

RMaterialResource* RMesh::m_pDefaultMaterial  = NULL;

bool RMesh::InitSharedResources(RDevice *pDevice)
{
	_ASSERT(pDevice);

	m_pDefaultMaterial = REngine::GetSceneManager().CreateResource< RMaterialResource >( "default" );
	m_pDefaultMaterial->CreateAsDefaultMaterial();

	return true;
}

bool RMesh::DestroySharedResources()
{
	if(m_pDefaultMaterial)
	{
		REngine::GetSceneManager().ReleaseResource( m_pDefaultMaterial );
		m_pDefaultMaterial = NULL;
	}

	return true;
}

RMesh::RMesh(const char* _pSzResourceName, RSceneManager* _pSceneManager) :
	RResource( _pSzResourceName, RRESOURCETYPE::RR_MESH ),
	m_pMeshCollision(NULL),
	m_nID(-1),
	m_nDataNum(0),
	m_PickingType( pick_collision_mesh ),
	m_isPhysiqueMesh(false),
	m_isAlpha2PassRendering(false),
	m_bAutoAnimation(false),
	m_nMaterialUsingSystemMemory(0),
	m_nMaterialUsingVideoMemory(0),
	m_nVertexUsingSystemMemory(0),
	m_nVertexUsingVideoMemory(0),
	m_bExistAnimationListFile(false)
{
	m_pSceneManager = _pSceneManager;
}

// RMesh 소멸자는 반드시 디바이스가 살아있을동안에 불립니다.
RMesh::~RMesh() 
{
	Erase();
}

// 소멸자는 Erase()를 호출 - 090608, OZ
bool RMesh::Erase()	
{
	for(int i=0;i<(int)m_vecMtrlList.size();i++)
	{
		REngine::GetSceneManager().ReleaseResource( m_vecMtrlList.at(i) );
	}
	m_vecMtrlList.clear();

	DelMeshList();

	SAFE_DELETE(m_pMeshCollision);

// 	if(m_meshLightList.empty() == false )
// 	{
// 		for ( vector<RSceneNode*>::iterator itr = m_meshLightList.begin(); itr != m_meshLightList.end(); ++itr )
// 		{
// 			delete *itr;
// 		}
// 	}

	return true;
}

bool RMesh::Fill()
{
	// Desc읽기
	if( !ReadDescXml(GetName().c_str(), IsBackgroundCreated()) )
		return false;

	CreateMetricInfoFromXML(GetName().c_str());

	string EluFileName = GetName();

	m_bExistAnimationListFile = GetAnimationMgr()->LoadAnimationInformationsToAnimationMgr(EluFileName,IsAutoAnimation(),IsBackgroundCreated());
	
	if( false == LoadEluFile(EluFileName) ) return false;

	ConnectMatrix();

	// material 연결, matBase 계산
	RMeshNode* pMeshNode;
	for(int i=0;i<(int)m_NodeTable.size();i++)
	{
		pMeshNode = m_NodeTable[i];
		assert(pMeshNode);

		pMeshNode->ConnectMtrl();

		if(!pMeshNode->m_pParent)
			pMeshNode->m_matBase = pMeshNode->m_matLocal;
		else
		{
			_ASSERT(pMeshNode->m_pParent->m_nID < i);
			pMeshNode->m_matBase = pMeshNode->m_matLocal * pMeshNode->m_pParent->m_matBase;
		}

		pMeshNode->m_matBase.GetInverse(&pMeshNode->m_matInvBase);
	}

	FillMeshNodeUserPropertyInfo();

	CalcStaticBoundingBox();

	return true;
}

bool RMesh::LoadEluFile( const string& EluFileName )
{
#define RF_READ(x,y) { if(!rf.Read((x),(y))) return false; }

	RFile rf;
	if(!rf.Open(EluFileName.c_str()))
	{
		mlog3("%s elu file 열기 실패.\n", GetName().c_str());
		return false;
	}

	ex_hd_t t_hd;
	RF_READ(&t_hd,sizeof(ex_hd_t) );

	if(t_hd.sig != EXPORTER_SIG)
	{
		mlog("%s elu file 파일 식별 실패.\n", EluFileName.c_str());
		return false;
	}

	m_nFileVersion = t_hd.ver;
	if(m_nFileVersion!=EXPORTER_CURRENT_MESH_VER)
	{
		mlog_tool("%s is not latest file version\n", EluFileName.c_str());
	}

	int i;

	RMatrix smat;

	// TODO: 로더 정리 할 것
	static RMeshNodeLoaderImpl_v12 meshNodeLoaderImpl_v12;
	static RMeshNodeLoaderImpl_v13 meshNodeLoaderImpl_v13;
	static RMeshNodeLoaderImpl_v14 meshNodeLoaderImpl_v14;
	static RMeshNodeLoaderImpl_v15 meshNodeLoaderImpl_v15;
	static RMeshNodeLoaderImpl_v16 meshNodeLoaderImpl_v16;
	static RMeshNodeLoaderImpl_v17 meshNodeLoaderImpl_v17;
	static RMeshNodeLoaderImpl_v18 meshNodeLoaderImpl_v18;
	static RMeshNodeLoaderImpl_v20 meshNodeLoaderImpl_v20;

	RMeshNodeLoaderImpl *pImpl = NULL;
	if ( m_nFileVersion == EXPORTER_MESH_VER20 )
	{
		pImpl = &meshNodeLoaderImpl_v20;
	}
	else if ( m_nFileVersion == EXPORTER_MESH_VER18 )
	{
		pImpl = &meshNodeLoaderImpl_v18;
	}
	else if ( m_nFileVersion == EXPORTER_MESH_VER17 )
	{
		pImpl = &meshNodeLoaderImpl_v17;
	}
	else if ( m_nFileVersion == EXPORTER_MESH_VER16 )
	{
		pImpl = &meshNodeLoaderImpl_v16;
	}
	else if ( m_nFileVersion == EXPORTER_MESH_VER15 )
	{
		pImpl = &meshNodeLoaderImpl_v15;
	}
	else if ( m_nFileVersion == EXPORTER_MESH_VER14 )
	{
		pImpl = &meshNodeLoaderImpl_v14;
	}
	else if ( m_nFileVersion == EXPORTER_MESH_VER13 )
	{
		pImpl = &meshNodeLoaderImpl_v13;
	}
	else if ( m_nFileVersion <= EXPORTER_MESH_VER12 )
	{
		pImpl = &meshNodeLoaderImpl_v12;
	}
	else
	{
		rf.Close();
		return false;
	}

	for(i=0;i < t_hd.mesh_num; i++)
	{
		RMeshNode* pMeshNode = new RMeshNodeMLeaf();

		pMeshNode->m_nID = m_nDataNum;
		pMeshNode->m_pParentMesh = this;
		pMeshNode->m_pBaseMesh = this;

		bool bResult = pImpl->Load(pMeshNode,&rf);
		_ASSERT(bResult);

		PushNode(pMeshNode);

		m_nDataNum++;
	}

	rf.Close();

	return true;
}

void RMesh::FillMeshNodeUserPropertyInfo( void )
{
	// UserProperty 등록
	for( size_t i = 0; i < m_vecUserProperty.size(); ++i )
	{
		RMeshNode *pMeshNode = GetNode(m_vecUserProperty[i].first.c_str());
		if(pMeshNode)
		{
			pMeshNode->m_userPropertyString.push_back(m_vecUserProperty[i].second);

			string strToken;

			stringstream sstream(m_vecUserProperty[i].second); // string을 stream에 넣는다
			sstream >> strToken;

			if (strcmp(strToken.c_str(), "set_cloth_mesh")==0) 
			{
				pMeshNode->AddFlag(RM_FLAG_CLOTH_MESH);
				sstream >> strToken;
				pMeshNode->m_nClothWidth = atoi(strToken.c_str());
				sstream >> strToken;
				pMeshNode->m_nClothHeight= atoi(strToken.c_str());
			}
		}
	}
	m_vecUserProperty.clear();
}

// TODO: 정리해야 함.
RMeshNode*			RMesh::loadClothMeshNodeELUFile(std::string fileName)
{
	
	if( fileName.empty() )
	{
		mlog3("RMesh : description is not loaded.");
		_ASSERT( !"Mesh 생성할 때 BuildAll()대신 Create()를 사용하세요." );
		return false;
	}

	RFile rf;
	if(!rf.Open(fileName.c_str()))
	{
		mlog3("%s elu file 열기 실패.\n", fileName.c_str());
		return NULL;
	}

	ex_hd_t t_hd;
	RF_READ(&t_hd,sizeof(ex_hd_t) );

	if(t_hd.sig != EXPORTER_SIG)
	{
		//mlog("%s elu file 파일 식별 실패.\n", fileName.c_str());

		return NULL;
	}

	int nFileVersion = t_hd.ver;
	if(nFileVersion!=EXPORTER_CURRENT_MESH_VER)
	{
		//mlog_tool("%s is not latest file version\n", fileName.c_str());
	}

	static RMeshNodeLoaderImpl_v12 meshNodeLoaderImpl_v12;
	static RMeshNodeLoaderImpl_v13 meshNodeLoaderImpl_v13;
	static RMeshNodeLoaderImpl_v14 meshNodeLoaderImpl_v14;
	static RMeshNodeLoaderImpl_v15 meshNodeLoaderImpl_v15;
	static RMeshNodeLoaderImpl_v16 meshNodeLoaderImpl_v16;
	static RMeshNodeLoaderImpl_v17 meshNodeLoaderImpl_v17;
	static RMeshNodeLoaderImpl_v18 meshNodeLoaderImpl_v18;
	static RMeshNodeLoaderImpl_v20 meshNodeLoaderImpl_v20;

	RMeshNodeLoaderImpl *pImpl = NULL;

	if ( nFileVersion == EXPORTER_MESH_VER20 )
	{
		pImpl = &meshNodeLoaderImpl_v20;
	}
	else if ( nFileVersion == EXPORTER_MESH_VER18 )
	{
		pImpl = &meshNodeLoaderImpl_v18;
	}
	else if ( nFileVersion == EXPORTER_MESH_VER17 )
	{
		pImpl = &meshNodeLoaderImpl_v17;
	}
	else if ( nFileVersion == EXPORTER_MESH_VER16 )
	{
		pImpl = &meshNodeLoaderImpl_v16;
	}
	else if ( nFileVersion == EXPORTER_MESH_VER15 )
	{
		pImpl = &meshNodeLoaderImpl_v15;
	}
	else if ( nFileVersion == EXPORTER_MESH_VER14 )
	{
		pImpl = &meshNodeLoaderImpl_v14;
	}
	else if ( nFileVersion == EXPORTER_MESH_VER13 )
	{
		pImpl = &meshNodeLoaderImpl_v13;
	}
	else if ( nFileVersion <= EXPORTER_MESH_VER12 )
	{
		pImpl = &meshNodeLoaderImpl_v12;
	}
	else
	{
		rf.Close();
		return NULL;
	}

	RMeshNode *pMeshNode;
	RMesh	  dummyMesh("dummyMesh",0);
	dummyMesh.SetFileVersion(EXPORTER_CURRENT_MESH_VER);

	for(int i=0;i < t_hd.mesh_num; i++)
	{
		pMeshNode = new RMeshNodeMLeaf();
		pMeshNode->m_nID = 0;
		pMeshNode->m_pParentMesh = NULL;
		pMeshNode->m_pBaseMesh = NULL;
		pMeshNode->m_pParentMesh = &dummyMesh;

		bool bResult = pImpl->Load(pMeshNode,&rf);
		_ASSERT(bResult);

		if( true == pMeshNode->isClothMesh() )
		{
			break;
		}

		delete pMeshNode;
		pMeshNode = NULL;
	}
	
	rf.Close();
	return pMeshNode;
}

RResource::LOAD_RESULT RMesh::Load()
{
	//TIMEPROFILE_THIS_FUNCTION;

	MakeAllNodeVertexBuffer();
	return LOADED;
}

void RMesh::DelMeshList()
{
	if (m_NodeHashList.empty())
		return;

	m_NodeHashList.EraseAll();
	m_NodeHashList.Clear();

	m_NodeTable.clear();	// 노드 테이블 비움 : 메시 리스트 비우는 함수지만 밀접하게 연관 되어 있는 것이므로 여기서 비워주는것이 맞을 듯 - 090608, OZ

	m_nDataNum = 0;
}

bool e_sort_str(RMeshNode* _a,RMeshNode* _b)
{
	if( _a->m_Name < _a->m_Name )
		return true;
	return false;
}

// bool RMesh::ReadEffectElements(MXmlElement* pEffectElement)
// {
// 	MXmlElement* pPropertyElement = pEffectElement->FirstChildElement("PROPERTY");
// 	if(!pPropertyElement) return false;
// 
// 	MXmlElement* pFileName = pPropertyElement->FirstChildElement("FILENAME");
// 	MXmlElement* pParentElement = pPropertyElement->FirstChildElement("PARENT");
//     RMeshEffect meshEffect;
// //		meshEffect.effectName = pFileName
// 	_Contents(meshEffect.effectName , pFileName);//파일네임인지 네임인지 구분필요.
// 	_Contents(meshEffect.parentName , pParentElement);
// 	m_meshEffectList.push_back(meshEffect);
// 
// 	return true;
// }
// 
// bool RMesh::ReadSceneElements(MXmlElement* pSceneElement)
// {
// 	_ASSERT( m_pSceneManager );
// 
// 	MXmlElement* pElement = pSceneElement->FirstChildElement();
// 	for (; pElement != NULL; pElement = pElement->NextSiblingElement())
// 	{
// 
// 		if (strcmp(pElement->Value(), "EFFECT")==0)
// 		{
// 			ReadEffectElements(pElement);
// 		}
// 		else if(strcmp(pElement->Value(), "LIGHT")==0)
// 		{
// 			RSceneNode* pLightSceneNode = m_pSceneManager->CreateSceneNode(RSID_LIGHTSCENENODE);
// 			// 이게 제일 좋으나 이안에서 업데이트를 하면 라이트매니저에 추가된다거나 아스트랄한 일들이 많이 발생해서
// 			// 중복해서 라이트씬을 로딩하게 했습니다.
// 			// TODO : LoadXML을 고쳐보아요~ by pok.
// 			// m_pSceneManager->LoadXML(pLightSceneNode, *pElement);
// 
// 			// common
// 			MXmlElement* pCommon = pElement->FirstChildElement("COMMON");
// 			if (pCommon)
// 			{
// 				if( m_pSceneManager->LoadCommonProperty(pLightSceneNode,*pCommon) == false )
// 				{
// 					delete pLightSceneNode;
// 					return false;
// 				}
// 			}
// 
// 			// property
// 			MXmlElement* pProperty = pElement->FirstChildElement("PROPERTY");
// 			if (pProperty)
// 			{
// 				if(pLightSceneNode->CreateFromXML(*pProperty, NULL) == false)
// 				{
// 					delete pLightSceneNode;
// 					return false;
// 				}
// 			}
// 			//pLightSceneNode->m_bCreatedByManager = false;	// 자원관리는 메시가 해준다.// 08.2.22 SceneNodeGroup으로 변경
// 			m_meshLightList.push_back( pLightSceneNode );
// 		}
// 	}
// 	return true;
// }

bool RMesh::ReadUserPropertyElements(MXmlElement* pUserPropertyListElement)
{
	MXmlElement* pElement = pUserPropertyListElement->FirstChildElement();
	for (; pElement != NULL; pElement = pElement->NextSiblingElement()) {

		if (strcmp(pElement->Value(), "USERPROPERTY")==0) {
			string strNodeName;
			_Attribute(strNodeName,pElement,"node");

			string strProperty;
			_Attribute(strProperty,pElement,"property");

			m_vecUserProperty.push_back( pair<string, string>(strNodeName, strProperty) );
		}
	}
	return true;
}

bool RMesh::ReadClothElements(MXmlElement* pClothListElement)
{
	MXmlElement* pElement = pClothListElement->FirstChildElement();
	for (; pElement != NULL; pElement = pElement->NextSiblingElement()) {

		if (strcmp(pElement->Value(), "CLOTH")==0) {
			string strNodeName;
			_Attribute(strNodeName,pElement,"name");

			RMeshNode *pMeshNode = GetNode(strNodeName.c_str());
			if(pMeshNode)
			{
				pMeshNode->AddFlag(RM_FLAG_CLOTH_MESH);
				_Attribute(pMeshNode->m_nClothWidth,  pElement,"width");
				_Attribute(pMeshNode->m_nClothHeight, pElement,"height");
			}
		}
	}
	return true;
}

bool RMesh::ReadDescXml(const char* fname, bool bBackground)
{
	char szDescName[256];

	strcpy(szDescName,fname);
	strcat(szDescName,".xml");


	MXml aXml;
	if(!aXml.LoadFile(szDescName))
		return false;

	////////////////////////////////////////////////////

	MXmlElement*	pChild;
	MXmlHandle		docHandle = aXml.DocHandle();

	pChild = docHandle.FirstChildElement().Element();
	
	if( m_pSceneManager )
	{
		MXmlElement* pMaterialElement = pChild->FirstChildElement("MATERIALLIST");
		if(pMaterialElement)
		{
			assert(m_vecMtrlList.empty());

			MXmlElement* pMaterialNode;

			char szTagName[256];

			pMaterialNode = pMaterialElement->FirstChildElement();

			char szSubName[10];
			int  nSubIndex = 0;
			for(pMaterialNode; pMaterialNode != NULL; pMaterialNode = pMaterialNode->NextSiblingElement())
			{
				strcpy(szTagName, pMaterialNode->Value());

				if(stricmp(szTagName,"MATERIAL")==0)
				{
					sprintf(szSubName, ".%d", nSubIndex++);
					string szMeshName = GetName() + szSubName;
				
					RMaterialResource* pMaterialResource = m_pSceneManager->CreateResource< RMaterialResource >( szMeshName.c_str() );
					if( pMaterialResource->GetState() == RRS_EMPTY )
					{
						pMaterialResource->Open(pMaterialNode, bBackground);
						pMaterialResource->BuildAll(bBackground);
					}
					m_vecMtrlList.push_back(pMaterialResource);
				}
			}
		}
	}

	MXmlElement* pUserPropertyListElement = pChild->FirstChildElement("USERPROPERTYLIST");
	if(pUserPropertyListElement)
		ReadUserPropertyElements(pUserPropertyListElement);

	MXmlElement* pCollisionElement = pChild->FirstChildElement("COLLISION");
	if(pCollisionElement)
	{
		SAFE_DELETE(m_pMeshCollision);
		m_pMeshCollision = new RMeshCollision;
		m_pMeshCollision->Load(*pCollisionElement);
	}

	if(pChild->FirstChildElement("ALPHA2PASS"))
		m_isAlpha2PassRendering = true;
	else
		m_isAlpha2PassRendering = false;

	if(pChild->FirstChildElement("AUTOANIMATION"))
		m_bAutoAnimation = true;
	else
		m_bAutoAnimation = false;

	return true;
}


//////////////////////////////////////////////////////////////////////////////

void RMesh::MakeAllNodeVertexBuffer()
{
	RMeshNode* pMeshNode=NULL;
	m_nVertexUsingSystemMemory = 0;
	m_nVertexUsingVideoMemory = 0;

	for( int i = 0 ; i < m_nDataNum; ++i )
	{
		pMeshNode = m_NodeTable[i];

		if(pMeshNode->isDummyMesh())	continue;
		if(!pMeshNode->m_nPointCnt)		continue;

		// 쉐이더 작동코드와 cloth ani 지원되는 모델의 경우 
		// + 옵션에서 껐다 켰다 하는 경우..
		// 강제 soft vertexbuffer mode + shader 없이 그리는 mode 구분.
		// shader 용은 soft 와 hard vertexbuffer 가 다르다.. 우선은 가속받은 방향으로..
		pMeshNode->MakeNodeBuffer( USE_VERTEX_HW | USE_VERTEX_SW );

		// 메모리 사용량
		m_nVertexUsingSystemMemory += pMeshNode->GetUsingSystemMemory();
		m_nVertexUsingVideoMemory += pMeshNode->GetUsingVideoMemory();
	}
}

void RMesh::ConnectMatrix()
{
	RMeshNode* pMeshNode=NULL;

	for(int i=0;i<m_nDataNum;i++) {

		pMeshNode = m_NodeTable[i];

		if(pMeshNode == NULL) continue;

		if(pMeshNode->m_nParentNodeID != -1) {
			pMeshNode->m_pParent = m_NodeTable[pMeshNode->m_nParentNodeID];
		}
	}
}

void RMesh::CalcStaticBoundingBox()
{
	m_staticBBox.Initialize();
	for(int i=0;i<m_nDataNum;i++)
	{
		RMeshNode *pMeshNode = m_NodeTable[i];
		if(!pMeshNode) continue;
		if(!pMeshNode->isRenderAble()) continue;

		RMatrix tm = pMeshNode->m_matBase;

		RBox trBox;
		TransformBox(&trBox,pMeshNode->m_boundingBox,tm);
		for(int j=0;j<8;j++)
			m_staticBBox.Add(trBox.Point(j));
	}
}

bool RMesh::GetStaticSkeletonBoundingBox(RBox& box)
{
	bool bResult = false;

	box.Initialize();
	for(int i = 0; i < m_nDataNum; i++)
	{
		RMeshNode *pMeshNode = m_NodeTable[i];
		if(!pMeshNode) continue;
//		if(pMeshNode->GetBipId()==RBIPID_UNKNOWN) continue;
		if(!pMeshNode->isBone()) continue;

		RMatrix tm = pMeshNode->m_matBase;
		RVector pos = RVector(tm._41,tm._42,tm._43);
		box.Add(pos);

		bResult = true;
	}

	return bResult;
}

void RMesh::PushNode( RMeshNode* pMeshNode )
{
	m_NodeHashList.PushBack(pMeshNode);
	m_NodeTable.push_back( pMeshNode );
}

const RMaterialResource* RMesh::GetMaterialResource(int nIndex)
{
	RPFC_THISFUNC;
	if( nIndex<0 || nIndex>= (int)m_vecMtrlList.size() )
		return NULL;

	return m_vecMtrlList[nIndex];
}

RMeshAnimationMgr* RMesh::GetAnimationMgr( void )
{
	return &m_AniMgr;
}

}
