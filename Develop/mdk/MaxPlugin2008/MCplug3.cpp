/**********************************************************************
 *<
	FILE: MCplug2.cpp

	DESCRIPTION:	Appwizard generated plugin

	CREATED BY: 

	HISTORY: 

 *>	Copyright (c) 1997, All Rights Reserved.
 **********************************************************************/

#include "stdafx.h"

#include "MCplug3.h"
#include "stdmat.h"	// maxsdk
#include "RMaxMaterial.h"
#include "MDebug.h"
#include "FileInfo.h"
#include "MFileSystem.h"
#include "MConvexHullAlgorithm.h"

#include "RTTGroup/IRTTGroup.h"

#pragma comment( lib, "mesh.lib" )
#pragma comment( lib, "maxutil.lib" )
#pragma comment( lib, "mnmath.lib" )

#define MCPLUG3_CLASS_ID	Class_ID(0x501e02c0, 0x1a538efb)

///////////////////////////////////////////////////

HINSTANCE hInstance;
int controlsInit = FALSE;

list<string> g_errors;
void ShowErrorMessage()
{
	if(g_errors.size())
	{
		string strBuffer;
		for(list<string>::iterator itr = g_errors.begin(); itr!= g_errors.end(); itr++)
		{
			strBuffer+= *itr;
			strBuffer+="\n";
		}

		char szBuffer[256];
		sprintf_s(szBuffer,"\n %d Errors",g_errors.size());

		strBuffer+= szBuffer;
		MessageBox(NULL,strBuffer.c_str(),"Errors",MB_OK);
	}
}
void MCLogCallBack(const char* szText, MLogLevel nLevel)
{
	// 워닝 이상이면 담아둔다
	if(nLevel>=MLOG_LEVEL_WARNING)
		g_errors.push_back(szText);
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL,ULONG fdwReason,LPVOID lpvReserved)
{
	hInstance = hinstDLL;				// Hang on to this DLL's instance handle.

	if (!controlsInit) 
	{
		controlsInit = TRUE;
		InitCommonControls();			// Initialize Win95 controls

		MInitLog(MLOG_LEVEL_DEBUG,MLOG_DEBUG | MLOG_PROG, MCLogCallBack);
	}

	return (TRUE);
}

__declspec( dllexport ) const TCHAR* LibDescription()
{
	return GetString(IDS_LIBDESCRIPTION);
}

__declspec( dllexport ) int LibNumberClasses()
{
	return 1;
}

__declspec( dllexport ) ClassDesc* LibClassDesc(int i)
{
	switch(i) 
	{
		case 0: return GetMCplug2Desc();
		default: return 0;
	}
}

__declspec( dllexport ) ULONG LibVersion()
{
	return VERSION_3DSMAX;
}

TCHAR *GetString(int id)
{
	static TCHAR buf[256];

	if (hInstance)
		return LoadString(hInstance, id, buf, sizeof(buf)) ? buf : NULL;
	return NULL;
}

///////////////////////////////////////////////////

class MCplug2ClassDesc:public ClassDesc2
{
public:
	int 			IsPublic() {return 1;}
	void *			Create(BOOL loading = FALSE) { return new MCplug2; }
	const TCHAR *	ClassName()		{return GetString(IDS_CLASS_NAME);}
	SClass_ID		SuperClassID()	{return SCENE_EXPORT_CLASS_ID;}
	Class_ID		ClassID()		{return MCPLUG3_CLASS_ID;}
	const TCHAR* 	Category()		{return GetString(IDS_CATEGORY);}
	const TCHAR*	InternalName()	{ return _T("MCplug2"); }	// returns fixed parsable name (scripter-visible name)
	HINSTANCE		HInstance()		{ return hInstance; }		// returns owning module handle
};
static MCplug2ClassDesc MCplug2Desc;
ClassDesc2* GetMCplug2Desc() {return &MCplug2Desc;}

///////////////////////////////////////////////////

void MCRCollisionTreeBuilder::OnUpdateProgress(int nTotalPoly, int nCurrentPoly)
{
	m_ip->ProgressUpdate((int)((float)nCurrentPoly/nTotalPoly * 100.0f)); 
}

///////////////////////////////////////////////////

// Dummy function for progress bar
DWORD WINAPI fn(LPVOID arg)
{
	return(0);
}

/////////////////////////////////////////////////////////

#define KEY_GAP				1

MCplug2::MCplug2()
{
//	bAlwaysSample	= true;
	m_nKeyFrameStep	= KEY_GAP;
//	nMeshFrameStep	= KEY_GAP;
//	nPrecision		= KEY_GAP;
	nStaticFrame	= 0;

	m_pCurrentCell	= NULL;
}

MCplug2::~MCplug2()
{
}

int MCplug2::ExtCount() 
{	
	return 3;
}

const TCHAR *MCplug2::Ext(int n)
{		
	switch(n) 
	{
	case 0:
		return _T("elu");

	case 1:
		return _T("rs");

	case 2:
		return _T("ani");
	}
	return _T("");
}

const TCHAR *MCplug2::LongDesc()
{
	return GetString(IDS_LONGDESC);
}
	
const TCHAR *MCplug2::ShortDesc() 
{			
	return GetString(IDS_SHORTDESC);
}

const TCHAR *MCplug2::AuthorName()
{			
	return _T("elhorus");
}

const TCHAR *MCplug2::CopyrightMessage() 
{	
	return GetString(IDS_COPYRIGHT);
}

const TCHAR *MCplug2::OtherMessage1() 
{		
	return _T("");
}

const TCHAR *MCplug2::OtherMessage2() 
{		
	return _T("");
}

unsigned int MCplug2::Version()
{				
	return 100;
}

void MCplug2::ShowAbout(HWND hWnd)
{			
}

BOOL MCplug2::SupportsOptions(int ext, DWORD options)
{
	return(options == SCENE_EXPORT_SELECTED) ? TRUE : FALSE;
}

