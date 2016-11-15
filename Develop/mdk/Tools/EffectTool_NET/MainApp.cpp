#include "stdafx.h"

#include <Windows.h>
#include <MMSystem.h>
#include <shlwapi.h>
#include <math.h>

#include "ToolConfig.h"
#include "TSceneManager.h"

#include "Global.h"

#include "MainApp.h"
#include "RConfiguration.h"
#include "RDeviceD3D.h"
#include "RMesh.h"
#include "RMeshMgr.h"
#include "RRenderHelper.h"
#include "RMeshUtil.h"
#include "RFont.h"
#include "RMaterial.h"
#include "MProfiler.h"
#include "RTrail.h"
#include "RDirectionalLightSceneNode.h"
#include "RTextSceneNode.h"
#include "RRenderingStrategy.h"

#include "MainForm.h"
#include "RAnimationResource.h"

#include <algorithm>

#pragma comment (lib, "shlwapi.lib")

#define WEAPON_TYPE_MAX WEAPON_MAX

using namespace rs3;
using namespace EffectTool_NET;

CMainApplication *g_pMainApp = NULL;
bool g_bModified = false;
bool g_bIgnoreValueChangedMessage = false;

#ifdef _OLD_FORMAT
const char*		CMainApplication::EFFECT_FILE_EXT = ".Effect.xml";
#else
const char*		CMainApplication::EFFECT_FILE_EXT = ".effect.scene.xml";
#endif
const size_t	CMainApplication::EFFECT_FILE_EXT_LENGTH = strlen(CMainApplication::EFFECT_FILE_EXT);

// get/set function
RSceneNode* CMainApplication::GetSelectedChild()
{
	return m_pSelectedChild;
}

REmitter* CMainApplication::GetSelectedEmitter()
{
	return MDynamicCast(REmitter, m_pSelectedChild);
}

RTrail* CMainApplication::GetSelectedTrail()
{
	return MDynamicCast(RTrail, m_pSelectedChild);
}

CMainApplication::CMainApplication()
: m_pDevice(NULL)
, m_pCamera(NULL)
, m_pCurrentEffectSceneNode(NULL)
, m_pSelectedChild(NULL)
, m_pSelectedEffectActor(NULL)
, m_pCopiedSceneNode(NULL)
, m_bShowAABB(false)
, m_FillMode(RFILL_SOLID)
, m_time(0)
, m_pActor(NULL)
, m_pSelectedActorNode(NULL)
, m_bShowPlatform(true)
, m_bAxis(true)
, m_bGrid(true)
, m_bShowEmitter(false)
, m_bAnimationLoop(false)
, m_bEmitterNameAutoUpdate(true)
, m_bShowLight(false)
, m_bUseLighting(true)
, m_bLightToCameraDir(true)
{
	MInitLog(MLOG_LEVEL_DEBUG, MLOG_FILE|MLOG_DEBUG|MLOG_PROG, McvLog, "EffectMlog.txt");

	//_CrtSetBreakAlloc(2527);
}

CMainApplication::~CMainApplication()
{
	Destroy();

	MFinalizeLog();
}

void CMainApplication::Destroy()
{
	m_pTextNode->RemoveFromParent();
	SAFE_DELETE(m_pTextNode);

	SAFE_DELETE(m_pFont);

	if(m_pCamera)
	{
		m_pCamera->RemoveFromParent();
		m_pCamera->Destroy();
	}
	SAFE_DELETE(m_pCamera);

	DeleteActor();
	DeleteEffectInstance();

	SAFE_DELETE(m_pCopiedSceneNode);

	RDirectionalLightSceneNode* pDirLight = m_pSceneManager->GetDefaultLight();
	if(pDirLight)
	{
		pDirLight->RemoveFromParent();
		SAFE_DELETE(pDirLight);
	}

	if(m_pSceneManager)
	{
		m_pSceneManager->Clear();
		REngine::DestroySharedResource();
		m_pSceneManager->Destroy();
	}
	SAFE_DELETE(m_pSceneManager);
	REngine::RegisterSceneManager(NULL);

	if(m_pDevice)
	{
		m_pDevice->Destroy();
		delete m_pDevice;
		m_pDevice = NULL;
		REngine::RegisterDevice(NULL);
	}
}

void CMainApplication::OnNew()
{
	m_strFileName = "";
	UpdateCaption();

	DeleteEffectInstance();

	_ASSERT(m_pCurrentEffectSceneNode == NULL);
	m_pCurrentEffectSceneNode = new REffectSceneNode;
	// REffectSceneNode가 기본적으로 usable 이 false 이므로 설정해주지 않으면 제대로 업데이트가 동작하지 않는다
	m_pCurrentEffectSceneNode->SetUsable( true );
	AttachEffect( m_pCurrentEffectSceneNode, m_pActor, m_pSelectedActorNode);

	m_pCurrentEffectSceneNode->SetNodeName("ef");

	RefreshSceneNodeTreeView();
}

bool CMainApplication::Init(HWND hWnd)
{
	// Current Directory를 맞춘다.
	char szModuleFileName[_MAX_DIR] = {0,};
	GetModuleFileName(NULL, szModuleFileName, _MAX_DIR);
	PathRemoveFileSpec(szModuleFileName);


	// Initialize file system
	MCreateFileSystem( szModuleFileName, MFILEACCESS_GENERIC, CToolConfig::GetInstance().m_strFileSystemPath.c_str() );
	
	//load config
	CToolConfig::GetInstance().LoadConfig();

	InitDevice(hWnd);

	ReloadEffectList();
	OnNew();

	return true;
}

void CMainApplication::InitDevice(HWND hWnd)
{
	RECT rect;
	GetClientRect(hWnd,&rect);

	int nWidth = (int)(rect.right - rect.left);
	int nHeight = (int)(rect.bottom - rect.top);

	REngine::GetConfig().m_nWidth = nWidth;
	REngine::GetConfig().m_nHeight = nHeight;

	m_pDevice = new RDeviceD3D();
	m_pDevice->Create(hWnd);
	REngine::RegisterDevice(m_pDevice);

	// Tool SceneManager
	m_pSceneManager = new TSceneManager;
	REngine::RegisterSceneManager(m_pSceneManager);
	m_pSceneManager->Create(m_pDevice);
	REngine::InitSharedResource();

	SAFE_DELETE(m_pCamera);
	m_pCamera = new TCamera();
	m_pCamera->ResetSize();
	m_pCamera->AddAttribute(RSNA_DO_NOT_SAVE);
	m_pCamera->SetNearFarZ(10.f, 50000.f);
	m_pCamera->SetPostEffect();

	m_pSceneManager->AddSceneNode(m_pCamera);
	m_pSceneManager->SetPrimaryCamera(m_pCamera);
	SetClearColor( D3DCOLOR_ARGB(255, 53, 53, 53));

	CameraReset();

	m_pDevice->SetAmbient(D3DCOLOR_COLORVALUE( 0.5f, 0.5f, 0.5f, 1.0f ));
	m_pDevice->SetTextureFilter(0,RTF_BILINEAR);
	m_pDevice->SetTextureMipmapLodBias(0,-0.5f);

	RDirectionalLightSceneNode* pDirLight = m_pSceneManager->GetDefaultLight();
	if(!pDirLight)
	{
		pDirLight = new RDirectionalLightSceneNode();
		m_pSceneManager->AddSceneNode(pDirLight);

		pDirLight->SetLightAmbient(0,0,0,1);
	}

	m_pFont = new RFont;
	m_pTextNode = new RTextSceneNode;

	m_pFont->Create(GetDevice(),L"Arial", 16);
	m_pTextNode->SetFont(m_pFont);
	m_pTextNode->SetColor(0xffff0000);
	m_pTextNode->SetText("");
	m_pTextNode->SetPosition(1,1);
	m_pTextNode->AddAttribute(RSNA_DO_NOT_SAVE);
	m_pSceneManager->AddSceneNode(m_pTextNode);

	SetHDREnable( true );
}

void CMainApplication::OnResetDevice()
{
	m_pDevice->ResetDevice();
}

void CMainApplication::InitEffectList()
{
	SetCurrentDirectory(szModuleFileName);

	m_EffectList.clear();

	HANDLE hDir;
	WIN32_FIND_DATA DirData = {0};
	BOOL bRet = TRUE;
	tstring subDir;
	char pureFileName[MAX_PATH];
	for( int i = 0; i < 1; i++)
	{
		bRet = TRUE;
		subDir = tstring(CToolConfig::GetInstance().m_strEffectPath) + "*" + EFFECT_FILE_EXT;
		hDir = FindFirstFile(subDir.c_str(), &DirData);
		while( (hDir != INVALID_HANDLE_VALUE) && bRet )
		{
			if (DirData.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE) 
			{
				//xml 이름에서 제거
				strncpy_s( pureFileName, MAX_PATH, DirData.cFileName, strlen(DirData.cFileName) - EFFECT_FILE_EXT_LENGTH );
				m_EffectList.push_back( string(pureFileName) );
			}
			bRet = FindNextFile(hDir, &DirData);
		}
		FindClose(hDir);
	}
}

bool CMainApplication::SetEffectListTreeView()
{
	System::Windows::Forms::TreeView ^treeview = GlobalObjects::g_pMainForm->childTabForm->EffectListtreeView;
	treeview->Nodes->Clear();

	String^ filter = gcnew String(m_EffectListTreeViewFilter.c_str());
	filter = filter->ToLowerInvariant();
	TreeNode^ node = nullptr;
	for( size_t i = 0; i < m_EffectList.size(); ++i )
	{
		const string& str = m_EffectList[i];
		String^ nodename = gcnew String(str.c_str());
		String^ keyname = nodename->ToLowerInvariant();
		if( keyname->Contains(filter) )
			node = treeview->Nodes->Add(nodename, nodename);
	}

	return true;
}

void CMainApplication::SetEffectListTreeViewFilter(System::String^ strFilter)
{
	m_EffectListTreeViewFilter = MStringToCharPointer(strFilter);
}

bool CMainApplication::LoadEffectFile(System::String^ FileName)
{
	DeleteEffectInstance();

	const char* filename= MStringToCharPointer(FileName);

	bool bLoad = LoadEffectFile(filename);

	if( !bLoad )
	{
		mlog("이펙트 파일 로드 실패\n");
		MFreeCharPointer(filename);
		ReSelectNode();
		return false;
	}

	mlog("이펙트 파일 로드 성공\n");

	RefreshSceneNodeTreeView();

	MFreeCharPointer(filename);

	ReSelectNode();

	return true;
}

void CMainApplication::DeleteEffectInstance()
{
	if(m_pCurrentEffectSceneNode)
		m_pCurrentEffectSceneNode->Clear();
	
	SAFE_DELETE( m_pCurrentEffectSceneNode );
	
	m_pSelectedEffectActor = NULL;
	m_pSelectedChild = NULL;
}

bool CMainApplication::LoadEffectFile(const char* szFileName)
{
	DeleteEffectInstance();

	string strFileNameWithExt = GetRemovePathName( string(szFileName) );
	size_t nPos = strFileNameWithExt.find( EFFECT_FILE_EXT );
	if( nPos == string::npos )
		strFileNameWithExt += EFFECT_FILE_EXT;

	string strFileName = CToolConfig::GetInstance().m_strEffectPath + strFileNameWithExt;

	RSceneManager *pSceneManager = REngine::GetSceneManagerPtr();
	pSceneManager->LoadWorldSceneNodeFromRootXMLFile( pSceneManager->GetCurrentRootSceneNode(true), strFileName.c_str() );
	const RSceneNodeList& topLevelNodes = pSceneManager->GetCurrentRootSceneNode(false)->GetChildren();
	for( RSceneNodeList::const_iterator itr = topLevelNodes.begin(); itr!=topLevelNodes.end(); ++itr)
	{
		RSceneNode* pSceneNode = *itr;
		if( MIsDerivedFromClass(REffectSceneNode, pSceneNode) )
		{
			// 두개 이상있으면 에러
			_ASSERT( NULL == m_pCurrentEffectSceneNode );
			m_pCurrentEffectSceneNode = MDynamicCast(REffectSceneNode, pSceneNode);
		}
	}
	if( NULL == m_pCurrentEffectSceneNode )
		return false;

	m_strFileName = strFileNameWithExt;
	UpdateCaption();

	m_pCurrentEffectSceneNode->UpdateRenderableNodesList();

	RefreshSceneNodeTreeView();

	m_pCurrentEffectSceneNode->Start();

	return true;
}

void CMainApplication::RefreshSceneNodeTreeView( RSceneNode* pSceneNode )
{
	TreeNode^ node = MakeTreeNode(pSceneNode);

	TreeNode^ parentNode;

	// actornode constraint 가 붙은 경우에는 actornode 의 자식처럼 보여준다
	RActorNodeConstraintController* pConstraint = rs3::GetFirstActorNodeConstraintController( pSceneNode );
	if(pConstraint)
	{
		const RActorNode* pActorNode = pConstraint->GetTargetActorNode();
		parentNode = FindTreeNode( GlobalObjects::g_pMainForm->SceneNodetreeView->Nodes, (RSceneNode*)pActorNode );
	}
	else
		parentNode = FindTreeNode( GlobalObjects::g_pMainForm->SceneNodetreeView->Nodes, pSceneNode->GetParent() );

	if( parentNode!=nullptr )
	{
		parentNode->Nodes->Add( node );
	}else
	{
		// 최상단 노드는 이펙트 하나라고 가정되어있다.
		_ASSERT( m_pCurrentEffectSceneNode == pSceneNode );
		GlobalObjects::g_pMainForm->SceneNodetreeView->Nodes->Add ( node );
	}

	// 자식노드들 등록
	for ( RSceneNodeList::const_iterator itor = pSceneNode->GetChildren().begin(); itor != pSceneNode->GetChildren().end(); ++itor)
	{
		RefreshSceneNodeTreeView( *itor );
	}
}

void CMainApplication::RefreshSceneNodeTreeView()
{
	TreeNodeCollection^ tnRootCollection = GlobalObjects::g_pMainForm->SceneNodetreeView->Nodes;
	tnRootCollection->Clear();

	RefreshSceneNodeTreeView( m_pCurrentEffectSceneNode );

	GlobalObjects::g_pMainForm->SceneNodetreeView->Nodes[0]->Expand();

	SetChild( m_pSelectedChild );
}

void CMainApplication::ReorderSceneRendering()
{
	if( m_pCurrentEffectSceneNode )
		m_pCurrentEffectSceneNode->UpdateRenderableNodesList();
}

TreeNode^ CMainApplication::MakeTreeNode(RSceneNode* pSceneNode)
{
	String^ nodename = gcnew String( pSceneNode->GetNodeName().c_str() );
	TreeNode^ node = gcnew TreeNode( nodename );

	REffectActor* pEffectActor = MDynamicCast(REffectActor, pSceneNode);
	if( pEffectActor )
	{
		if( pEffectActor->GetVisibleSwitch() )
			node->Checked = true;
		else
			node->Checked = false;
	}
	else
	{
		if( pSceneNode->GetVisible() )
			node->Checked = true;
		else
			node->Checked = false;
	}

	// 아이콘 설정
	SetTreeNodeIcon(pSceneNode, node);

	SetSceneNodeToTreeNode( node, pSceneNode);

	// 딸린 노드들은 수정불가
	if(pSceneNode->QueryAttribute(RSNA_DO_NOT_SAVE))
		node->ForeColor = Color::Gray;

	return node;
}

void CMainApplication::SetTreeNodeIcon(RSceneNode* pSceneNode, TreeNode^ treeNode)
{
	int imageIndex = GetTreeNodeIcon(pSceneNode);
	treeNode->ImageIndex = imageIndex;
	treeNode->SelectedImageIndex = imageIndex;
}

int CMainApplication::GetTreeNodeIcon( RSceneNode* pSceneNode )
{
	if( MDynamicCast(REffectSceneNode, pSceneNode) )
		return 0;

	if( MDynamicCast(REffectActor, pSceneNode) )
		return 1;

	if( MDynamicCast(RActorNode, pSceneNode) )
		return 2;

	if( MDynamicCast(RTrail, pSceneNode) )
		return 3;

	if( MDynamicCast(RPointLightSceneNode, pSceneNode) )
		return 7;

	if( MDynamicCast(RSpotLightSceneNode, pSceneNode) )
		return 8;

	REmitter* pEmitter;
	if( pEmitter = MDynamicCast(REmitter, pSceneNode) )
	{
		const REmitterDescription* pDesc = pEmitter->GetDesc();

		switch( pDesc->eEmitterType )
		{
		case EMITTER_SPHERE: return 4;
		case EMITTER_BOX: return 5;
		case EMITTER_RING: return 6;
		}
	}

	return 0;
}

void CMainApplication::CameraPosCal()
{
}

void CMainApplication::CameraReset()
{
	m_pCamera->Reset(RVector(0.f,0.f,0.f));
	CameraPosCal();
}

void CMainApplication::InitState(int nWidth, int nHeight, bool bReset)
{
	if(nWidth == 0 || nHeight == 0)
		return;

	g_pMainApp->m_pCamera->SetFov((float)CToolConfig::GetInstance().CAMERA_FOV, (float)nWidth/(float)nHeight);

	if(bReset)
		m_pCamera->Reset(RVector(0.f,0.f,0.f));
}

void CMainApplication::OnLostDevice()
{
}

TreeNode^ CMainApplication::SearchNode(TreeNodeCollection^ nodes, String^ str)
{
	TreeNode^ tn = nullptr;
	System::Collections::IEnumerator^ myNodes = (safe_cast<System::Collections::IEnumerable^>(nodes))->GetEnumerator();
	while ( myNodes->MoveNext() )
	{
		tn = safe_cast<TreeNode^>(myNodes->Current);
		String^ tnstr = (safe_cast<TreeNode^>(myNodes->Current))->Text;
		if( String::Compare(tnstr, str) == 0)
		{
			return tn;
		}
		else
		{
			tn = SearchNode( tn->Nodes, str);
			if(tn != nullptr)
				return tn;
		}
	}

	return tn;
}

void CMainApplication::DrawPlatform()
{
	if( m_bShowPlatform)
	{
		RRenderHelper::SetRenderState();
		if( m_bGrid)
		{
			RMatrix matCenter;

			matCenter.MakeIdentity();
			matCenter.SetTranslation(RVector( -500.f, -500.f, 0.f ));
			RRenderHelper::RenderGrid(matCenter, 10.f, 100, 100, 0xff898989);

			matCenter.MakeIdentity();
			matCenter.SetTranslation(RVector( -2000.f, -2000.f, 0.f ));
			RRenderHelper::RenderGrid(matCenter, 100.f, 40, 40, 0xff8f8f8f);

			RRenderHelper::RenderCenter(RMatrix::IDENTITY, 4000.f, 4000.f, 0xff88ff88);
		}

		if( m_bAxis)
			RRenderHelper::RenderAxis(RMatrix::IDENTITY, 20.f);

		RRenderHelper::EndRenderState();
	}

}