int MCplug2::DoExport(const TCHAR *name,ExpInterface *ei,Interface *i, BOOL suppressPrompts, DWORD options) 
{
	exportSelected = (options & SCENE_EXPORT_SELECTED) ? TRUE : FALSE;

	// 에러스트링 초기화
	g_errors.clear();

	ip = i;// 3DMAX 정보 인터페이스...

	// 파일 로딩 경로를 설정한다.
	// 파일 로딩 경로가 추가되면, 추가로 경로를 Add해줘야 한다.
	MCreateFileSystem( MFILEACCESS_GENERIC, GetCfgFilePath() );
	m_options.ReadConfig( GetCfgFilename() );

	m_nBeginFrame = 0;

	//////////////////////////////////
	// run dlg
	if(!suppressPrompts)
	{
		if (!DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_ELU_EXPORT),ip->GetMAXHWnd(), MCConfig::EluDlgProc, (LPARAM)this)) 
			return 1;
	}
	else
	{ 
		// 맵에서 불리는 경우, 아무 것도 하지 않는다.
	}

	m_options.WriteConfig( GetCfgFilename() );

	strncpy( m_szFileName, name, strlen(name)-4);

	//////////////////////////////////////////////////////
	//////////////////////////////////////////////////////

	ip->ProgressStart( GetString(IDS_PROGRESS_MSG1), TRUE, fn, NULL );

	// 애니메이션에 따라 첫프레임이 꼭 0 이 아닐수도 있다, static frame 은 첫 프레임으로 설정,
	SetStaticFrame( ip->GetAnimRange().Start() );

	m_nCurNode = 0;

	m_bCustomSampling = false;
	m_pBip01Node = NULL;
	m_pDummyLocNode = NULL;
	m_eDummyLocType = DUMMY_LOC_NONE;

	ExportProperties();

	ExportPrerequisite( ip->GetRootNode() );

	// keeper에 time관련 정보를 set
	m_meshList.m_pMaterialKeeper->SetStartTime( ip->GetAnimRange().Start());
	m_meshList.m_pMaterialKeeper->SetEndTime  ( ip->GetAnimRange().End());
	m_meshList.m_pMaterialKeeper->SetDeltaTime( GetTicksPerFrame());

	m_meshList.m_pMaterialKeeper->GetMaterialList(ip->GetRootNode(), this);
	
	ExportGlobalInfo();

	m_pGlobalCell = new RCell;
	m_pGlobalCell->m_strName = "global";
	m_sceneBuilder.AddCell(m_pGlobalCell);

	m_pCurrentCell = m_pGlobalCell;

	m_pIGame = GetIGameInterface();
	bool bIsExportSelected = false;
	m_pIGame->InitialiseIGame(bIsExportSelected);
	m_pIGame->SetStaticFrame(0); //staticFrame	// Normal Data만 수집할 용도라 Static 프레임이어도 상관없다.

	m_nTotalNodeCount = 0;
	GetNodeCount(ip->GetRootNode(), m_nTotalNodeCount); // 전체 노드 개수를 구한다
	int numChildren = ip->GetRootNode()->NumberOfChildren();
	for (int idx=0; idx<numChildren; idx++)
	{
		if (ip->GetCancel())	break;

		ExportAll(ip->GetRootNode()->GetChildNode(idx));
	}

	ip->ProgressEnd();

	char filename[256];
	char file_bin[256];
	char file_obstacle[256];

	// 확장자 벗겨냄
	string nameString = name;
	string pathString = MGetPurePath( name );
	string fileNameString = MGetPureFileName( nameString );
	{
		string ext = MGetPureExtension( fileNameString );
		if( ext == ".elu" )
			fileNameString = MGetPureFileName( fileNameString );
	}
	sprintf( filename, "%s%s", pathString.c_str(), fileNameString.c_str() );

	sprintf(file_bin, "%s.elu",filename );
	sprintf(file_obstacle, "%s.obstacle",filename );

	m_meshList.CalcParentID();
	m_meshList.m_pMaterialKeeper->ClearVoidMaterial();	// 빈 mtrl 을 제거한다..

	// 합칠 수 있는 메시노드를 합쳐서 최적화
	if( /*m_options.m_bMeshOut &&*/ m_options.m_bAttachMesh )
	{
		m_meshList.AttachMeshNodes();
	}

	m_meshList.CalcPhysiquePID();	// physique 이름으로 부모 id 를 연결하고 base bone matrix 를 생성한다..
	m_meshList.CalcBufferIndex();

	// changemtrl 수집하고 dds 출력하게 순서 변경 - 09.04.24 by jun
	//Get the material editor's materials
	const int NBSlotMEditor = 24;	// 전체 Material Slot가 24개(6 * 4)
	for (int i=0; i<NBSlotMEditor; i++)
	{
		//Get Material
		MtlBase* CurMatSlot = ip->GetMtlSlot(i);
		if( !CurMatSlot->IsMultiMtl() )	// Sub & Multi 일때만
			continue;

		//for( int i = 0; i < m_meshList.m_MeshCount; ++i )
		//{
			if( _stricmp( CurMatSlot->GetName(), "changemtrl" ) == 0 ) 
			{
				m_meshList.m_pMaterialKeeper->AddSubMtl((Mtl*)CurMatSlot, NULL, true);
			}
		//}
	}
	m_meshList.m_pMaterialKeeper->ClearVoidMaterial();	// 빈 mtrl 을 제거한다..

	//////////////////////////////////////////////////////////////////////////
	// export textures 
	// dds 로 컨버팅 하고나서 저장된 텍스쳐의 이름을 dds 로 바꿔준다
	// export_desc_xml 을 저장할때 텍스쳐 경로가 삭제된다. 따라서 먼저 컨버트를 해준다
	string strTextureExportFolder;
	if(m_options.m_strTextureExportFolder.empty())
		strTextureExportFolder = MGetPurePath(file_bin);
	else
		strTextureExportFolder = m_options.m_strTextureExportFolder;

	D3DFORMAT plainFormat = D3D_DDS_FORMAT[m_options.m_fmtPlain];
	D3DFORMAT alphaFormat = D3D_DDS_FORMAT[m_options.m_fmtAlpha];
	// dds 사용 체크가 안되어있으면 export textures 가 비활성화된다. 출력할필요없다
	m_meshList.m_pMaterialKeeper->ExportTextures( m_options.m_bUseDDS, 
		m_options.m_bUseDDS ? m_options.m_bExportTextures : false, 
		plainFormat, alphaFormat, strTextureExportFolder );


	if (m_options.m_bMeshOut && !m_meshList.IsEmpty() )
	{
		m_meshList.export_bin(file_bin);
		m_meshList.export_desc_xml(file_bin);
		m_meshList.export_obstacle(file_obstacle);
	}

	if (m_options.m_bStaticMesh)
	{
		char szFileStaticMeshName[256];
		sprintf(szFileStaticMeshName,"%s.rs", filename);

		m_meshList.export_obstacle(file_obstacle);

		if(!m_sceneBuilder.IsEmpty())
		{
			m_meshList.export_desc_xml(szFileStaticMeshName, false);
			SaveStaticMesh(szFileStaticMeshName, m_options.m_nPolygonPerNodes);
		}
	}

	if(m_options.m_bOptimize_keys)			m_meshList.OptimizeKeys();
	if(m_options.m_bKeyAnimationOut)		m_meshList.export_ani(file_bin,RAniType_Bone);		// TODO: 둘중하나만 하자
	if(m_options.m_bVertexAnimationOut)		m_meshList.export_ani(file_bin,RAniType_Vertex);	// TODO: 둘중하나만 하자
	
	if( m_options.m_bMeshOut || m_options.m_bStaticMesh )
		SaveSceneXML(file_bin);

	vector<string> vecNotUnique;
	int nIsNotUnique = 0;
	// 맥스 오브젝트 이름이 같으면 익스포트 후 경고 메시지를 보여준다.
	for (int i = 0; i < m_meshList.m_MeshCount; ++i )
	{
		for (int j = i + 1; j < m_meshList.m_MeshCount; ++j )
		{
			if ( strcmp(m_meshList.m_MeshTable[i]->m_Name.c_str(), m_meshList.m_MeshTable[j]->m_Name.c_str()) == 0 )
			{
				vecNotUnique.push_back(m_meshList.m_MeshTable[i]->m_Name);
				nIsNotUnique++;
			}
		}
	}
	if ( nIsNotUnique )
	{
		mlog3("(%d)개의 오브젝트 이름이 같습니다\n", nIsNotUnique);
		for( size_t i = 0; i < vecNotUnique.size(); ++i )
			mlog3(vecNotUnique[i].c_str());
	}

	m_meshList.DelAll();
	m_sceneBuilder.Destroy();

	::ShowErrorMessage();

	m_pCurrentCell = NULL;

	return 1;
}