void CMainApplication::DrawInfoText()
{
	class RCollectParticleNum : public RSceneNodeVisitor
	{
	public:
		RCollectParticleNum() : m_TotalParticleNums(0) {}
		virtual ~RCollectParticleNum(){}

		virtual bool OnPreOrderVisit(RSceneNode* pSceneNode )
		{
			REmitter* pEmitter = MDynamicCast(REmitter, pSceneNode );

			// 이미터에 대해
			if (pEmitter)
				m_TotalParticleNums += pEmitter->GetRenderParticleNum();

			return true;
		}
		virtual void OnPostOrderVisit(RSceneNode* pSceneNode ){}
		DWORD m_TotalParticleNums;
	};

	string str;
	DWORD dwRenderParticleNums = 0;

	if( m_pCurrentEffectSceneNode )
	{
		RCollectParticleNum visitor;
		m_pCurrentEffectSceneNode->Traverse(&visitor);
		dwRenderParticleNums = visitor.m_TotalParticleNums;

		GetFormatString(str, " EffectName : %s\n",
			m_pCurrentEffectSceneNode->GetNodeName().c_str());
	}

	REmitter *pEmitter = GetSelectedEmitter();
	if( pEmitter )
	{
		string strAdd;
		// by pok. 뜬금없이 여기에 전체 파티클량이 들어가서 코드가 지저분하게 되었네요.
		// 나중에 보시고 원 제작자가 적절히 수정해주삼.
		GetFormatString(strAdd, " Time : %.3f\n Emitter Name : %s \n Loop : %s\n Render Num : %d \n Total Render Num : %d \n",
			pEmitter->GetTime(), pEmitter->GetNodeName().c_str(),
			pEmitter->IsLooping() ? "true" : "false",
			pEmitter->GetRenderParticleNum(),
			dwRenderParticleNums);
		str += strAdd;
	}
	
	RTrail *pTrail = GetSelectedTrail();
	if ( pTrail )
	{
		string strAdd;
		GetFormatString(strAdd, "Max Count : %d\n", pTrail->GetCount());

		str += strAdd;
	}

	str += "** profile\n";
	class CProfileCallback
	{
	public:
		static void Func(const char *szString, void *pUserPointer)
		{
			std::string *pStr = (std::string*)pUserPointer;
			*pStr += szString;
		}
	};

	CProfileInfoVisitor ProfileInfoVisitor(&GetProfileManager(),(void*)&str,&CProfileCallback::Func);
	GetProfileManager().Traverse(&ProfileInfoVisitor,GetProfileManager().GetCallStackProfileRootNode());
	m_pTextNode->SetText(str.c_str());
}

// 이미터의 렌더를 불러서 그리는 걸로 바꾸기.
void CMainApplication::DrawEmitter(bool bShowEmitter)
{
	if( !bShowEmitter ) return;
	
	REmitter *pEmitter = GetSelectedEmitter();
	if( (pEmitter == NULL) ) return;

	const REmitterDescription *pDesc = pEmitter->GetDesc();

	EMITTER_TYPE eType		= pEmitter->GetEmitterType();
	const RVector *vRange	= pEmitter->GetRange();

	RMatrix mat;
	mat.MakeIdentity();

	if( pDesc->bFollowParentPosition && pDesc->bFollowParentRotation )
	{
		mat.SetScale(RVector(vRange->x*0.5f, vRange->y*0.5f, vRange->z*0.5f));
		mat *= pEmitter->GetWorldTransform();
	}
	else
	{
		mat = pEmitter->GetWorldTransform();
		mat.SetScale(RVector(vRange->x*0.5f, vRange->y*0.5f, vRange->z*0.5f));
	}

	m_pDevice->SetTransform( RST_WORLD,(RMatrix)mat);

	RRenderHelper::SetRenderState();

	switch ( eType )
	{
	case EMITTER_SPHERE:
		RRenderHelper::RenderSphere(mat, 1.0f, 0xffffffff);
		break;
	case EMITTER_BOX:
		RRenderHelper::RenderBox(mat, RBoundingBox(RVector(-1,-1,-1),RVector(1,1,1)));//, 0xffff0000);
		break;
	case EMITTER_RING:
		RRenderHelper::RenderRing(mat, 1.f, 1.f, 0x40ff0000, false);
		break;
	}

	RRenderHelper::EndRenderState();
}

void CMainApplication::DrawActor(bool bShowActor)
{
	if( !bShowActor ) return;
	
	RSceneNode* pSceneNode = GetSelectedChild();
	if( !pSceneNode ) return;

	RRenderHelper::SetRenderState();

	RActor* pActor = MDynamicCast( RActor, GetSelectedChild() );
	if( pActor )
	{
		for( int i = 0; i < pActor->GetActorNodeCount(); ++i )
		{
			RActorNode* pActorNode = pActor->GetActorNodes()[i];

			RBoundingBox box = pActorNode->m_pMeshNode->m_boundingBox;
			if( !pActorNode->m_pMeshNode->isRenderAble() )
			{
				box = RBoundingBox(RVector(-5,-5,-5),RVector(5,5,5));
			}
			RRenderHelper::RenderBox(pActorNode->GetWorldTransform(),box,0xffffffff);
		}
	}

	RActorNode* pActorNode = MDynamicCast( RActorNode, GetSelectedChild() );
	if( pActorNode )
	{
		RBoundingBox box = pActorNode->m_pMeshNode->m_boundingBox;
		if( !pActorNode->m_pMeshNode->isRenderAble() )
		{
			box = RBoundingBox(RVector(-5,-5,-5),RVector(5,5,5));
		}
		RRenderHelper::RenderBox(pActorNode->GetWorldTransform(),box,0xffffffff);
	}

	RRenderHelper::EndRenderState();
}

void CMainApplication::DrawEmitterAABB(bool bShowAABB)
{
	if(!bShowAABB) return;
	if( m_pSelectedChild == NULL ) return;

	RRenderHelper::SetRenderState();

	m_pSelectedChild->UpdateBoundingVolume();
	const RBoundingBox &aabb = m_pSelectedChild->GetAABB();

	RRenderHelper::RenderBox(RMatrix::IDENTITY, aabb);
	RRenderHelper::EndRenderState();
}

void CMainApplication::DrawLightAABB(bool bShowAABB)
{
	if(!bShowAABB) return;
	if( m_pSelectedChild == NULL ) return;

	RRenderHelper::SetRenderState();

	if( RPointLightSceneNode* pPointLight = MDynamicCast(RPointLightSceneNode, m_pSelectedChild) )
	{
		RSphere sphere = pPointLight->GetSphere();
		RRenderHelper::RenderSphere(sphere);
	}
	else if( RSpotLightSceneNode* pSpotLight = MDynamicCast(RSpotLightSceneNode, m_pSelectedChild))
	{
		RViewFrustum LightFrumstum = pSpotLight->GetLightFrustum();
		rs3::RRenderHelper::RenderPyramid(pSpotLight->GetWorldPosition(), LightFrumstum, 0xFFFFFFFF, true);
	}

	RRenderHelper::EndRenderState();
}

void CMainApplication::PreOrderRender()
{
	DrawPlatform();

	m_pDevice->SetFillMode(m_FillMode);
}

void CMainApplication::PostOrderRender()
{
	DrawBoxSelectedNode();
	DrawEmitter(m_bShowEmitter);
	DrawEmitterAABB(m_bShowEmitter);
	DrawActor(m_bShowEmitter);
	DrawLightAABB(m_bShowLight);
}

bool CMainApplication::OnDraw()
{
	if( m_pDevice == NULL) return false;
//	m_pSceneManager->Cull(m_pCamera);

	m_pDevice->Clear(true, true, false, m_pCamera->GetClearColor());
	m_pDevice->BeginScene();

// 	m_pCamera->BeginRender();
// 	m_pCamera->OnClearBuffer();
// 	//DrawPlatform();
// 	//m_pDevice->SetFillMode(m_FillMode);
// 	// ... PreOrderRender
// 	m_pCamera->Render();
// 	m_pCamera->UpdateData();
// 	//DrawBoxSelectedNode();
// 	//DrawEmitter(m_bShowEmitter);
// 	//DrawEmitterAABB(m_bShowEmitter);
// 	// ... PostOrderRender
// 	m_pCamera->EndRender();
// 	m_pCamera->OnEndRender();

	m_pSceneManager->RenderPrimaryCamera( this);

	m_pDevice->EndScene();
	m_pDevice->Flip();

	return true;
}

void CMainApplication::OnUpdateKeys()
{
	static DWORD thistime , lasttime = timeGetTime() , elapsed;

	thistime = timeGetTime();
	elapsed  = (thistime - lasttime) * (IsKeyDown(VK_SHIFT) ? 1:3);
	lasttime = thistime;

	float fsp = elapsed * 0.03f;

	UpdateKey(fsp);
}

void CMainApplication::OnUpdate()
{
	m_pCamera->UpdateData();
	UpdateLightToCamera();

	PFC_B("UpdateAndCull");

	if( GetActor() )
	{
		RSceneNodeUpdateVisitor v;
		GetActor()->Traverse( &v );
	}

	m_pSceneManager->UpdateTimer();
	m_pSceneManager->Update();

	PFC_E;

	if( m_pSelectedChild == NULL && m_pSelectedEffectActor == NULL)
		GlobalObjects::g_pMainForm->childTabForm->DisableAllControls();

	if( m_bAnimationLoop && GetActor() && GetActor()->GetAnimationController().IsPlayDone())
	{
		GetActor()->GetAnimationController().SetFrame(0);
		GetActor()->Play();
	}
}

void CMainApplication::UpdateKey(float time)
{
	UpdateCameraKey(time);
}