void MCplug2::GetNodeCount(INode* node, int& nodeCount)
{
	nodeCount++;

	for (int c = 0; c < node->NumberOfChildren(); c++) {
		GetNodeCount(node->GetChildNode(c), nodeCount);
	}
}

void MCplug2::ExportPrerequisite(INode* node)
{
	// dummy_loc 가 있는지 체크
	if( strcmp(node->GetName(), "dummy_loc") ==0 )
	{
		m_pDummyLocNode = node;
		m_eDummyLocType = DUMMY_LOC;
	}
	else
	if( strcmp(node->GetName(), "dummy_loc_dir") ==0 )
	{
		m_pDummyLocNode = node;
		m_eDummyLocType = DUMMY_LOC_DIR;
	}	
	if( strcmp(node->GetName(), "dummy_loc_manual") ==0 )
	{
		m_pDummyLocNode = node;
		m_eDummyLocType = DUMMY_LOC_MANUAL;
	}	
	// Bip01 체크
	else
	if( strcmp(node->GetName(), "Bip01") ==0 )
		m_pBip01Node = node;

	Object* obj = node->EvalWorldState(0).obj;
	if (obj && obj->ClassID()==RTTGROUP_CLASS_ID )
		ExportRTTGroup(node);

	for (int c = 0; c < node->NumberOfChildren(); c++) {
		ExportPrerequisite(node->GetChildNode(c));
	}
}

TSTR MCplug2::GetCfgFilePath()
{
	TSTR filepath;
	
	filepath += ip->GetDir(APP_PLUGCFG_DIR);
	filepath += "\\";

	return filepath;
}

TSTR MCplug2::GetCfgFilename()
{
	TSTR filename;
	
	filename += GetCfgFilePath();
	filename += CFGFILENAME;
	return filename;
}

BOOL MCplug2::ExportAll(INode* node) 
{
	if(exportSelected && node->Selected() == FALSE)
	{
		return FALSE;
	}

	string strName = node->GetName();

	m_nCurNode++;
	ip->ProgressUpdate((int)((float)m_nCurNode/m_nTotalNodeCount*100.0f)); 

	if (ip->GetCancel())	return FALSE;

	RCell* pOldCell = m_pCurrentCell;
	
	ERESULT eResult = CONTINUE_EXPORT;

	if(!exportSelected || node->Selected())
	{
		ObjectState os = node->EvalWorldState(0); 

		if (os.obj)
		{
			switch(os.obj->SuperClassID())
			{
				case GEOMOBJECT_CLASS_ID:
					{
						bool bCellNode = _strnicmp(strName.c_str(), RTOK_MAX_CELL, strlen(RTOK_MAX_CELL) )==0;
						bool bPortalNode = _strnicmp(strName.c_str(), RTOK_MAX_PORTAL, strlen(RTOK_MAX_PORTAL) )==0;

						if(bCellNode)
						{
							RCell* pNewCell = MakeCell(node);
							if(m_pCurrentCell!=m_pGlobalCell)
								mlog3("cell 안에 다른 cell 이 있을수 없습니다");

							m_pCurrentCell = pNewCell;
							m_sceneBuilder.AddCell(pNewCell);

							// 메시 익스포트 할 때 계층구조가 깨지는 것을 방지하기 위해 셀을 더미로 취급해서 뽑아준다.
							if(!m_options.m_bStaticMesh)
							{
								ExportGeometryObject(node);
							}
						}
						else if(bPortalNode)
						{
							RPortal* pNewPortal = MakePortal(node);

							if( pNewPortal )
								m_sceneBuilder.m_portals.push_back( pNewPortal);
						}
						else
						{
							ExportGeometryObject(node); 
						}
					}
					break;

				case LIGHT_CLASS_ID:
					ExportLight(node);
					break;

				case HELPER_CLASS_ID: 
					eResult = ExportHelperObject(node); 
					break;
			}
		}
	}
	
	if( eResult != DONT_EXPORT_CHILDREN )
	{
		for (int c = 0; c < node->NumberOfChildren(); c++) {
			ExportAll( node->GetChildNode(c) );
		}
	}

	m_pCurrentCell = pOldCell;

	return TRUE;
}