//카메라
void CMainApplication::UpdateCameraKey(float time)
{
	// 모드에 따라서 작동이 달라져야함
	// 카메라를 중심캐릭터를 비추는 모드
	// 카메라가 마음대로 돌아다니는 모드

	//////////////////////////////////
	// 각도 조정
	if ((GlobalObjects::g_pMainForm->Focused == false) && (GlobalObjects::g_pMainForm->MainView->Focused == false)) return;

	// 카메라 이동

	if(IsKeyDown('Q'))		m_pCamera->Up(time);		// 높이조정
	if(IsKeyDown('E'))		m_pCamera->Down(time);
	if(IsKeyDown('W'))		m_pCamera->m_fDist -= (time*4);	// 줌인 아웃
	if(IsKeyDown('S'))		m_pCamera->m_fDist += (time*4);
	if(IsKeyDown('A'))		m_pCamera->CenterRotationLeft(time);
	if(IsKeyDown('D'))		m_pCamera->CenterRotationRight(time);	// 그냥 회전

	if( m_pCurrentEffectSceneNode && IsKeyDown(VK_SPACE))	m_pCurrentEffectSceneNode->Start();
}

void CMainApplication::OnMouseMove(int stat, RVector2 pos, int delta)
{
	m_pCamera->OnMouseMove(stat, pos, delta);
}

//배경색
void CMainApplication::SetClearColor(DWORD col)
{
	m_pCamera->SetClearColor(col);
}

void CMainApplication::SetFogSettingEnable(bool bEnable)
{
	m_pCamera->SetFogEnable(bEnable);
	if ( bEnable )
	{
		m_pCamera->SetFog(500.f, m_pCamera->GetFarZ(), m_pCamera->GetClearColor());
	}
}

void CMainApplication::SetHDREnable(bool bEnable)
{
	if( REngine::GetSceneManagerPtr())
	{
		REngine::GetSceneManagerPtr()->SetHDREnable(bEnable);
	}
}

void CMainApplication::SetFillMode()
{
	m_FillMode = (m_FillMode == RFILL_SOLID) ? RFILL_WIRE : RFILL_SOLID ;
}

void CMainApplication::SetLightToggle()
{
	m_bUseLighting = !m_bUseLighting;

	m_pSceneManager->UseLighting( m_bUseLighting );
	GlobalObjects::g_pMainForm->LightOnOffMenuItem->Checked = m_bUseLighting;
}

void CMainApplication::SetLightToCameraToggle()
{
	m_bLightToCameraDir = !m_bLightToCameraDir;

	GlobalObjects::g_pMainForm->LightToCameraPosMenuItem->Checked = m_bLightToCameraDir;
}

void CMainApplication::SetLightAmbient(const RVector4& ambient)
{
	RDirectionalLightSceneNode* pDirLight = m_pSceneManager->GetDefaultLight();
	if( pDirLight )
	{
		pDirLight->SetLightAmbient( ambient );
	}
}

// bool CMainApplication::OnChildFileOpen(System::String^ strFileName)
// {
// 	_ASSERT(m_pCurrentEffectSceneNode != NULL);
// 
// 	m_pSelectedChild = NULL;
// 	m_pSelectedEffectActor = NULL;
// 
// 	const char* filename= MStringToCharPointer(strFileName);
// 	string str = GetRemovePathName(filename);
// 	bool bLoad = OnChildFileOpen(str.c_str());
// 	MFreeCharPointer(filename);
// 
// 	if( !bLoad )
// 		return false;
// 
// 	return true;
// }

bool CMainApplication::ImportSceneXML(TreeNode^ treeNode, const char* FileName)
{
	string strFileName = CToolConfig::GetInstance().m_strEffectPath + string(FileName) ;

	RWorldSceneNode worldSceneNode;

	TSceneManager *pSceneManager = dynamic_cast<TSceneManager*>(REngine::GetSceneManagerPtr());
	bool bSucceed =false;
	if(pSceneManager)
		bSucceed = pSceneManager->LoadWorldSceneNodeFromRootXMLFile( &worldSceneNode, strFileName.c_str() );
	else
		bSucceed = REngine::GetSceneManagerPtr()->LoadWorldSceneNodeFromRootXMLFile( &worldSceneNode, strFileName.c_str() );

	if ( !bSucceed )
		return false;

	// 모든 toplevel 노드들을 옮겨오자
	RSceneNode* pSceneNode = NULL;
	while( !worldSceneNode.GetChildren().empty() )
	{
		pSceneNode = worldSceneNode.GetChildren().front();
		pSceneNode->RemoveFromParent();
		AttachChild( pSceneNode, treeNode );
	}
	
	if(pSceneNode)
		SelectTreeNode(treeNode, pSceneNode);

	if( m_pCurrentEffectSceneNode )
		m_pCurrentEffectSceneNode->Start();

	return true;
}

void CMainApplication::MoveUpDownSceneNode( TreeNode^ treeNode, bool bUp )
{
	RSceneNode* pNode = GetSceneNodeFromTreeNode(treeNode);
	RSceneNode* pParentNode = pNode->GetParent();
	RSceneNodeList& children = const_cast<RSceneNodeList&> ( pParentNode->GetChildren() );

	RSceneNodeList::iterator itr = find( children.begin(), children.end(), pNode );
	if( itr==children.end() ) {
		_ASSERT(FALSE); // 없는 노드 ?
		return;
	}

	TreeNode^ parentTreeNode = treeNode->Parent;
	int treeIndex = treeNode->Index;

	RSceneNodeList::iterator itrToInsert = itr;
	if(bUp)
	{
		if( itr==children.begin() ) return;	// 맨 앞이면 더 앞으로 갈수가 없다

		itrToInsert--;
		treeIndex--;
	}
	else
	{
		itrToInsert++;

		if( itrToInsert==children.end() ) return;	// 맨 뒤면 더 뒤로 갈수가 없다
		
		itrToInsert++;
		treeIndex++;
	}

	children.erase( itr );
	children.insert( itrToInsert, pNode );

	parentTreeNode->Nodes->Remove( treeNode );
	parentTreeNode->Nodes->Insert( treeIndex, treeNode );

	ReorderSceneRendering();
	SelectTreeNode(parentTreeNode, pNode);

	g_bModified = true;
}

bool CMainApplication::OnChildFileOpen(TreeNode^ treeNode, const char* FileName)
{
	assert(m_pCurrentEffectSceneNode);

	SetCurrentDirectory(szModuleFileName);
	int len = strlen(FileName);

	const char* SCENEXML = ".scene.xml";

 	if(strncmp(&FileName[len-strlen(SCENEXML)],SCENEXML,strlen(SCENEXML))==0)
 	{
		if( !ImportSceneXML( treeNode, FileName ) )
			mlog("%s Scene 임포트 실패", FileName);
	}
	else if(strncmp(&FileName[len-4],".elu",4)==0)
	{
		REffectActor *pMeshActor = new REffectActor();
		if(pMeshActor->Create(FileName))
		{
			pMeshActor->SetNodeName(FileName);
			AttachChild( pMeshActor, treeNode );
			SelectTreeNode(treeNode, pMeshActor);
		}
		else
		{
			mlog("%s EffectActor 생성실패", FileName);
			delete pMeshActor;

			return false;
		}
	}
	else
	{
		::MessageBox(NULL,"알수없는 확장자를 가진 파일입니다~","info",MB_OK);
		return false;
	}

	ReorderSceneRendering();

	return true;
}

// 숫자를 하나씩 늘여가며 중복되지 않은 이름을 찾아준다
String^ CMainApplication::GetProperNodeName( String^ strNodeName )
{
	const char* pstr= MStringToCharPointer(strNodeName);
	string strProperNamePre = pstr;

	int count = 0;
	size_t lastNotANumber = strProperNamePre.find_last_not_of( "0123456789" );
	if(lastNotANumber!=string::npos)
	{
		count = atoi( strProperNamePre.substr(lastNotANumber+1).c_str() );
		strProperNamePre = strProperNamePre.substr(0,lastNotANumber+1);
	}
	
	String^ str;
	TreeNode^ node;
	do
	{
		str = String::Format("{0}{1:D2}", gcnew String(strProperNamePre.c_str()), count);
		node = FindTreeNode( GlobalObjects::g_pMainForm->SceneNodetreeView->Nodes, str) ;
		count++;
	} while ( node != nullptr );

	MFreeCharPointer(pstr);
	return str;
}

bool CMainApplication::CreateTrail( TreeNode^ parentTreeNode )
{
	_ASSERT( m_pCurrentEffectSceneNode != NULL );

	String^ effectname = gcnew String(m_pCurrentEffectSceneNode->GetNodeName().c_str()) + "_trail";
	String^ str = GetProperNodeName( effectname );
	const char* pstr =  MStringToCharPointer(str);

	RTrail *pTrail = new RTrail();
	_ASSERT(pTrail);

	pTrail->SetNodeName(pstr);
	AttachChild( pTrail, parentTreeNode );

	MFreeCharPointer(pstr);

	pTrail->SetLife(0.5f);
	pTrail->SetWidth(10.f);
	pTrail->UpdateTransform();
	pTrail->UpdateBoundingVolume();

	ReorderSceneRendering();
	SelectTreeNode(parentTreeNode, pTrail);
	g_bModified = true;

	return true;	
}

bool CMainApplication::CreateEmitter( TreeNode^ parentTreeNode, EMITTER_TYPE type)
{
	_ASSERT( m_pCurrentEffectSceneNode != NULL );

	String^ effectname = gcnew String(m_pCurrentEffectSceneNode->GetNodeName().c_str()) + "_em";
	String^ str = GetProperNodeName( effectname );
	const char* pstr= MStringToCharPointer(str);

	REmitter* pEmitter = new REmitter;
	pEmitter->CreateNew();

	pEmitter->SetEmitterType(type);
	pEmitter->SetBillboardType(BILLBOARD_XYZ);	//일단 기본으로 생성.
	pEmitter->SetNodeName(pstr);

	AttachChild( pEmitter, parentTreeNode );

	mlog("%s 이미터가 추가 되었습니다.", pstr);

	MFreeCharPointer(pstr);

	//기본값 추가.
	pEmitter->GetParticleTrack()[0].fScaleX = 100.f;
	pEmitter->GetParticleTrack()[1].fScaleX = 100.f;

	pEmitter->GetParticleTrack()[0].fScaleY = 100.f;
	pEmitter->GetParticleTrack()[1].fScaleY = 100.f;

	ReorderSceneRendering();
	SelectTreeNode(parentTreeNode, pEmitter);
	g_bModified = true;

	return true;
}