// XRefinXRef나 일반Group이 아닌 루트 XREFGROUP 인가
// 루트 그룹은 Dummy이고 그룹헤더이고 자식으로 XREF를 하나이상 가진다. 
bool MCplug2::IsXRefRootGroup(INode* node)
{
	Object *obj = node->GetObjectRef();
	if ( node->IsGroupHead() )
	{
		// Dummy가 XRef 이면 패스 
		if (obj->SuperClassID() == SYSTEM_CLASS_ID && obj->ClassID() == XREFOBJ_CLASS_ID) 
			return false; //루트가 아님
		
		// 자식중에 XRef가 하나도 없으면 일반 Group이다
		for (int c = 0; c < node->NumberOfChildren(); c++) 
		{
			INode* pChildNode = node->GetChildNode(c);
			Object *ChildObj = pChildNode->GetObjectRef();
			// 하나라도 있으면 XRef Root 그룹
			if (ChildObj->SuperClassID()==SYSTEM_CLASS_ID && ChildObj->ClassID()==XREFOBJ_CLASS_ID) 
			{
				return true;
			}
		}
	}

	return false;
}

bool MCplug2::IsStatic(INode* node)
{
	ObjectState os = node->EvalWorldState(0); 
	string strName = node->GetName();

	bool bWater			= false;
	bool bParentWater	= false;
	bool bIgnore		= false;
	bool bStatic		= false;

	// 모든 조건을 만족하면 일반 elu로 출력할 메쉬이다.
	if ( node->IsGroupHead() )					// Group Dummy는 제외 (Property를 가지고 있지 않다)
		return true;
	if( strstr( node->GetName(), "ex_") )		// 'ex_'는 이름 검색으로 제외
		return true;
	if( strstr( node->GetName(), "ground_") )	// 'ground_'도 제외
		return true;
	if( os.obj->SuperClassID() == LIGHT_CLASS_ID )	// Light도 제외
		return true;
	if( _strnicmp(strName.c_str(), RTOK_MAX_CELL, strlen(RTOK_MAX_CELL) ) == 0 )	// CELL
		return true;
	if( _strnicmp(strName.c_str(), RTOK_MAX_PORTAL, strlen(RTOK_MAX_PORTAL) ) == 0 )// PORTAL
		return true;

	// 부모가 물이면 제외
	INode* pParentNode = node->GetParentNode();
	if( pParentNode )
		GetUserPropertyString( pParentNode, "set_water", &bParentWater );

	GetUserPropertyString( node, "set_water", &bWater );
	GetUserPropertyString( node, "set_export_ignore", &bIgnore );
	GetUserPropertyString( node, "set_static", &bStatic );

	if( bWater || bParentWater || bIgnore || bStatic )
		return true;
	
	return false;
}

// Record의 목록에 해당하는 XRefObject가 있는지 검색하는 재귀 함수
bool MCplug2::IsXRefinRecord(IObjXRefRecord *XRecord, string dstNodeName)
{
	Tab<IObjXRefRecord*> tabChild;
	int nChildCount = XRecord->GetChildRecords(tabChild);
	for( int i = 0; i < tabChild.Count(); ++i )
	{
		string strRecordName;

#ifdef MAX_RELEASE_R13	// Max 2011
		MaxSDK::AssetManagement::AssetUser assetSrcFile = tabChild[i]->GetSrcFile();
		string strFilename = assetSrcFile.GetFileName();
		strRecordName = MGetPureFileName( strFilename );

#else
		strRecordName = MGetPureFileName( tabChild[i]->GetSrcFileName() );
#endif

		if ( strcmp(dstNodeName.c_str(), strRecordName.c_str()) == 0 )
			return true;

		// 재귀
		if( IsXRefinRecord(tabChild[i], dstNodeName) )
			return true;
	}

	return false;
}

// true이면 XRef의 XRef 이고, false 이면 일반 Group 이다.
bool MCplug2::IsXRefinXRef(INode* node)
{
	Object *XObj = node->GetObjectRef();
	if ( XObj && XObj->SuperClassID()==SYSTEM_CLASS_ID && XObj->ClassID()==XREFOBJ_CLASS_ID ) 
	{
		IObjXRefRecord *XRecord = ( (IXRefObject8 *)XObj )->GetActiveRecord();
		string strNodeName = MGetPureFileName( node->GetName() );

		// 현재 Active Record 부터 하위 목록을 검색한다
		string strRecordName;

#ifdef MAX_RELEASE_R13	// Max 2011
		MaxSDK::AssetManagement::AssetUser assetSrcFile = XRecord->GetSrcFile();
		string strFilename = assetSrcFile.GetFileName();
		strRecordName = MGetPureFileName( strFilename );

#else
		strRecordName = MGetPureFileName( XRecord->GetSrcFileName() );
#endif

		if ( strcmp(strNodeName.c_str(), strRecordName.c_str()) == 0 )
			return true;

		// XRef Record 목록에 해당되면 출력하지 않는 XRef의 XRef로 처리
		return IsXRefinRecord(XRecord, strNodeName);
	}

	return false;
}

// 하위 노드가 모두 static 인가
bool MCplug2::IsAllStatic(INode* node)
{
	int nChild = node->NumberOfChildren();
	string strRootName = node->GetName();
	for (int c = 0; c < node->NumberOfChildren(); c++) 
	{
		INode* pChildNode = node->GetChildNode(c);
		string strName = pChildNode->GetName();

		// static 메쉬인가
		if ( IsStatic(pChildNode) == false )
			return false;

		// XRef 안의 XRef 이면 자식 노드 검색하지 않고 패스
		if( pChildNode->IsGroupHead() && IsXRefinXRef(pChildNode) )
			continue;

		// 아니면 자식 노드들 순회
		if( node->NumberOfChildren() && IsAllStatic(pChildNode) == false)
			return false;
	}

	return true; // 모든 하위 목록이 Static 이면 출력하지 않는다.
}