bool CMainApplication::CreateLight( TreeNode^ parentTreeNode, bool bSpotLight )
{
	_ASSERT( m_pCurrentEffectSceneNode != NULL );

	RLightSceneNode* pLight = NULL;
	
	if(bSpotLight)
	{
		pLight = new RSpotLightSceneNode;
		pLight->SetNodeName("SLight");
	}
	else
	{
		pLight = new RPointLightSceneNode;
		pLight->SetNodeName("PLight");
	}

	AttachChild( pLight, parentTreeNode );

	mlog("라이트가 추가되었습니다.");

	ReorderSceneRendering();
	SelectTreeNode(parentTreeNode, pLight);
	g_bModified = true;

	return true;
}

bool CMainApplication::CreateEffectSceneNode( TreeNode^ parentTreeNode )
{
	_ASSERT( m_pCurrentEffectSceneNode != NULL );

	String^ nodeName = gcnew String(m_pCurrentEffectSceneNode->GetNodeName().c_str()) + "_grp";
	String^ str = GetProperNodeName( nodeName );
	const char* pstr= MStringToCharPointer(str);

	REffectSceneNode* pEffectSceneNode = new REffectSceneNode;
	pEffectSceneNode->SetNodeName(pstr);

	AttachChild( pEffectSceneNode, parentTreeNode );

	mlog("%s 이펙트 그룹이 추가 되었습니다.", pstr);

	MFreeCharPointer(pstr);

	ReorderSceneRendering();
	SelectTreeNode(parentTreeNode, pEffectSceneNode);
	g_bModified = true;

	return true;
}

void CMainApplication::AttachChild( RSceneNode* pNode, TreeNode^ parentTreeNode )
{
	RActorNodeConstraintController* pOldConstraint = GetFirstActorNodeConstraintController( pNode );
	if( pOldConstraint )
	{
		pNode->RemoveController(pOldConstraint);
		delete pOldConstraint;
	}

	RSceneNode* pParent = GetSceneNodeFromTreeNode(parentTreeNode);

	// RActorNode 밑으로 붙이면 RActor로 붙여주고 constraint controller 를 달아준다
	RActorNode* pActorNode = MDynamicCast( RActorNode, pParent );
	if( pActorNode )
	{
		pActorNode->m_pActor->AddChild( pNode );
		pActorNode->m_pActor->AddToSceneNodeGroup( pNode );

		RActorNodeConstraintController* pConstraintController = new RActorNodeConstraintController;
		pConstraintController->SetTargetActorNode(pActorNode->GetNodeName());
		pConstraintController->SetIgnoreActorNodeDirection( false );
		pNode->AddController(pConstraintController);
	}else
	{
		pParent->AddChild( pNode );
		pParent->AddToSceneNodeGroup( pNode );
	}

	if( parentTreeNode != nullptr )
	{
		RefreshSceneNodeTreeView(pNode);
		parentTreeNode->Expand();
	}
}

void CMainApplication::MoveSceneNode( TreeNode^ treeNode, TreeNode^ parentTreeNode )
{
	RSceneNode* pNode = GetSceneNodeFromTreeNode( treeNode );

	pNode->RemoveFromParent();
	treeNode->Remove();

	AttachChild( pNode, parentTreeNode );

	g_bModified = true;
	ReorderSceneRendering();
	SelectTreeNode( parentTreeNode, pNode );
}

void CMainApplication::CopySceneNode( TreeNode^ treeNode, TreeNode^ parentTreeNode )
{
	RSceneNode* pNode = GetSceneNodeFromTreeNode( treeNode );

	RSceneNode* pNewSceneNode = pNode->Clone();

	String^ nodeName = gcnew String(pNode->GetNodeName().c_str());
	String^ str = GetProperNodeName( nodeName );
	const char* pstr= MStringToCharPointer(str);
	pNewSceneNode->SetNodeName( pstr );
	MFreeCharPointer( pstr ); 

	AttachChild( pNewSceneNode, parentTreeNode );

	g_bModified = true;
	ReorderSceneRendering();
	SelectTreeNode( parentTreeNode, pNewSceneNode );
}

void CMainApplication::LinkSceneNode( TreeNode^ treeNode, TreeNode^ parentTreeNode )
{
	RSceneNode* pNode = GetSceneNodeFromTreeNode( treeNode );

	RSceneNode* pNewSceneNode = pNode->Clone();

	AttachChild( pNewSceneNode, parentTreeNode );

	g_bModified = true;
	ReorderSceneRendering();
	SelectTreeNode( parentTreeNode, pNewSceneNode );
}

void CMainApplication::DeleteChild(TreeNode^ treeNode)
{
	RSceneNode* pSceneNode = GetSceneNodeFromTreeNode(treeNode);

	// 최상단 노드는 삭제 불능
	if( pSceneNode == m_pCurrentEffectSceneNode ) return;

	pSceneNode->RemoveFromParent();
	delete pSceneNode;

	treeNode->Remove();

 	m_pSelectedEffectActor = NULL;
 	m_pSelectedChild = NULL;
 
	g_bModified = true;
	ReorderSceneRendering();
}

// 관련 탭이 액티브 되게 할 수 없나... -  0906008, OZ
void CMainApplication::SetChild(RSceneNode* pSceneNode)
{
	m_pSelectedChild = pSceneNode;
	m_pSelectedEffectActor = MDynamicCast(REffectActor, pSceneNode);

	EffectTabForm ^tabForm = GlobalObjects::g_pMainForm->childTabForm;

	g_bIgnoreValueChangedMessage = true;

	// effect 를 선택한경우
	REffectSceneNode* pEffectSceneNode = MDynamicCast(REffectSceneNode, pSceneNode);
	if(pEffectSceneNode)
	{
		tabForm->ActiveEffectControls();
		tabForm->SetEffectValue(pEffectSceneNode);
	}
	// REffectActor의 처리
	else if ( m_pSelectedEffectActor )
	{
		tabForm->ActiveActorControls();

		bool bAniLoopType = false;
		bAniLoopType = m_pSelectedEffectActor->IsLooping();

		tabForm->MeshLoopcheckBox->Checked = bAniLoopType;

		tabForm->MeshStartTimeNumericUpDown->Value = Decimal(m_pSelectedEffectActor->GetStartTime()*30);

		float fRotateMin, fRotateMax;
		m_pSelectedEffectActor->GetRotateMinMax( fRotateMin, fRotateMax );
		float fRotateMinRad = RadianToDegree( fRotateMin );
		float fRotateMaxRad = RadianToDegree( fRotateMax );

		if( m_pSelectedEffectActor->IsAlign() )
			tabForm->MeshAlignRadioButton->Checked = true;
		else
		if( m_pSelectedEffectActor->IsAlignZFixed() )
			tabForm->MeshAlignXYRadioButton->Checked = true;
		else
		if( m_pSelectedEffectActor->IsRotateX() )
			tabForm->MeshRotateXRadioButton->Checked = true;
		else
		if( m_pSelectedEffectActor->IsRotateY() )
			tabForm->MeshRotateYRadioButton->Checked = true;
		else
		if( m_pSelectedEffectActor->IsRotateZ() )
			tabForm->MeshRotateZRadioButton->Checked = true;
		else
			tabForm->MeshNoOptionRadioButton->Checked = true;

		tabForm->MeshRotateMinNumericUpDown->Value = Decimal( fRotateMinRad );
		tabForm->MeshRotateMaxNumericUpDown->Value = Decimal( fRotateMaxRad );

		tabForm->MeshScalenumericUpDown->Value = Decimal(m_pSelectedEffectActor->GetScale().x);

		const RVector4& color = m_pSelectedEffectActor->GetMultiplyColor();
		tabForm->MeshColorSelectButton->BackColor = Color::FromArgb((int)(color.r*255), (int)(color.g*255), (int)(color.b*255));

		tabForm->MeshDistortioncheckBox->Checked = m_pSelectedEffectActor->IsDistortion();
	}
	else if ( GetSelectedEmitter() )
	{
		REmitter *pEmitter = GetSelectedEmitter();
		tabForm->ActiveEmitterControls();

		// set emitter value
		tabForm->SetEmitterValue(pEmitter);
		tabForm->SetParticleValue(pEmitter);
	}
	else if( GetSelectedTrail() ) // RTrail 의 처리 ( 현재는 REmitter와 RTrail 밖에 없음 )
	{
		RTrail *pTrail = GetSelectedTrail();
		_ASSERT(pTrail);

		tabForm->ActiveTrailControls();
		tabForm->SetTrailValue(pTrail);
	}
	else
	{
		// actor node 등등 처리 없는 노드들
		tabForm->DisableAllControls();
		g_bIgnoreValueChangedMessage = false;
		return;
	}

	g_bIgnoreValueChangedMessage = false;

	MarkSameNamedNodes();
}

void CMainApplication::SelectTreeNode(TreeNode^ parentTreeNode, RSceneNode* pSceneNode)
{
	TreeNode^ treeNode = FindTreeNode(parentTreeNode->Nodes, pSceneNode);

	if(treeNode != nullptr)
		GlobalObjects::g_pMainForm->SceneNodetreeView->SelectedNode = treeNode;
}

void CMainApplication::MarkSameNamedNodes()
{
	m_vecSameNamed.clear();
	MarkSameNamedNodes( GlobalObjects::g_pMainForm->SceneNodetreeView->Nodes );
}