void MCplug2::ExportRTTGroup(INode* node)	///< 라이트맵을 위한 이름규칙
{
	string strLightMapName = node->GetName();
	strLightMapName+="_lightmap.tga";

	string strProperty;

	TSTR userPropBuffer;
	node->GetUserPropBuffer(userPropBuffer);

	istringstream iStringBuffer(userPropBuffer.data());
	string aLine;
	while(getline(iStringBuffer,aLine))
	{
		// trim right
		int n = aLine.find_last_not_of(" \n\r");
		if(n==string::npos) continue;
		string buf = aLine.substr(0,n+1);

		stringstream sstream(buf); // string을 stream에 넣는다
		sstream >> aLine;

		// 나머지 user property는 무시
		if(strcmp(aLine.c_str(),"set_visible_at_night")==0)
		{
			sstream >> aLine;	// = 
			sstream >> aLine;	// true / false
			if(strcmp(aLine.c_str(), "true") == 0)
				strProperty = "set_visible_at_night = true";
		}
	}

	IRTTGroup* pRTTGroup = NULL;
	Object* obj = node->EvalWorldState(0).obj;
	if(obj)
		pRTTGroup = (IRTTGroup*)(obj->GetInterface( RTTGROUP_INTERFACE ) );

	m_meshList.m_pMaterialKeeper->AddLightMap(strLightMapName, strProperty, pRTTGroup );
}

bool MCplug2::IsXRefExport(INode* node)
{
	string strName = node->GetName();

	if( IsXRefRootGroup(node) || IsXRefinXRef(node) )	// 루트 그룹이거나 XRef 그룹일때만 배치한다 - 일반 Group은 걸러짐
	{
		if( IsAllStatic(node) == false )	// 모든 하위 노드가 static 이 아닐때만 출력한다
			return true;
	}
	return false;
}

MCplug2::ERESULT MCplug2::ExportHelperObject(INode* node)
{
	// 허용된 헬퍼오브젝트 이외는 출력하지 않는다
	Object* helperObj = node->EvalWorldState(0).obj;
	if (!helperObj) return CONTINUE_EXPORT;

	TSTR className;
	helperObj->GetClassName(className);

	if( strcmp(className, "Dummy") != 0 &&
		strcmp(className, "PointHelper") != 0 )
		return CONTINUE_EXPORT;

	string strNodeName = node->GetName();

	// 그룹에 대한 처리
	Object *obj = node->GetObjectRef();
	if ( node->IsGroupHead() )
	{
		if(IsXRefExport(node))
		{
			m_XRefList.push_back( node );

			// 스태틱 메시 익스포트라면 XRefGroup 자식들을 모두 익스포트 해야 함
			if( !m_options.m_bStaticMesh )
				return DONT_EXPORT_CHILDREN;
		}
	}
	else
	{
		ExportGeometryObject(node);
	}

	return CONTINUE_EXPORT;
}

void MCplug2::ExportProperties()
{
	int nCount = ip->GetNumProperties(PROPSET_USERDEFINED );
	for( int i=0; i< nCount ; ++i)
	{
		char szName[256];
		const PROPSPEC* pProspec = ip->GetPropertySpec ( PROPSET_USERDEFINED, i);

		// olestr -> ansi str
		if( WideCharToMultiByte(CP_ACP, 0, pProspec->lpwstr, -1, szName, sizeof(szName), NULL, NULL) )
		{
			const PROPVARIANT* pProvariant = ip->GetPropertyVariant (PROPSET_USERDEFINED, i);

			// file properties 중 custom animation sampling 범위 얻기
			if( stricmp(szName,"CustomAnimationSampling")==0 )
			{
				string strValue = pProvariant->pszVal; 
				size_t posHyphen = strValue.find_first_of("-");
				size_t posComma = strValue.find_first_of(",");	
				size_t posInterval = strValue.find("interval");	
				size_t posEqual = strValue.find_first_of("=", posInterval+8);	
				if(posHyphen!=string::npos && posComma!=string::npos && posInterval!=string::npos)
				{
					int nTicksPerFrame = GetTicksPerFrame();

					m_bCustomSampling = true;
					m_nCustomSamplingStart = atoi(strValue.substr(0, posHyphen).c_str() ) * nTicksPerFrame;
					m_nCustomSamplingEnd = atoi(strValue.substr(posHyphen+1 , posComma - posHyphen).c_str() ) * nTicksPerFrame;
					m_nCustomSamplingInterval = atoi(strValue.substr(posEqual+1).c_str() );
				}
			}
			else
			if( stricmp(szName,"Alpha2Pass")==0 )
			{
				bool alpha2Pass = ( pProvariant->boolVal == 0 ) ? false : true;
				m_meshList.m_bAlpha2Pass = alpha2Pass;
			}
			else
			if( stricmp(szName,"AutoAnimation")==0 )
			{
				bool autoAnimation = ( pProvariant->boolVal == 0 ) ? false : true;
				m_meshList.m_bAutoAnimation = autoAnimation;
			}
			else
			if( stricmp(szName,"FogCulling")==0 )
			{
				bool fogCulling = ( pProvariant->boolVal == 0 ) ? false : true;
				m_meshList.m_bFogCulling = fogCulling;
			}
			else
			if( stricmp(szName,"EnableDistanceCulling")==0 )
			{
				bool enableDistanceCulling = ( pProvariant->boolVal == 0 ) ? false : true;
				m_meshList.m_bDistanceCulling = enableDistanceCulling;
			}
		}
	}
}

void MCplug2::ExportGlobalInfo()
{
//	frame 은 0부터 시작해야 한다.

	Interval range = ip->GetAnimRange();

	m_nBeginFrame = range.Start();

	int frame = range.End() - range.Start();

	m_meshList.m_nMaxFrame = frame / GetTicksPerFrame();
}

//////////////////////////////////////////////////////////////////
// util

BOOL MCplug2::TMNegParity(Matrix3 &m)
{
	return (DotProd(CrossProd(m.GetRow(0),m.GetRow(1)),m.GetRow(2))<0.0)?1:0;
}