void CMainApplication::MarkSameNamedNodes( TreeNodeCollection^ nodes )
{
	TreeNode^ tn = nullptr;
	System::Collections::IEnumerator^ myNodes = (safe_cast<System::Collections::IEnumerable^>(nodes))->GetEnumerator();
	while ( myNodes->MoveNext() )
	{
		tn = safe_cast<TreeNode^>(myNodes->Current);
		RSceneNode* pNode = GetSceneNodeFromTreeNode( tn );
		
		// 같은이름을가진 노드는 아쿠아, 나머진 흰색
		if( pNode != m_pSelectedChild &&
			pNode->GetNodeName() == m_pSelectedChild->GetNodeName()  &&
			pNode->GetNodeIDFromType() == m_pSelectedChild->GetNodeIDFromType() )
		{
			m_vecSameNamed.push_back( pNode );
			tn->BackColor = Color::Aqua ;
		}else
			tn->BackColor = Color::White ;

		MarkSameNamedNodes( tn->Nodes );
	}
}

void CMainApplication::ApplyToAllSameNamed( RSceneNode* pNode )
{
	for( size_t i=0; i<m_vecSameNamed.size(); i++)
	{
		RSceneNode* pSameNamedNode = m_vecSameNamed[i];

		// 원래 노드를 clone 한후 달려있는 자식노드들을 옮겨준다
		RSceneNode* pNewNode = pNode->Clone();

		while( !pSameNamedNode->GetChildren().empty() )
		{
			RSceneNode* pChildNode = pSameNamedNode->GetChildren().front();
			pChildNode->RemoveFromParent();
			pNewNode->AddChild( pChildNode );
		}

		// 부모로 부터 떼어내고 교체
		RSceneNode* pParent = pSameNamedNode->GetParent();
		TreeNode^ parentTreeNode = FindTreeNode( GlobalObjects::g_pMainForm->SceneNodetreeView->Nodes, pParent );

		if(pParent)
		{
			TreeNode^ treeNode = FindTreeNode( GlobalObjects::g_pMainForm->SceneNodetreeView->Nodes, pSameNamedNode );
			if(treeNode != nullptr)
				treeNode->Remove();

			pSameNamedNode->RemoveFromParent();

			AttachChild( pNewNode, parentTreeNode );

			// actornodeconstraint 가 이미 부모에 달린 다음에 addcontroller 되어야 정상적으로 동작해서 순서가 이상하다
			// controller는 원래 것을 clone 해준다
			for( int i = 0; i < RSNCTRL_MAX; ++i )
			{
				RSCENENODECONTROLLERLIST* pControllers = pSameNamedNode->GetControllers( RSNCTRL_TYPE(i));

				for( RSCENENODECONTROLLERLIST::iterator it = pControllers->begin(); it != pControllers->end(); ++it )
				{
					RSceneNodeController* pController = *it;
					if( pController->QueryAttribute( RCA_STICKY ) ) continue;

					RSceneNodeController* pNewController = pController->Clone();
					pNewNode->AddController( pNewController );
				}
			}

			delete pSameNamedNode;
		}
	}

	ReorderSceneRendering();
}

//m_pCurrentEffectSceneNode 가 이미터를 가진 인스턴스인지 메쉬를 가진 인스턴스 인지 체크
void CMainApplication::SetChild(System::String^ strEmitterName)
{
	const char* pcStr = MStringToCharPointer(strEmitterName);
	if( pcStr == NULL)
	{
		mlog("이미터 선택이 잘못되었습니다.");
		return;
	}

	_ASSERT( m_pCurrentEffectSceneNode != NULL );
	REffectSceneNode::EFFECT_NODETYPE effect_element = m_pCurrentEffectSceneNode->GetChild(pcStr);

	MFreeCharPointer(pcStr);
	pcStr = NULL;

	if( NULL!= effect_element.first )
	{
		SetChild( effect_element.first );
		effect_element.second = NULL;
	}
	else
	{
		SetChild( effect_element.second );
		effect_element.first = NULL;
	}
}

bool CMainApplication::SaveEffectFile()
{
	if(m_strFileName.empty()) return SaveAs();

	std::string strFileName(CToolConfig::GetInstance().m_strEffectPath + m_strFileName);
	bool bSave = SaveEffectFile( strFileName.c_str() );
	return bSave;
}

bool CMainApplication::SaveAs()
{
	SaveFileDialog^ saveFileDialog1 = gcnew SaveFileDialog;
	saveFileDialog1->DefaultExt = "Effect xml files";
	saveFileDialog1->InitialDirectory = String::Format("{0}{1}", gcnew String(g_pMainApp->szModuleFileName),"\\..\\..\\Data\\SFX");// ;

	saveFileDialog1->Filter = "Effect xml files (*.effect.scene.xml)|*.effect.scene.xml|All files (*.*)|*.*";
	saveFileDialog1->FilterIndex = 0;
	saveFileDialog1->RestoreDirectory = false;

	if(saveFileDialog1->ShowDialog() == System::Windows::Forms::DialogResult::OK)
	{
		// TODO: 다른 이름으로 저장이면 이미터들의 이름을 바뀐이펙트의 이름으로 해주삼.
		RenameEffectForSaveAs(saveFileDialog1->FileName);

		const char* pstr= MStringToCharPointer(saveFileDialog1->FileName);
		SaveEffectFile(pstr);
		MFreeCharPointer(pstr);

		g_bModified = false;

		ReloadEffectList();
		System::Windows::Forms::TreeView ^treeview = GlobalObjects::g_pMainForm->childTabForm->EffectListtreeView;
		string strFileName = GetRemovePathName( m_strFileName );

		size_t nPos = strFileName.find_first_of( EFFECT_FILE_EXT );
		if( nPos != string::npos )
			strFileName = strFileName.substr( 0, nPos );

		String^ nodename = gcnew String(strFileName.c_str());
		array<TreeNode^>^ findList = treeview->Nodes->Find( nodename, false );
		if( findList->Length )
			treeview->SelectedNode = findList[0];

		delete saveFileDialog1;
		return true;
	}

	delete saveFileDialog1;
	return false;
}

void CMainApplication::UpdateCaption()
{
	GlobalObjects::g_pMainForm->Text = 
		String::Format("EffectTool_NET - {0}", gcnew String( m_strFileName.c_str() ) );

	// TODO : g_bModifed 참조하여 수정된것이 있으면 * 표시 넣어주자
}

void CMainApplication::UpdateLightToCamera()
{
	if(m_bLightToCameraDir)
	{
		RDirectionalLightSceneNode* pDirLight = m_pSceneManager->GetDefaultLight();
		if( pDirLight )
		{
			pDirLight->SetLightPosition( m_pCamera->GetPosition() );
			pDirLight->SetLightDirection( m_pCamera->m_vTargetPos - m_pCamera->GetPosition() );
		}
	}
}

bool CMainApplication::SaveEffectFile(const char* szFileName)
{
	// 저장
	_ASSERT( m_pCurrentEffectSceneNode != NULL );
	if( NULL == m_pCurrentEffectSceneNode ) return false;

	MXml xml;
	MXmlDeclaration* pDec = xml.CreateDeclaration("1.0", "UTF-8", "");
	xml.Doc()->LinkEndChild(pDec);

	MXmlElement *pRootElement = new MXmlElement("ROOT");
	xml.Doc()->LinkEndChild(pRootElement);

	MXmlElement* pSceneElement = new MXmlElement("SCENE");
	pRootElement->LinkEndChild(pSceneElement);

	m_pSceneManager->SaveXML(m_pCurrentEffectSceneNode,pSceneElement);

	SetCurrentDirectory(szModuleFileName);

	bool bSave = xml.SaveFile( szFileName );

	if( !bSave)
	{
		mlog("%s 이펙트를 저장하지 못했습니다.", szFileName);
	}
	else
		mlog("%s 이펙트 파일을 저장하였습니다.", szFileName);

	return bSave;
}

void CMainApplication::SetEffectName(System::String^ strEffectName)
{
	_ASSERT( m_pCurrentEffectSceneNode != NULL );
	const char* pstr= MStringToCharPointer(strEffectName);
	m_pCurrentEffectSceneNode->SetNodeName(pstr);
	MFreeCharPointer(pstr);
}

void CMainApplication::SetEmitterTexture(System::String^ strName)
{
	REmitter *pEmitter = GetSelectedEmitter();
	_ASSERT( pEmitter != NULL );

	if (pEmitter == NULL)
	{
		mlog("선택된 이미터가 없습니다.");
		return;
	}

	const char* pstr= MStringToCharPointer(strName);
	string str = GetRemovePathName(pstr);

	pEmitter->ReplaceTexture(str.c_str());

	GlobalObjects::g_pMainForm->childTabForm->TexFileNametextBox->Text = gcnew String(str.c_str());
	MFreeCharPointer(pstr);
}

void CMainApplication::SetEmitterSITexture(System::String^ strName)
{
	REmitter *pEmitter = GetSelectedEmitter();
	_ASSERT( pEmitter != NULL );

	if (pEmitter == NULL)
	{
		mlog("선택된 이미터가 없습니다.");
		return;
	}

	const char* pstr= MStringToCharPointer(strName);
	string str = GetRemovePathName(pstr);

	pEmitter->ReplaceSITexture(str.c_str());

	GlobalObjects::g_pMainForm->childTabForm->TexSIFileNametextBox->Text = gcnew String(str.c_str());
	MFreeCharPointer(pstr);
}

void CMainApplication::SetTrailTexture(System::String^ strName)
{
	RTrail* pTrail = GetSelectedTrail();
	_ASSERT( pTrail );
	if( pTrail == NULL )
	{
		mlog("선택된 트레일이 없습니다.");
		return;
	}

	const char* pstr= MStringToCharPointer(strName);
	string str = GetRemovePathName(pstr);

	pTrail->ReplaceTexture(str.c_str());

	GlobalObjects::g_pMainForm->childTabForm->trailTexture->Text = gcnew String(str.c_str());
	MFreeCharPointer(pstr);
}

void CMainApplication::SetTrailSITexture(System::String^ strName)
{
	RTrail* pTrail = GetSelectedTrail();
	_ASSERT( pTrail );
	if( pTrail == NULL )
	{
		mlog("선택된 트레일이 없습니다.");
		return;
	}

	const char* pstr= MStringToCharPointer(strName);
	string str = GetRemovePathName(pstr);

	pTrail->ReplaceSITexture(str.c_str());

	GlobalObjects::g_pMainForm->childTabForm->TrailTexSIFileNameTextBox->Text = gcnew String(str.c_str());
	MFreeCharPointer(pstr);
}

bool CMainApplication::ChangeEmitterType(TreeNode^ treeNode, EMITTER_TYPE type)
{
	RSceneNode* pSceneNode = GetSceneNodeFromTreeNode(treeNode);

	REmitter* pEmitter = MDynamicCast(REmitter, pSceneNode);
	if( pEmitter && pEmitter->GetEmitterType() != type )
	{
		REmitterDescription* pDesc = const_cast<REmitterDescription*>(pEmitter->GetDesc());
		pDesc->eEmitterType = type;
		pEmitter->UpdateResource();

	//	EffectTabForm ^tabForm = GlobalObjects::g_pMainForm->childTabForm;
	//	tabForm->SetEmitterRangeZEnable( type );

		SetTreeNodeIcon(pSceneNode, treeNode);
		return true;
	}

	return false;
}

bool CMainApplication::MeshFileOpen(System::String^ strFileName)
{
	const char* filename= MStringToCharPointer(strFileName);
	bool bLoad = MeshFileOpen(filename, true);
	MFreeCharPointer(filename);

	return bLoad;
}

bool CMainApplication::MeshFileOpen(const char* FileName, bool bAddMRUList)
{
	int len = strlen(FileName);

	if(strncmp(&FileName[len-4],".elu",4)==0)
	{
		if( eluFileOpen(FileName) )
		{
			ReSelectNode();
			RePlayAnimation();

			if( bAddMRUList )
			{
				CToolConfig::GetInstance().AddEluFileToMRUList(FileName);
				GlobalObjects::g_pMainForm->RefreshMRUEluFileList();
			}
			return true;
		}
		else
		{
			return false;
		}
	}
	else if(strncmp(&FileName[len-4],".ani",4)==0)
	{
		return LoadAniFile(FileName);
	}

	mlog("지원하지 않는 파일 형식입니다. - %s", &FileName[len-4]);

	return false;
}

bool CMainApplication::MeshFileOpenFromMRUList(int index)
{
	string fileName = CToolConfig::GetInstance().m_eluFileMRUList.Get( index );

	if( fileName.length() )
		return MeshFileOpen( fileName.c_str(), true );

	return false;
}

bool CMainApplication::DefaultMeshFileOpen()
{
	std::string defaultEluFilename = string( CToolConfig::GetInstance().m_strEffectPath ) + "test dummy.elu";

	bool result = MeshFileOpen(defaultEluFilename.c_str(), false);
	if( result )
	{
		GetCurrentEffectSceneNode()->RemoveFromParent();
		AttachEffect( GetCurrentEffectSceneNode(), m_pActor, m_pActor->GetActorNode("Dummy02") );
	}

	return result;
}

void CMainApplication::DeleteActor()
{
	m_pSelectedActorNode = NULL;

	if(m_pActor)
	{
		m_pActor->RemoveFromParent();
		m_pActor->Destroy();
	}
	SAFE_DELETE(m_pActor);
}

bool CMainApplication::eluFileOpen(const char* FileName)
{
	DeleteActor();

	SetCurrentDirectory(szModuleFileName);

	m_pActor = new RActor;
	m_pActor->AddAttribute(RSNA_DO_NOT_SAVE);

	if(!m_pActor->RActor::Create(FileName))
	{
		SAFE_DELETE(m_pActor);
		mlog("Mesh파일 로드 실패 - %s", FileName);
		return false;
	}

	m_loadedActorName = FileName;

	m_pActor->SetPosition(RVector(0,0,0));
	m_pSceneManager->AddSceneNode(m_pActor);	// 액터를 씬에 붙임, COMMENT OZ

	//애니메이션이 있는 모델 파일이면
	SetAnitreeView();
	SetNodeListTreeView();

	mlog("%s 로드 완료 \n", FileName);

	return true;
}

bool CMainApplication::LoadAniFile(const char* FileName)
{
	if( !m_pActor ) return false;

	string pureName = GetPureFileName(FileName);
	string folderName1 = GetFolderName(FileName);
	int cs = folderName1.size() + 1;//'_'
	string strAnimationName = pureName.substr(0, pureName.size()-4);

	//////////////////////////////////////////////////////////////////////////
	//애니가 지정되어 있는데 넣는 경우 
	RAnimation* pAnimation = m_pActor->GetAnimation((char*)strAnimationName.c_str());
	if( pAnimation )
	{
		m_pActor->m_pMesh->m_AniMgr.Delete((char*)strAnimationName.c_str());
		pAnimation = NULL;
		mlog("이름이 같아 기존 애니메이션을 지우고 새로운 파일로 대체 합니다. - %s", strAnimationName.c_str());
	}

	pAnimation = m_pActor->m_pMesh->m_AniMgr.Add(strAnimationName.c_str(),(char*)FileName );
	pAnimation->GetResource()->BuildAll();
	pAnimation->SetAnimationLoopType(RAniLoopType_HoldLastFrame);
	m_pActor->SetAnimation(pAnimation);
	m_pActor->Play();

	GlobalObjects::g_pMainForm->childTabForm->AnitreeView->Nodes->Add("{0}", gcnew String(strAnimationName.c_str()) );

	return true;
}

bool CMainApplication::SetAnitreeView()
{
	System::Windows::Forms::TreeView ^treeview = GlobalObjects::g_pMainForm->childTabForm->AnitreeView;
	treeview->Nodes->Clear();
	treeview->BeginUpdate();

	if( NULL == m_pActor )
		return false;

	if( NULL == m_pActor->m_pMesh)
		return false;

	RMeshAnimationMgr* pAMgr = &m_pActor->m_pMesh->m_AniMgr;
	_ASSERT( pAMgr );

	String^ filter = gcnew String(m_AniTreeViewFilter.c_str());
	filter = filter->ToLowerInvariant();

	for(RAnimationHashList_Iter it = pAMgr->m_NodeHashList.begin(); it != pAMgr->m_NodeHashList.end(); it++)
	{
		RAnimation* pAnimation = *it;
		String^ nodeName = gcnew String(pAnimation->GetAliasName().c_str() );
		String^ keyName = nodeName->ToLowerInvariant();
		if( keyName->Contains(filter) )
			treeview->Nodes->Add("{0}", nodeName);
	}

	//sort
	treeview->Sort();
	treeview->EndUpdate();

	return true;
}

void CMainApplication::SetAnitreeViewFilter(System::String^ strFilter)
{
	m_AniTreeViewFilter = MStringToCharPointer(strFilter);
}

void CMainApplication::SetAnimation_Play(System::String^ Name)//const char* name)
{
	const char* name= MStringToCharPointer(Name);

	if(m_pActor->SetAnimation((char*)name))
	{
		m_loadedActorAnimationName = name;

		//m_pActor->GetAnimation()->SetSpeed(0.1);	//테스트
		m_pActor->Play();
	}

	MFreeCharPointer(name);
}

bool CMainApplication::SetNodeListTreeView()
{
	System::Windows::Forms::TreeView ^treeview = GlobalObjects::g_pMainForm->childTabForm->NodeListtreeView;
	treeview->Nodes->Clear();

	String^ filter = gcnew String(m_NodeListTreeViewFilter.c_str());
	filter = filter->ToLowerInvariant();

	for(int i = 0; i<m_pActor->GetActorNodeCount();i++)
	{
		RActorNode *pActorNode = m_pActor->GetActorNodes()[i];
		String^ nodename = gcnew String(pActorNode->m_pMeshNode->m_Name.c_str());

		if( filter->Length > 0 )
		{
			String^ keyName = nodename->ToLowerInvariant();
			if( keyName->Contains(filter) )
				treeview->Nodes->Add("{0}", nodename);
		}
		else
		if( pActorNode->GetParentActorNode() == NULL)
		{
			treeview->Nodes->Add("{0}", nodename);
		}
		else
		{
			RActorNode *parentActorNode = pActorNode->GetParentActorNode();
			String^ parentnodename = gcnew String(parentActorNode->m_pMeshNode->m_Name.c_str());

			TreeNode^ tn = SearchNode(treeview->Nodes, parentnodename);
			_ASSERT( tn != nullptr );
			if( tn != nullptr)
				tn->Nodes->Add("{0}", nodename);
		}
	}

	return true;
}

void CMainApplication::SetNodeListTreeViewFilter(System::String^ strFilter)
{
	m_NodeListTreeViewFilter = MStringToCharPointer(strFilter);
}

/************************************************************************/
/* 선택된 노드에 이펙트 붙이기.											*/
/************************************************************************/
void CMainApplication::SelectedNode(String^ strNodeName)
{
	const char* nodename = MStringToCharPointer(strNodeName);
	m_pSelectedActorNode = m_pActor->GetActorNode(nodename);

	if( m_pSelectedActorNode && GetCurrentEffectSceneNode() )
	{
		m_loadedActorLinkedDummyName = nodename;

		GetCurrentEffectSceneNode()->RemoveFromParent();
		AttachEffect( GetCurrentEffectSceneNode(), m_pActor, m_pSelectedActorNode );
	}

	MFreeCharPointer(nodename);
}