#define CTL_CHARS  31
#define SINGLE_QUOTE 39

TCHAR* MCplug2::FixupName(TCHAR* name)
{
	static char buffer[256];
	TCHAR* cPtr;
	
    _tcscpy(buffer, name);
    cPtr = buffer;
	
    while(*cPtr) {
		if (*cPtr == '"')
			*cPtr = SINGLE_QUOTE;
        else if (*cPtr <= CTL_CHARS)
			*cPtr = _T('_');
        cPtr++;
    }
	
	return buffer;
}

bool MCplug2::SaveSceneXML_StaticMeshNode( MXmlElement* pElement, const char* szPureFileName )
{
	if(!m_options.m_bStaticMesh) return false;
	if(m_sceneBuilder.IsEmpty()) return false;

	MXmlElement* pNodeElement = new MXmlElement( "STATICMESH" );
	pElement->LinkEndChild(pNodeElement);

	MXmlElement* pCommonElement = new MXmlElement("COMMON");
	pNodeElement->LinkEndChild(pCommonElement);
	_SetAttribute(pCommonElement,"name",szPureFileName);

	MXmlElement* pPropertyElement = new MXmlElement("PROPERTY");
	pNodeElement->LinkEndChild(pPropertyElement);

	MXmlElement* pFileNameElement = new MXmlElement("FILENAME");
	pPropertyElement->LinkEndChild(pFileNameElement);

	_SetContents(pFileNameElement,string(szPureFileName) + ".rs" );

	////////////////////////////////////////////////////////////////////////
	// staticmesh 의 children

	MXmlElement* pSceneElement = new MXmlElement("SCENE");
	pNodeElement->LinkEndChild(pSceneElement);

	// 이펙트
	m_effectList.Save( pSceneElement, true);
	// 라이트
	m_lightList.Save(pSceneElement, true);
	// XRefActor
	SaveSceneXML_XRefActor( pSceneElement );

	return true;
}

bool MCplug2::SaveSceneXML_Actor( MXmlElement* pElement, const char* szPureFileName )
{
	if(!m_options.m_bMeshOut) return false;
//	if( m_meshList.IsEmpty()) return false;	// XRef만 있어도 배치 정보는 출력 되어야 한다.

	MXmlElement* pSceneElement = NULL;

	if( m_meshList.IsEmpty() == false )
	{
		MXmlElement* pNodeElement = new MXmlElement( "ACTOR");
		pElement->LinkEndChild(pNodeElement);

		MXmlElement* pCommonElement = new MXmlElement("COMMON");
		pNodeElement->LinkEndChild(pCommonElement);
		_SetAttribute(pCommonElement,"name",szPureFileName);

		MXmlElement* pPropertyElement = new MXmlElement("PROPERTY");
		pNodeElement->LinkEndChild(pPropertyElement);

		MXmlElement* pFileNameElement = new MXmlElement("FILENAME");
		pPropertyElement->LinkEndChild(pFileNameElement);

		_SetContents(pFileNameElement,string(szPureFileName) + ".elu" );

		if( m_meshList.m_bAutoAnimation )
		{
			MXmlElement* pAutoAnimationElement = new MXmlElement("ANI");
			pPropertyElement->LinkEndChild(pAutoAnimationElement);
		}

		if( m_meshList.m_bFogCulling == false ) // 포그 컬링을 하는게 기본값이라서 false일때만 저장한다
		{
			MXmlElement* pFogCullingElement = new MXmlElement("FOGCULLINGOFF");
			pPropertyElement->LinkEndChild(pFogCullingElement);
		}

		if( m_meshList.m_bDistanceCulling == false )
		{
			MXmlElement* pFogCullingElement = new MXmlElement("DISTANCECULLOFF");
			pPropertyElement->LinkEndChild(pFogCullingElement);
		}

		////////////////////////////////////////////////////////////////////////
		// actor 의 children 으로 붙임

		pSceneElement = new MXmlElement("SCENE");
		pNodeElement->LinkEndChild(pSceneElement);
	}
	else
	{
		// actor 가 없는경우 바로 자식을을 붙여준다
		pSceneElement = pElement;
	}

	//////////////////////////////////////////////////////////////////////////
	// children nodes

	// 이펙트
	m_effectList.Save(pSceneElement, false);
	// 라이트
	m_lightList.Save(pSceneElement, false);
	// XRefScene
	SaveSceneXML_XRefActor( pSceneElement );

	return true;
}

bool MCplug2::SaveSceneXML_Occluder( MXmlElement* pElement )
{
	if( 0 == m_meshList.m_OccluderPointList.size() ) return true;


	MXmlElement* pNodeElement = new MXmlElement( "OCCLUDER");
	pElement->LinkEndChild(pNodeElement);

	MXmlElement* pPropertyElement = new MXmlElement("PROPERTY");
	pNodeElement->LinkEndChild(pPropertyElement);

	char buffer[2048];

	for(int i=0;i<(int)m_meshList.m_OccluderPointList.size();i++)
	{
		MXmlElement* pAnOccluderElement = new MXmlElement("OCCLUDERPOINT");
		pPropertyElement->LinkEndChild(pAnOccluderElement);	

		for(int j=0;j<(int)m_meshList.m_OccluderPointList[i].size();j++)
		{
			MXmlElement* pOccluderPointElement = new MXmlElement("P");
			pAnOccluderElement->LinkEndChild(pOccluderPointElement);
			const MVector3& OccluderPoint = m_meshList.m_OccluderPointList[i][j];
			sprintf(buffer,"%f %f %f",OccluderPoint.x,OccluderPoint.y,OccluderPoint.z);
			_SetContents(pOccluderPointElement,buffer);
		}
	}

	return true;
}