void CMainApplication::DrawBoxSelectedNode()
{
	if(m_pSelectedActorNode)
	{
		RRenderHelper::SetRenderState();

		//m_pDevice->SetDepthEnable(false,false);
		RBoundingBox box = m_pSelectedActorNode->m_pMeshNode->m_boundingBox;
		if(!m_pSelectedActorNode->m_pMeshNode->isRenderAble())
			box = RBoundingBox(RVector(-5,-5,-5),RVector(5,5,5));
		RMatrix matTM = m_pSelectedActorNode->GetWorldTransform();
		RRenderHelper::RenderBox(matTM,box,0xffffffff);

		if( m_bAxis)
			RRenderHelper::RenderAxis(matTM,30,0.5f);

		RRenderHelper::EndRenderState();
	}
}

/************************************************************************/
/* mlog에 콜백함수로 연결되어 있음.										*/
/* mlog로 사용할것.                                                     */
/************************************************************************/
void CMainApplication::McvLog(const char* szText, MLogLevel nLevel)
{
	GlobalObjects::g_pMainForm->LogtextBox->Text += gcnew String(szText);
	GlobalObjects::g_pMainForm->LogtextBox->Text += System::Environment::NewLine;

	//스크롤 내리기.
	GlobalObjects::g_pMainForm->LogtextBox->SelectionStart = GlobalObjects::g_pMainForm->LogtextBox->TextLength;
	GlobalObjects::g_pMainForm->LogtextBox->ScrollToCaret();
}

//이미터의 이름 변경.
std::string CMainApplication::SetChildName(String^ str)
{
	const char* name = MStringToCharPointer(str);

	bool bSameName(false);
	REffectSceneNode::EFFECT_NODETYPE effectPair = m_pCurrentEffectSceneNode->GetChild(name);
	if ( effectPair.first != NULL || effectPair.second != NULL )
	{
		bSameName = true;
	}

	if (m_pSelectedChild)
	{
		// 같은 이름이 있는데
		if (bSameName)
		{
			// 그 이름이 현재 선택된 노드의 이름이 아닌, 다른 노드와 같다면
			if (m_pSelectedChild->GetNodeName() != name )
			{
				// 실패, 예전이름을 리턴해서 이름변경을 취소한다.
				return m_pSelectedChild->GetNodeName();
			}
		}
		m_pSelectedChild->SetNodeName( name );
	}
	else if( m_pSelectedEffectActor)
	{
		// 같은 이름이 있는데
		if (bSameName)
		{
			// 그 이름이 현재 선택된 노드의 이름이 아닌, 다른 노드와 같다면
			if (m_pSelectedEffectActor->GetNodeName() != name )
			{
				// 실패, 예전이름을 리턴해서 이름변경을 취소한다.
				return m_pSelectedEffectActor->GetNodeName();
			}
		}
		m_pSelectedEffectActor->SetNodeName( name );
	}
	else
	{
		_ASSERT( !"둘다 선택된게 없으면 안되는디.." );
	}

	MFreeCharPointer(name);
	return std::string("");
}

/************************************************************************/
/* 다른이름으로 저장시 effectname으로 이미터들의 이름을 바꿈			*/
/************************************************************************/
// by pok. 현재 지원하지 않음.
void CMainApplication::RenameEffectForSaveAs(String^ strname)
{
	const char* name = MStringToCharPointer(strname);

	m_strFileName = name;
	UpdateCaption();

	string purename = GetPureFileName(name);
	purename = GetRemoveDotlast(purename);

	m_pCurrentEffectSceneNode->SetNodeName(purename.c_str());
	
	if( m_bEmitterNameAutoUpdate )
	{
		std::vector< std::string > vecNameTable = m_pCurrentEffectSceneNode->GetChildrenNameTable();
		int i = 0;
		for (std::vector< std::string >::iterator itr = vecNameTable.begin(); itr != vecNameTable.end(); ++itr)
		{
			REffectSceneNode::EFFECT_NODETYPE elementPair = m_pCurrentEffectSceneNode->GetChild(itr->c_str());
			if (elementPair.first)
			{
				string emittername;
				emittername = GetFormatString(emittername, "%s%.2d", purename.c_str(), i);
				elementPair.first->SetNodeName(emittername);
				++i;
			}
		}
	}
	
	MFreeCharPointer(name);
}

void CMainApplication::ReloadEffectList()
{
	InitEffectList();
	SetEffectListTreeView();
}

void CMainApplication::ReloadEffect()
{
	if( m_strFileName.length() )
	{
		LoadEffectFile( m_strFileName.c_str() );
	}

	ReloadActor();
}

void CMainApplication::ReloadActor()
{
	if( m_loadedActorName.length() )
	{
		eluFileOpen( m_loadedActorName.c_str() );

		ReSelectNode();
		RePlayAnimation();
	}
}

void CMainApplication::AttachEffect( REffectSceneNode* pEffectInst, RSceneNode* pSceneNode, RActorNode* pActorNode , const RMatrix& _rBaseFrame, bool bIgnoreDir )
{
	RSceneNodeController* pOldController = pEffectInst->GetFirstControllerByName(RSNCTRL_PREUPDATE, RCID_ACTORNODECONSTRAINT);
	if(pOldController)
	{
		pEffectInst->RemoveController( pOldController );
		delete pOldController;
	}

	m_pSceneManager->AttachEffect( pEffectInst, pSceneNode, pActorNode, _rBaseFrame, bIgnoreDir );
	RSceneNodeController* pNewController = pEffectInst->GetFirstControllerByName(RSNCTRL_PREUPDATE, RCID_ACTORNODECONSTRAINT);
	if(pNewController)
		pNewController->AddAttribute( RCA_STICKY );

}

void CMainApplication::ReSelectNode()
{
	if( m_pActor )
	{
		System::Windows::Forms::TreeView^ treeview = GlobalObjects::g_pMainForm->childTabForm->NodeListtreeView;
		String^ attachNodeName = gcnew String( m_loadedActorLinkedDummyName.c_str() );
		TreeNode^ treeNode = SearchNode( treeview->Nodes, attachNodeName );
		if( treeNode != nullptr )
		{
			treeview->SelectedNode = nullptr; // 이미 같은 노드가 선택되어 있으면 이벤트가 발생하지 않으므로 초기화 해준다.
			treeview->SelectedNode = treeNode;
		}
	}
}

void CMainApplication::RePlayAnimation()
{
	if( m_pActor )
	{
		if( m_pActor->SetAnimation( m_loadedActorAnimationName.c_str() ) )
			m_pActor->Play();
	}
}

void CMainApplication::OnLoopAnimation()
{
	if( GlobalObjects::g_pMainForm->childTabForm->AniLoopcheckBox->Checked)
		m_bAnimationLoop = true;
	else
		m_bAnimationLoop = false;
}

void CMainApplication::StartButton()
{
	if( GetCurrentEffectSceneNode() )
	{
		GetCurrentEffectSceneNode()->Start();
	}
	if( GetActor() )
	{
		//GetActor()->SetAnimation(GetActor()->GetAnimation());
		GetActor()->GetAnimationController().SetFrame(0);
		GetActor()->Play();
	}
}

void CMainApplication::StopButton()
{
	if( GetCurrentEffectSceneNode() )
	{
		GetCurrentEffectSceneNode()->Stop();
	}
	if( GetActor() )
	{
		GetActor()->Stop();
	}
}

void CMainApplication::PauseButton()
{
	if( GetCurrentEffectSceneNode() )
	{
		GetCurrentEffectSceneNode()->Pause();
	}
	if( GetActor() )
	{
		GetActor()->Pause();
	}
}

bool CMainApplication::ConfirmSave()
{
	if (g_bModified && g_pMainApp->GetCurrentEffectSceneNode())
	{
		System::Windows::Forms::DialogResult r =
			// YesNoCancel로 변경 - 090602, OZ(Engine Ticket #650)
			System::Windows::Forms::MessageBox::Show(L"편집된 이팩트가 있습니다. 저장하시겠습니까?", "Notice", System::Windows::Forms::MessageBoxButtons::YesNoCancel );

		if (r == System::Windows::Forms::DialogResult::Yes)
		{
			std::string strFileName( (string(CToolConfig::GetInstance().m_strEffectPath ) + (g_pMainApp->GetCurrentEffectSceneNode()->GetNodeName() + ".Effect.scene.xml").c_str() ) );
			g_pMainApp->SaveEffectFile( strFileName.c_str() );
		}
		// Cancel하면 F반 - 090602, OZ
		else if( r == System::Windows::Forms::DialogResult::Cancel)
		{
			return false;	
		}
	}
	g_bModified = false;

	return true;
}

void CMainApplication::CopySceneNode( TreeNode^ treeNode )
{
	RSceneNode* pNode = GetSceneNodeFromTreeNode( treeNode );
	if( pNode == NULL )
		return;

	SAFE_DELETE(m_pCopiedSceneNode);
	m_pCopiedSceneNode = pNode->Clone();

	if( m_pCopiedSceneNode )
		m_pCopiedSceneNode->SetNodeName( pNode->GetNodeName() );
}

void CMainApplication::PasteSceneNode( TreeNode^ parentTreeNode )
{
	if( m_pCopiedSceneNode == NULL )
		return;

	RSceneNode* pNewSceneNode = m_pCopiedSceneNode->Clone();
	if( pNewSceneNode == NULL )
		return;

	String^ nodeName = gcnew String(m_pCopiedSceneNode->GetNodeName().c_str());
	String^ str = GetProperNodeName( nodeName );
	const char* pstr= MStringToCharPointer(str);
	pNewSceneNode->SetNodeName( pstr );
	MFreeCharPointer( pstr ); 

	AttachChild( pNewSceneNode, parentTreeNode );

	g_bModified = true;
	ReorderSceneRendering();
	SelectTreeNode( parentTreeNode, pNewSceneNode );

	StartButton();
}