bool MCplug2::SaveSceneXML_WaterPlane( MXmlElement* pElement )
{
	char buff[256];

	for( size_t i = 0; i < m_waterList.size(); ++i )
	{
		MXmlElement* pWaterElement = new MXmlElement( "WATERPLANE" );
		pElement->LinkEndChild( pWaterElement );

		SaveSceneXML_Common( pWaterElement, m_waterList[i] );

		MXmlElement* pWaterPropertyElement = new MXmlElement( "PROPERTY" );
		pWaterElement->LinkEndChild( pWaterPropertyElement );

		string waterMeshFileName = m_waterList[i]->GetName();
		waterMeshFileName += ".elu";
		MXmlElement* pMeshFileElement = new MXmlElement( "MESHFILE" );
		_SetContents( pMeshFileElement, waterMeshFileName.c_str() );
		pWaterPropertyElement->LinkEndChild( pMeshFileElement );
		
		MXmlElement* pWaterPortalsElement = new MXmlElement( "PORTALS" );
		pWaterPropertyElement->LinkEndChild( pWaterPortalsElement );
		for( int j = 0; j < m_waterList[i]->NumberOfChildren(); ++j )
		{
			MXmlElement* pWaterPortalElement = new MXmlElement( "PORTAL" );
			pWaterPortalsElement->LinkEndChild( pWaterPortalElement );

			RPortal* pNewPortal = MakePortal(m_waterList[i]->GetChildNode(j));
			if( pNewPortal )
			{
				for( int k = 0; k < pNewPortal->nVertices; ++k )
				{
					MXmlElement* pChild = new MXmlElement( RTOK_POSITION );
					pWaterPortalElement->LinkEndChild( pChild );

					sprintf( buff, "%.7f %.7f %.7f",
						pNewPortal->m_pVertices[k].x, pNewPortal->m_pVertices[k].y, pNewPortal->m_pVertices[k].z );

					_SetContents( pChild, buff );
				}
				delete pNewPortal;
			}
		}
	}

	return true;
}

void MCplug2::SaveSceneXML_XRefActor( MXmlElement* pElement )
{
	for( size_t i = 0; i < m_XRefList.size(); ++i )
	{
		INode* pNode = m_XRefList[i];

		MXmlElement* pActorElement = new MXmlElement( "SCENEINSTANCE" );
		pElement->LinkEndChild(pActorElement);

		SaveSceneXML_Common( pActorElement, pNode );

		MXmlElement* pPropertyElement = new MXmlElement("PROPERTY");
		pActorElement->LinkEndChild(pPropertyElement);

		MXmlElement* pFileNameElement = new MXmlElement("FILENAME");
		pPropertyElement->LinkEndChild(pFileNameElement);

		string strPureFileName = MGetPureFileName(pNode->GetName());
		strPureFileName += ".scene.xml";

		_SetContents(pFileNameElement, strPureFileName.c_str());
	}
}

bool MCplug2::SaveSceneXML_Common( MXmlElement* pElement, INode* pNode )
{
	char szContents[256];

	MXmlElement* pCommonElement = new MXmlElement( "COMMON" );
	pElement->LinkEndChild(pCommonElement);
	_SetAttribute( pCommonElement, "name", pNode->GetName() );

	Matrix3 tm;
	AffineParts ap;
	tm = pNode->GetNodeTM(0);
	decomp_affine(tm, &ap);

	RVector vPosition( ap.t );
	RVector vScale( ap.k );

	Matrix3 tmRot;
	ap.q.MakeMatrix( tmRot );

	Point3 vD = tmRot.GetRow(1);
	RVector vDirection( vD );
	vDirection *= ap.f;

	Point3 vU = tmRot.GetRow(2);
	RVector vUP( vU );
	vUP *= ap.f;

	MXmlElement *pPosElement = new MXmlElement("POSITION");
	sprintf(szContents,"%f %f %f", vPosition.x, vPosition.y, vPosition.z);
	_SetContents(pPosElement, szContents);
	pCommonElement->LinkEndChild(pPosElement);

	MXmlElement *pDirElement = new MXmlElement("DIRECTION");
	sprintf(szContents,"%f %f %f", vDirection.x, vDirection.y, vDirection.z);
	_SetContents(pDirElement, szContents);
	pCommonElement->LinkEndChild(pDirElement);

	MXmlElement *pUpElement = new MXmlElement("UP");
	sprintf(szContents,"%f %f %f", vUP.x, vUP.y, vUP.z);
	_SetContents(pUpElement, szContents);
	pCommonElement->LinkEndChild(pUpElement);

	MXmlElement *pScaleElement = new MXmlElement("SCALE");
	sprintf(szContents,"%f %f %f", vScale.x, vScale.y, vScale.z);
	_SetContents(pScaleElement, szContents);
	pCommonElement->LinkEndChild(pScaleElement);

	return true;
}

bool MCplug2::SaveSceneXML(const char* szMeshFileName)
{
	char szPureFileName[256];
	GetPureFilename(szPureFileName,szMeshFileName);

	char szXmlFileName[_MAX_PATH];
	ReplaceExtension(szXmlFileName,szMeshFileName,"scene.xml");

	MXml xml;

	MXmlDeclaration* pDec = xml.CreateDeclaration("1.0", "UTF-8", "");
	xml.Doc()->LinkEndChild(pDec);

	MXmlElement* pRootElement = new MXmlElement("ROOT");
	xml.Doc()->LinkEndChild(pRootElement);

	// root scene
	MXmlElement* pRootSceneElement = new MXmlElement("SCENE");
	pRootElement->LinkEndChild(pRootSceneElement);

	// static mesh
	SaveSceneXML_StaticMeshNode( pRootSceneElement, szPureFileName );

	// actor
	SaveSceneXML_Actor( pRootSceneElement, szPureFileName );

	// occluder
	SaveSceneXML_Occluder( pRootSceneElement );

	// water plane
	SaveSceneXML_WaterPlane( pRootSceneElement );

	xml.SaveFile(szXmlFileName);
	return true;
}

void MCplug2::ConvertToD3DXMatrix(D3DXMATRIX& outD3DMat, const Matrix3 &m)
{
	D3DXMatrixIdentity(&outD3DMat);

	Point3 row;

	row = m.GetRow(0);

	outD3DMat._11 = row.x;
	outD3DMat._12 = row.y;
	outD3DMat._13 = row.z;

	row = m.GetRow(1);	

	outD3DMat._21 = row.x;
	outD3DMat._22 = row.y;
	outD3DMat._23 = row.z;

	row = m.GetRow(2);	

	outD3DMat._31 = row.x;
	outD3DMat._32 = row.y;
	outD3DMat._33 = row.z;

	row = m.GetRow(3);	

	outD3DMat._41 = row.x;
	outD3DMat._42 = row.y;
	outD3DMat._43 = row.z;
}

void MCplug2::ConvertToMMatrix(MMatrix& outMatrix, const Matrix3 &tm )
{
	outMatrix.MakeIdentity();

	Point3 row;

	row = tm.GetRow(0);
	outMatrix._11 = row.x;
	outMatrix._12 = row.y;
	outMatrix._13 = row.z;

	row = tm.GetRow(1);	
	outMatrix._21 = row.x;
	outMatrix._22 = row.y;
	outMatrix._23 = row.z;

	row = tm.GetRow(2);	
	outMatrix._31 = row.x;
	outMatrix._32 = row.y;
	outMatrix._33 = row.z;

	row = tm.GetRow(3);	
	outMatrix._41 = row.x;
	outMatrix._42 = row.y;
	outMatrix._43 = row.z;
}

bool MCplug2::SaveStaticMesh(const char* szStaticMeshFileName, int nPolygonPerNodes)
{
	if (m_sceneBuilder.Build(ip, m_meshList.m_pMaterialKeeper, nPolygonPerNodes) )
		return m_sceneBuilder.SaveRS( szStaticMeshFileName);
	else 
		return false;
}



void MCplug2::ExportOccluder( INode* node, RMaxMeshNode* pMeshNode )
{
	IGameNode* pGameNode = m_pIGame->GetIGameNode(node);
	if(!pGameNode)
	{
		MessageBox(NULL, "GetIGameNode() 실패", "ExportOccluder", MB_OK);
		return;
	}

	IGameObject* obj = pGameNode->GetIGameObject();
	IGameMesh* gmesh = (IGameMesh*)obj;
	bool bGMeshInitializeSuccess = gmesh->InitializeData();
	// 느려서 일단 주석처리
	if(bGMeshInitializeSuccess)
		gmesh->SetCreateOptimizedNormalList();

	Mtl* nodeMtl	= node->GetMtl();
	Matrix3 tm		= node->GetObjTMAfterWSM(GetStaticFrame());
	BOOL negScale	= TMNegParity(tm);

	Matrix3 nodetm = node->GetNodeTM(0);
	Matrix3 matSign(1);
	if(negScale)
		matSign = matNegative * matSign;

	ObjectState os = node->EvalWorldState(GetStaticFrame());
	if (!os.obj || os.obj->SuperClassID()!=GEOMOBJECT_CLASS_ID)
		return; 

	BOOL needDel = FALSE;
	TriObject* tri = GetTriObjectFromNode(node, GetStaticFrame(), needDel);
	if (!tri)
		return;

	Mesh* mesh = &tri->GetMesh();

	mesh->buildNormals();

	int point_num = mesh->getNumVerts();
	int face_num  = mesh->getNumFaces();

	pMeshNode->m_PointCnt = point_num;
	pMeshNode->m_PointTable = new D3DXVECTOR3[point_num];
	memset( pMeshNode->m_PointTable,0,sizeof(D3DXVECTOR3)*point_num );

	if( mesh->vcFace )
	{
		pMeshNode->m_PointColorCnt = point_num;
		pMeshNode->m_PointColorTable = new D3DXVECTOR3[ point_num ];// 정점과 순서 보장
	}

	// vertex position
	for (int i=0; i<mesh->getNumVerts(); i++)
	{
		Point3 v = tm * mesh->verts[i]  * matSign;
		pMeshNode->m_PointTable[i].x = v.x;
		pMeshNode->m_PointTable[i].y = v.y;
		pMeshNode->m_PointTable[i].z = v.z;
	}


	for ( int i = 0; i < face_num; ++i ) 
	{
		FaceEx* face = bGMeshInitializeSuccess ? gmesh->GetFace(i) : NULL;
		Face* f = &mesh->faces[i];
		RMaxFaceData *pFaceData = new RMaxFaceData;

		pFaceData->nDeg = 3;
		pFaceData->index = new RFaceSubData[3];

		pFaceData->triangleIndices = new unsigned char[3];
		pFaceData->triangleIndices[0] = 0;
		pFaceData->triangleIndices[1] = 1;
		pFaceData->triangleIndices[2] = 2;
		pMeshNode->m_FaceTable.push_back(pFaceData);

		for( int j = 0; j < 3; ++j )
		{
			int ncorner = negScale ? 3 - j -1 : j;	// 2 1 0
			// negative Mesh 일때 IGameMesh 는 삼각형 구축 순서가 다르다
			// 2 1 0 -> 1 2 0 으로 삼각형 구축 순서 변경
			int ncornernormal = negScale ? (j + 1) % 3 : j; // 1 2 0
			// Position
			pFaceData->index[j].p = (WORD)f->v[ncorner];
		}
	}

	pMeshNode->m_FaceCnt  = pMeshNode->m_FaceTable.size();


	//중복없는 점 목록 만듬
	typedef std::set<MVector3,MVector3_less<MVector3>> POINT_SET;
	POINT_SET PointSet;
	for(int i=0;i<pMeshNode->m_FaceCnt;i++)
	{
		RMaxFaceData* pFD = pMeshNode->m_FaceTable[i];
		for( int j = 0; j < 3; ++j )
		{
			WORD index = pFD->index[j].p;
			MVector3 Point;
			Point.x = pMeshNode->m_PointTable[index].x;
			Point.y = pMeshNode->m_PointTable[index].y;
			Point.z = pMeshNode->m_PointTable[index].z;

			PointSet.insert(Point);
		}
	}

	std::vector<MVector3>	PointArray;
	for(POINT_SET::iterator iter = PointSet.begin();iter != PointSet.end();iter++)
	{
		PointArray.push_back(*iter);
	}

	std::vector<MVector3> ConvexhullPoints;
	if( false ==  MConvexHullAlgorithm::MakeConvexHullByCounterClockWise3D(PointArray,ConvexhullPoints) )
	{
		mlog3("Occluder를 이루는 점들이 한평면상에 있지 못하여 해당 정보를 뽑지 못하였습니다. ");
		return;
	}

	m_meshList.m_OccluderPointList.push_back(ConvexhullPoints);
}

