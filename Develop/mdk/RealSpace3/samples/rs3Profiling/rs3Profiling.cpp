#include "rs3Profiling.h"

#include "RRenderHelper.h"
#include "RDebugPassRenderer.h"
#include "REnvironmentSet.h"
#include "RCameraSceneNode.h"
#include "REffectInstance.h"
#include "RPointLightSceneNode.h"
#include "REffectPassRenderer.h"
#include "RTreePassRenderer.h"
#include "RActorPassRenderer.h"
#include "RShaderEffectController.h"
#include "RShaderCompositeManager.h"
#include "RTimer.h"
#include "RDirectionalLightSceneNode.h"
#include "RTerrain.h"
#include "RTreeSceneNode.h"

#include "MFileSystem.h"

using namespace rs3;

//////////////////////////////////////////////////////////////////////////
// Global
//////////////////////////////////////////////////////////////////////////
float g_fMoveSpeed	= 20;

//const char* ACTOR_MESH = "Player/hf/hf.elu";
//const char* FACE_MESH = "Player/hf/hf_face_01.elu";
//const char* HAIR_MESH = "Player/hf/hf_hair_01.elu";
//const char* WEAPON_MESH = "weapons/1hd_03.elu";
//const char* SHIELD_MESH = "weapons/shield_03.elu";


//const char* ACTOR_MESH = "hf/hf.elu";
const char* ACTOR_MESH = "poluto/poluto.elu";


const char* FACE_MESH[] = { "hf_face_01.elu",  "hf_face_02.elu",  "hf_face_03.elu"};
const char* HAIR_MESH[] = { "hf_hair_01.elu", "hf_hair_02.elu", "hf_hair_03.elu"};
const char* WEAPON_MESH[] = { "2hd_03.elu", "2hd_01.elu", "bow_02.elu"};
const char* SHIELD_MESH[] = { "shield_03.elu", "shield_03.elu", "shield_03.elu"};
const char* ARMOR_MESH[] = { "hf_guard_armor.elu", "hf_heavy_chrome_red.elu", "hf_leather_pioneers.elu"};
const char* EFFECT_NAME[] = { "ef_slash_dmg1", "ef_slash_dmg2", "ef_slash_dmg3"};







//const char* ANIMATION_NAMES[] = { "1hs_atk1", "1hs_atk1_r", "1hs_atk1f", "1hs_atk1f_r" };
const char* ANIMATION_NAMES[] = { "idle", "atk1", "pain_1", "grip" };
const int ANIMATION_COUNT = sizeof(ANIMATION_NAMES) / sizeof(ANIMATION_NAMES[0]);
#define ACTOR_ROW_COUNT	3
//#define ACTOR_GAP		100
#define ACTOR_GAP		250	//( 250 + 500)
const int ACTOR_COUNT = ACTOR_ROW_COUNT * ACTOR_ROW_COUNT;


#define ACTOR_MAX_ROW 10
#define ACTOR_MAX_COL 10

//Test_map
// #define DEFAULT_X	(15390.f)
// #define DEFAULT_Y	(6690.f)
// #define DEFAULT_Z	(50.f)


// Ingen
// #define DEFAULT_X	(-3000.f)
// #define DEFAULT_Y	(-5070.f)
// #define DEFAULT_Z	(-100.f)

// Ingen
 #define DEFAULT_X	(51940.f)
 #define DEFAULT_Y	(46371.f)
 #define DEFAULT_Z	(1709.f)

class CFileSearch
{
public:
	typedef stdext::hash_map<std::string,std::string> FILE2PATH;

	static void FindAllFile(std::string FilePath,FILE2PATH& FileNameTable,std::string& DuplicateFiles)
	{

		WIN32_FIND_DATA FindFileData;
		char Buffer[255];
		GetCurrentDirectory(255,Buffer);
		std::string SearchFilePath = FilePath + "*.*";

		HANDLE hFind = FindFirstFile(SearchFilePath.c_str(), &FindFileData);

		std::string Path;
		std::string strTmp;
		std::string cur_Tmp;

		if (hFind == INVALID_HANDLE_VALUE)   // 에러 처리
		{
			return;
		} 

		// 해당 폴더의 모든파일에 대해 검사
		while( true )
		{
			if(! (	!(strcmp(FindFileData.cFileName, "."))		|| 
				!(strcmp(FindFileData.cFileName,".."))		||
				!(strcmp(FindFileData.cFileName,".svn")) 
				) )
			{
				//--현재 파일의 전체 경로를 얻음--
				Path = FilePath + FindFileData.cFileName;

				//------------------------------------
				// 해당 파일이 디렉토리이면,
				if( (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) != 0 )
				{
					Path += "/";
					// 재귀함수 호출(해당 폴더 안으로 들어가서 검색)
					FindAllFile(Path, FileNameTable,DuplicateFiles);
				}
				else// 디렉토리가 아니고 파일이면,
				{
					std::string lowercase = FindFileData.cFileName;
					transform( lowercase.begin(), lowercase.end(), lowercase.begin(), tolower);
					FILE2PATH::iterator iter = FileNameTable.find( lowercase );
					if( FileNameTable.end() != iter )
					{//파일이름이 이미 있다면
						DuplicateFiles.append(iter->second +" __ "+Path+"\r\n");
					}
					else
					{
						FileNameTable.insert(pair<std::string,std::string>(lowercase,Path));
					}

				}
			}

			if( FindNextFile( hFind, &FindFileData ) == FALSE )	// 다음 파일을 검색
			{
				DWORD dwErr = GetLastError();
				if( dwErr != ERROR_NO_MORE_FILES )
				{
					// 더이상 파일이 없는 이유가 아닌 다른 이유로 FindNextFile이 실패한 경우,
					// TO DO : Implement as you want_
				}
				break;
			}
		}
	}
};



bool MyApplication::Create()
{
	

	REngine::GetConfig().m_nWidth	= 1600;
	REngine::GetConfig().m_nHeight	= 1024;

	return RD3DApplication::Create(); 

	nCtrlMode = 0;
	nActorCnt = 0;
	nEffectCnt = 0;
	nLightCnt = 0;
	nShadowMode = 0;
	bSort = true;
}

bool MyApplication::OnCreate()
{
	m_pFont		= new RFont;
	m_pTextNode = new RTextSceneNode;
	m_pCamera	= new RCameraSceneNode;
	
	ResetCursor();
	// font
	m_pFont->Create(m_pDevice,L"굴림",12);
	// text 테스트 노드
	m_pTextNode->SetFont(m_pFont);
	m_pTextNode->SetText("test");
	m_pTextNode->SetPosition(0,0);
	GetSceneManager()->AddSceneNode(m_pTextNode);

	// 카메라
	GetSceneManager()->AddSceneNode(m_pCamera);
	GetSceneManager()->SetPrimaryCamera(m_pCamera);

	//m_pDevice->SetLighting(false);
	//m_pDevice->SetDepthBias(0);

	DragAcceptFiles(GetWindowHandle(), TRUE);
	bool bBackgroundLoad = false;
	if (bBackgroundLoad)
		REngine::GetBackgroundWorker();

	CreateScene();

	return true;
}


void MyApplication::CreateScene()
{
	//const char *ZONE_FILE_NAME = "./Data/Zone/BirdTown/BirdTown.zone.xml";
	//const char *ZONE_ENV_FILE_NAME = "./Data/Zone/BirdTown/BirdTown.zone.env.xml";

	//const char *ZONE_FILE_NAME = "./Data/Zone/Ingen_Freeport/Ingen_Freeport.zone.xml";
	//const char *ZONE_ENV_FILE_NAME = "./Data/Zone/Ingen_Freeport/Ingen_Freeport.zone.env.xml";

	//const char *ZONE_FILE_NAME = "./Data/Zone/Ritz/Ritz.zone.xml";
	//const char *ZONE_ENV_FILE_NAME = "./Data/Zone/Ritz/Ritz.zone.env.xml";

 	//const char *ZONE_FILE_NAME = "./Data/Zone/Ritz_Sewer/Ritz_Sewer.zone.xml";
 	//const char *ZONE_ENV_FILE_NAME = "./Data/Zone/Ritz_Sewer/Ritz_Sewer.zone.env.xml";

	//const char *ZONE_FILE_NAME = "./Data/Zone/Hero's_Lane/Hero's_Lane.zone.xml";
	//const char *ZONE_ENV_FILE_NAME = "./Data/Zone/Hero's_Lane/Hero's_Lane.zone.env.xml";

	//const char *ZONE_FILE_NAME = "./Data/Zone/Teress_Plain/Teress_Plain.zone.xml";
	//const char *ZONE_ENV_FILE_NAME = "./Data/Zone/Teress_Plain/Teress_Plain.zone.env.xml";

	const char *ZONE_FILE_NAME = "./Data/Zone/Catacomb_of_damned/Catacomb_of_Damned.zone.xml";
	const char *ZONE_ENV_FILE_NAME = "./Data/Zone/Catacomb_of_damned/Catacomb_of_Damned.zone.env.xml";

	//const char *ZONE_FILE_NAME = "./Data/Zone/The_Broken_Mast/The_Broken_Mast.zone.xml";
	//const char *ZONE_ENV_FILE_NAME = "./Data/Zone/The_Broken_Mast/The_Broken_Mast.zone.env.xml";

	// 존 로딩
	bool bRet = REngine::GetSceneManager().LoadWorldSceneNodeFromRootXMLFile( REngine::GetSceneManager().GetCurrentRootSceneNode(true),ZONE_FILE_NAME);
	_ASSERT(bRet);

	// 환경 씬노드 생성
	m_pEnvironmentSet = new rs3::REnvironmentSet;
	m_pEnvironmentSet->CreateFromFile(ZONE_ENV_FILE_NAME, false);
	REngine::GetSceneManager().AddSceneNode(m_pEnvironmentSet);
	m_pEnvironmentSet->SetCurrentEnvSet(REnvironment::DAYTIME, REnvironment::SUNNY);

	// 카메라 셋팅
	RCameraSceneNode* pCamera = REngine::GetSceneManager().GetPrimaryCamera();
	_ASSERT(m_pCamera);
	m_pCamera->SetNearFarZ(10, 50000);

	// Test_map
	//m_pCamera->SetPosition(RVector3(5816, 4, 1082));
	//m_pCamera->SetDirection(RVector3( -0.954f, 0, -0.297f));

	// Login
	//m_pCamera->SetPosition(RVector3( 3759, -9683, 600));
	//m_pCamera->SetDirection(RVector3( 0.824f, 0.537f, -0.177f));

	// Ingen
	//m_pCamera->SetPosition(RVector3( 46991, 49356, 5432));
	//m_pCamera->SetDirection(RVector3( 0.427f, 0.777f, -0.462f));

	// Tree instancing test
	//m_pCamera->SetPosition(RVector3( 124, -2495, 330));
	//m_pCamera->SetDirection(RVector3( -0.084f, 0.976f, 0.197f));

	// Teress hole
	//m_pCamera->SetPosition(RVector3( 41279, 39026, 2587));
	//m_pCamera->SetDirection(RVector3( 0.241f, 0.901f, -0.360f));

	// ingen instanging
	//m_pCamera->SetPosition(RVector3( 56808, 48766, 3517));
	//m_pCamera->SetDirection(RVector3( 0.466f, -0.390f, -0.793f));

	// Ritz
	//m_pCamera->SetPosition(RVector3( 0, -9595, 4530));
	//m_pCamera->SetDirection(RVector3( 0, 0.88f, -0.45f));

	// Teress
	//m_pCamera->SetPosition(RVector3( 52267, 61036, 1205));
	//m_pCamera->SetDirection(RVector3( 0.494f, -0.8322f, -0.250f));
	// Teress
	//m_pCamera->SetPosition(RVector3( 42108, 42946, 674));
	//m_pCamera->SetDirection(RVector3( 0.0f, 0.9f, -0.3f));

	//Catacomb_of_damned
	m_pCamera->SetPosition(RVector3(-354.f,-17668.f,5137.f));
	m_pCamera->SetDirection(RVector3(-0.1f,0.8f,-0.6f));


	m_cameraPos = m_pCamera->GetPosition();
	m_cameraDir = m_pCamera->GetDirection();

	//((REffectPassRenderer*)(REngine::GetSceneManager().GetPassRenderer( RRP_PARTICLE)))->m_bSort = bSort;


	//m_pCamera->SetFogEnable(false);

 	//REngine::GetSceneManager().SetAccumMotionBlurEnable(true);
 	//REngine::GetSceneManager().SetAccumMotionBlurActive(true);
 
 	//REngine::GetSceneManager().SetHDREnable(true);

	//REngine::GetSceneManager().SetDistortionEnable( false);

	//REngine::GetSceneManager().SetPixelMotionBlurEnable( true);
	//REngine::GetSceneManager().SetPixelMotionBlurActive( true);

	//REngine::GetSceneManager().SetPEEdgeEnable(true);
	//REngine::GetSceneManager().SetPEQuality(2,2);
	//REngine::GetSceneManager().SetPEFactorTickNess( 3.6f, 1.6f, 1.75f);
	

}

void MyApplication::OnDestroy()
{
	// 환경 씬노드 소멸
	if(m_pEnvironmentSet)
	{
		m_pEnvironmentSet->RemoveFromParent();
		delete m_pEnvironmentSet;
	}


	for(int nIdx=0; nIdx<(int)m_vecActor.size();nIdx++)
	{
		RActor* pActors = m_vecActor[nIdx];
		pActors->RemoveFromParent();
		SAFE_DELETE( pActors);
	}
	m_vecActor.clear();
	nActorCnt = m_vecActor.size();

	for(int nIdx=0; nIdx<(int)m_vecEffect.size();nIdx++)
	{
		REffectSceneNode* pEffect = m_vecEffect[nIdx];
		pEffect->RemoveFromParent();
		SAFE_DELETE( pEffect);
	}
	m_vecEffect.clear();
	nEffectCnt = m_vecEffect.size();

	for(int nIdx=0; nIdx<(int)m_vecLight.size();nIdx++)
	{
		RLightSceneNode* pLight = m_vecLight[nIdx];
		pLight->RemoveFromParent();
		SAFE_DELETE( pLight);
	}
	m_vecLight.clear();
	nLightCnt = m_vecLight.size();


}


void MyApplication::AddActor()
{
	int nCnt = m_vecActor.size();
	int x = nCnt % ACTOR_MAX_ROW;
	int y = nCnt / ACTOR_MAX_ROW;

	RActor* pActors = new RActor();
	REngine::GetSceneManager().AddSceneNode( pActors);


//  	switch( nCnt % 1)
//  	{
//  	case 0:
//  		pActors->Create("skeleton_armor_01.elu");
//  		pActors->SetAnimation("idle"); 
// 		//pActors->SetAO(true);
// 		//pActors->SetVertexMotionBlur(true);
// 		//pActors->SetPixelMotionBlur( PMB_FULL);
// 		//pActors->SetNoiseRef(0.5f);
// 		//pActors->SetVertexMotionBlurBasisFrame( nCnt);
//  		break;
//  	case 1:
//  		pActors->Create("ATD11.elu");
// 		pActors->SetAnimation("ATD11"); 
// 		//pActors->SetPixelMotionBlur( PMB_FULL);
// 		//pActors->SetVertexMotionBlurBasisFrame( 60);
// 		pActors->SetAO(true);
// 		//pActors->SetVertexMotionBlurBasisFrame( nCnt);
// 		//pActors->SetNoiseRef(0.5f);
//  		break;
// 	case 2:
// 		pActors->Create("test.elu");
// 		pActors->SetAnimation("test"); 
// 		pActors->SetPixelMotionBlur( PMB_FULL);
// 		//pActors->SetNoiseRef(0.5f);
// 		//pActors->SetVertexMotionBlurBasisFrame( nCnt);
// 		break;
// 	case 3:
// 		pActors->Create("test.elu");
// 		pActors->SetAnimation("test"); 
// 		pActors->SetPixelMotionBlur( PMB_FULL);
// 		pActors->SetVertexMotionBlurBasisFrame( 60);
// 		//pActors->SetNoiseRef(0.5f);
// 		//pActors->SetVertexMotionBlurBasisFrame( nCnt);
// 		break;
//  	case 4:
//  		pActors->Create("valkyrie.elu");
//  		pActors->SetAnimation("idle");
// 		//pActors->Create("test.elu");
// 		//pActors->SetAnimation("test"); 
// 		//pActors->SetPixelMotionBlur( PMB_FULL);
//  		break;
// 	case 5:
// 		pActors->Create("article_smithytool_01.elu");
// 		pActors->SetAnimation("mesh_fire_test");
// 		//pActors->Create("Broken-Mast_prob02.elu");
// 		//pActors->SetAnimation("Broken-Mast_prob02"); 
// 		//pActors->SetPixelMotionBlur( PMB_FULL);
// 		pActors->SetAO(false);
// 		break;
// 	}	
	
 
	if( nCnt < 20)
	{
		pActors->Create("skeleton_armor_01.elu");
		pActors->SetAnimation("idle"); 
	}
	else
	{
		pActors->Create("hf.elu");
		pActors->SetAnimation("hf_1hp_idle");
		
		RMesh* pMesh;

		pMesh = REngine::GetSceneManager().CreateResource< RMesh >( FACE_MESH[rand()%3] );
		if( pMesh->BuildAll() )
		{
			RMeshNode* pMeshNode = pMesh->GetNode( "face" );
			if( pMeshNode )
			{
				pActors->AddMeshNode( pMeshNode, NULL );
				if( pActors->GetActorNode(	"face"))
					pActors->GetActorNode(	"face")->SetPartsColor( D3DCOLOR_ARGB( 255,255,255,0));
			}
		}
		else
		{
			REngine::GetSceneManager().ReleaseResource( pMesh );
		}
	 
		pMesh = REngine::GetSceneManager().CreateResource< RMesh >( HAIR_MESH[rand()%3] );
		if( pMesh->BuildAll() )
		{
			RMeshNode* pMeshNode = pMesh->GetNode( "hair" );
			if( pMeshNode )
			{
				pActors->AddMeshNode( pMeshNode, NULL );
				pActors->AddMeshNode( pMeshNode, NULL );
				if( pActors->GetActorNode(	"hair"))
					pActors->GetActorNode(	"hair")->SetPartsColor( D3DCOLOR_ARGB( 255,0,255,0));
			}
		}
		else
		{
		REngine::GetSceneManager().ReleaseResource( pMesh );
		}

		pMesh = REngine::GetSceneManager().CreateResource< RMesh >( SHIELD_MESH[rand()%3] );
		if( pMesh->BuildAll() )
		{
			RMeshNode* pMeshNode = pMesh->GetNode( "shield_03" );
			if( pMeshNode )
			{
				pActors->AddMeshNode( pMeshNode, pActors->GetActorNode( "dummy_shield" ) );
			}
		}
		else
		{
			REngine::GetSceneManager().ReleaseResource( pMesh );
		}

		pMesh = REngine::GetSceneManager().CreateResource< RMesh >( ARMOR_MESH[rand()%3] );
		if( pMesh->BuildAll() )
		{
			RMeshNode* pMeshNode = pMesh->GetNode( "chest_item" );
			if( pMeshNode )
			{
				pActors->AddMeshNode( pMeshNode, pActors->GetActorNode( "dummy_shest" ) );
			}
		}
		else
		{
			REngine::GetSceneManager().ReleaseResource( pMesh );
		}
	}
	/////////////////////////////////////////////////////////////////////////

	pActors->SetUseDoubleShadow( true);

	RVector3 actorPos( DEFAULT_X - ACTOR_GAP * x, DEFAULT_Y + ACTOR_GAP * y, 0);
	actorPos = m_pCamera->GetWorldPosition();
// 	RPICKINFO pickInfo;
// 	pickInfo.inPickOrigin = actorPos + RVector(0,0,300000);
// 	pickInfo.inPickTo = actorPos + RVector(0,0,-30000);
// 	pickInfo.outNormal = RVector( 0,0,1);
// 	REngine::GetSceneManager().PickSceneNode( pickInfo);
// 	actorPos.z = pickInfo.outPosition.z;
	//actorPos.z = DEFAULT_Z;
	pActors->SetPosition( actorPos);
	
	//pActors->SetPosition( actorPos);
	RSceneNodeUpdateVisitor updateVisitor;
	pActors->Traverse( &updateVisitor );
	// 패스 랜더러 사용여부
	pActors->SetUsingPassRenderer(true);

	
	pActors->Play();
	//pActors->Pause();
	//pActors->ForceAnimationControlLoop(true);

// 	if( nCnt < 4)
// 	{
// 		RVector3 vClr( 1,0,0);
// 		pActors->GetShaderEffectController()->EnableGlowEffect( vClr);
// 	} else
// 	if( nCnt < 8)
// 	{
// 		RVector3 vClr( 1,0,0);
// 		pActors->GetShaderEffectController()->EnableAddColorEffect( vClr);
// 	}
// 	else
// 	if( nCnt < 12)
// 	{
// 		pActors->SetDistortion( true);
// 	}

	pActors->EnablePEEdge( RVector3( 1.f,1.f,0.2f));
	pActors->SetEdgeColor( RVector3( 1.f,1.f,0.2f));

	m_vecActor.push_back( pActors);
	nActorCnt = m_vecActor.size();
}


void MyApplication::RemoveActor()
{
	int nIdx = m_vecActor.size() - 1;
	
	if( nIdx >= 0)
	{
		RActor* pActors = m_vecActor[nIdx];

		pActors->RemoveFromParent();
		std::vector<RActor*>::iterator it = m_vecActor.end();
		it--;
		m_vecActor.erase( it);
		SAFE_DELETE( pActors);
	}
	nActorCnt = m_vecActor.size();

}


void MyApplication::AddEffect()
{
	int nCnt = m_vecEffect.size();
	int x = nCnt % ACTOR_MAX_ROW;
	int y = nCnt / ACTOR_MAX_ROW;

	// 이팩트
	REffectInstanceSceneNode effectInstance;
	bool bRet = effectInstance.Create( "ef_bastero_FireMagic");
	_ASSERT(bRet);

	REffectSceneNode* pEffect = effectInstance.GetEffectSceneNode();
	pEffect->RemoveFromParent();

	// 이팩트 오프셋
	RVector3 EffectPos( DEFAULT_X - ACTOR_GAP * x, DEFAULT_Y + ACTOR_GAP * y, 0);
	RPICKINFO pickInfo;
	pickInfo.inPickOrigin = EffectPos + RVector(0,0,300000);
	pickInfo.inPickTo = EffectPos + RVector(0,0,-30000);
	pickInfo.outNormal = RVector( 0,0,1);
	REngine::GetSceneManager().PickSceneNode( pickInfo);
	EffectPos.z = pickInfo.outPosition.z + 25;

	EffectPos = m_pCamera->GetWorldPosition();

	RMatrix matTransOffset;
	matTransOffset.MakeIdentity();
	matTransOffset.SetTranslation( EffectPos );

	// 신에 더해준다
	REngine::GetSceneManager().AttachEffect( pEffect, NULL, NULL, matTransOffset);

	m_vecEffect.push_back( pEffect);
	nEffectCnt = m_vecEffect.size();
}

void MyApplication::AddEffect( const RVector& pos )
{
	// 이팩트
	REffectInstanceSceneNode effectInstance;
	bool bRet = effectInstance.Create( "00_sec");
	_ASSERT(bRet);

	REffectSceneNode* pEffect = effectInstance.GetEffectSceneNode();
	pEffect->RemoveFromParent();

	RMatrix matTransOffset;
	matTransOffset.MakeIdentity();
	matTransOffset.SetTranslation( pos );

	// 신에 더해준다
	REngine::GetSceneManager().AttachEffect( pEffect, NULL, NULL, matTransOffset);

	m_vecEffect.push_back( pEffect);
	nEffectCnt = m_vecEffect.size();
}

void MyApplication::RemoveEffect()
{
	int nIdx = m_vecEffect.size() - 1;

	if( nIdx >= 0)
	{
		REffectSceneNode* pEffect = m_vecEffect[nIdx];

		pEffect->RemoveFromParent();
		std::vector<REffectSceneNode*>::iterator it = m_vecEffect.end();
		it--;
		m_vecEffect.erase( it);
		SAFE_DELETE( pEffect);
	}
	nEffectCnt = m_vecEffect.size();

}



void MyApplication::AddLight()
{
	int nCnt = m_vecLight.size();
	int x = nCnt % ACTOR_MAX_ROW;
	int y = nCnt / ACTOR_MAX_ROW;

	// 이팩트 오프셋
	RVector3 LightPos( DEFAULT_X - ACTOR_GAP * x, DEFAULT_Y + ACTOR_GAP * y, 0);
	RPICKINFO pickInfo;
	pickInfo.inPickOrigin = LightPos + RVector(0,0,300000);
	pickInfo.inPickTo = LightPos + RVector(0,0,-30000);
	pickInfo.outNormal = RVector( 0,0,1);
	REngine::GetSceneManager().PickSceneNode( pickInfo);
	LightPos.z = pickInfo.outPosition.z + 50;
	LightPos.y = pickInfo.outPosition.y - 20;

	LightPos = m_pCamera->GetPosition();

	// 디렉셔널 라이트 생성
	RPointLightSceneNode* pLight = new RPointLightSceneNode;
	pLight->SetRadius( (rand() % 4 + 1) * 100);
	pLight->SetPosition( LightPos);
	float fR = (float)(rand() % 100) / 100.0f;
	float fG = (float)(rand() % 100) / 100.0f;
	float fB = (float)(rand() % 100) / 100.0f;
	pLight->SetColorVector( RVector( fR, fG, fB));

	
	// 신에 더해준다
	pLight->UpdateTransform(); //http://dev:8181/projects/engine/ticket/1236  땜빵으로 고친거 <-- 삭제되어야 하는 코드
	REngine::GetSceneManager().AddSceneNode( pLight);

	//pLight->SetPosition( LightPos);

	m_vecLight.push_back( pLight);
	nLightCnt = m_vecLight.size();
}


void MyApplication::RemoveLight()
{
	int nIdx = m_vecLight.size() - 1;

	if( nIdx >= 0)
	{
		RLightSceneNode* pLight = m_vecLight[nIdx];

		pLight->RemoveFromParent();
		std::vector<RLightSceneNode*>::iterator it = m_vecLight.end();
		it--;
		m_vecLight.erase( it);
		SAFE_DELETE( pLight);
	}
	nLightCnt = m_vecLight.size();

}



void MyApplication::ToggleHDREnable()
{
	REngine::GetSceneManager().SetHDREnable( !REngine::GetSceneManager().IsEnableHDR());
}

void MyApplication::ToggleDOFEnable()
{
	REngine::GetSceneManager().SetDOFEnable( !REngine::GetSceneManager().IsEnableDOF());
}

void MyApplication::ToggleSSAOEnable()
{
	REngine::GetSceneManager().SetSSAO( !REngine::GetSceneManager().GetSSAO());
}

void MyApplication::ToggleShadow()
{
	nShadowMode++;
	if( nShadowMode > 4)
		nShadowMode = 0;

	RShadowDesc desc;

	if( nShadowMode <= 0)
	{
		desc.eMethod = SHADOW_NONE;
	}
	else
	{
		desc.nTextureNum = nShadowMode;
	}
	
	REngine::GetSceneManager().SetRealtimeShadowMethod(desc);
}


POINT g_Pt;
static bool g_bRbuttonDown = false;
static bool g_bLbuttonDown = false;
static bool g_bMbuttonDown = false;
bool MyApplication::OnWndMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT *pReturn)
{
	switch (message)
	{
	case WM_CHAR:
		{
			if(wParam==VK_ESCAPE)
				PostMessage(hWnd,WM_CLOSE,0,0);
		}break;

		

	case WM_KEYDOWN:
		if( wParam==VK_ADD)
		{
			//AddActor( );
			switch( nCtrlMode){
				case NODE_ADD_ACTOR: AddActor(); break;
				case NODE_ADD_EFFECT: AddEffect(); break;
				case NODE_ADD_LIGHT: AddLight(); break;
			}
		}
		else if( wParam==VK_SUBTRACT)
		{
			switch( nCtrlMode){
				case NODE_ADD_ACTOR: RemoveActor(); break;
				case NODE_ADD_EFFECT: RemoveEffect(); break;
				case NODE_ADD_LIGHT: RemoveLight(); break;
			}
		}
		else if( wParam==VK_MULTIPLY)
		{
			nCtrlMode++;
			if( nCtrlMode >= NODE_ADD_CNT)
				nCtrlMode = 0;
		}
		else if( wParam==VK_LEFT)
		{
			ToggleSSAOEnable();
		}
		else if( wParam==VK_DOWN)
		{
			ToggleHDREnable();
		}
		else if( wParam==VK_RIGHT)
		{
			ToggleDOFEnable();
		}
		else if( wParam==VK_UP)
		{
			ToggleShadow();
		}
		else if ( wParam==VK_DIVIDE)
		{
			m_pCamera->SetFogEnable( !m_pCamera->GetFogEnable());
			///((REffectPassRenderer*)(REngine::GetSceneManager().GetPassRenderer( RRP_PARTICLE)))->m_bSort = !((REffectPassRenderer*)(REngine::GetSceneManager().GetPassRenderer( RRP_PARTICLE)))->m_bSort;
		}
// 		else if ( wParam==VK_DELETE)
// 		{
// 			REngine::GetSceneManager().UseNormalMap( !REngine::GetSceneManager().IsUsingNormalMap());
// 		}
// 		else if ( wParam==VK_END)
// 		{
// 			REngine::m_bUseSkinning = !REngine::m_bUseSkinning;
// 		}
		else if ( wParam==VK_HOME)
		{
			RTerrain::SetDetailDistance( min( 20000, RTerrain::GetDetailDistance()+2500));
			RTreeSceneNode::SetLodFarDist( RTerrain::GetDetailDistance());
			if( REngine::GetSceneManager().GetCurrentTerrain())
				REngine::GetSceneManager().GetCurrentTerrain()->SetGrassLodParams( RTerrain::GetDetailDistance(), RTerrain::GetDetailDistance());
			//((RActorPassRenderer*)(REngine::GetSceneManager().GetPassRenderer( RRP_ACTOR)))->m_bUseInstancing = !((RActorPassRenderer*)(REngine::GetSceneManager().GetPassRenderer( RRP_ACTOR)))->m_bUseInstancing;
			//((RTreePassRenderer*)(REngine::GetSceneManager().GetPassRenderer( RRP_SPEEDTREE)))->m_bUseInstancing = ((RActorPassRenderer*)(REngine::GetSceneManager().GetPassRenderer( RRP_ACTOR)))->m_bUseInstancing;
		}
		else if ( wParam==VK_INSERT)
		{
			RTerrain::SetDetailDistance( max( 0, RTerrain::GetDetailDistance()-2500));
			RTreeSceneNode::SetLodFarDist( RTerrain::GetDetailDistance());
			if( REngine::GetSceneManager().GetCurrentTerrain())
				REngine::GetSceneManager().GetCurrentTerrain()->SetGrassLodParams( RTerrain::GetDetailDistance(), RTerrain::GetDetailDistance());
			//REngine::GetSceneManager().GetShaderCompositeManager()->SetDepthBiasBlending( !REngine::GetSceneManager().GetShaderCompositeManager()->GetDepthBiasBlending());	
		}
		else if ( wParam==VK_PAUSE)
		{
			if( REngine::GetTimer().IsStopped())
				REngine::GetTimer().Start();
			else
				REngine::GetTimer().Stop();
		}
		else if ( wParam==VK_END)
		{
			//REngine::GetSceneManager().SetRealtimeRenderingMethod( RENDERING_INFERRED);
			//m_pCamera->SetFogEnable( !m_pCamera->GetFogEnable());

			//REngine::GetSceneManager().SetPixelMotionBlurActive( true);
			//m_pCamera->SetFogEnable( !m_pCamera->GetFogEnable());
			//REngine::GetSceneManager().GetRenderingStrategy()->m_bDebug = false;
		}
		else if ( wParam==VK_DELETE) 
		{
			//REngine::GetSceneManager().SetRealtimeRenderingMethod( RENDERING_DEFERRED);
			//switch( REngine::GetSceneManager().GetRenderingStrategy()->GetRenderingMode())
			//{
			//case RRM_DIFFUSE_ONLY:
			//	REngine::GetSceneManager().GetRenderingStrategy()->SetRenderingMode(RRM_NO_SPECULAR_RIM_LIGHT);
			//	break;
			//case RRM_NO_SPECULAR_RIM_LIGHT:
			//	REngine::GetSceneManager().GetRenderingStrategy()->SetRenderingMode(RRM_FULL_LIGHT);
			//	break;
			//case RRM_FULL_LIGHT:
			//	REngine::GetSceneManager().GetRenderingStrategy()->SetRenderingMode(RRM_DIFFUSE_ONLY);
			//	break;
			//}
			//REngine::GetSceneManager().SetPixelMotionBlurActive( false);

			//REngine::GetSceneManager().GetRenderingStrategy()->m_bDebug = true;
		}

		

		break;

	case WM_MOUSEWHEEL:
		{
			// 값의 + 가 휠이 위로, -가 휠이 아래로 내려가는걸 나타냅니다.
			if((INT)wParam < 0) 
				m_fDistToCamera += g_fMoveSpeed;
			else 
				m_fDistToCamera -= g_fMoveSpeed;
		}
		break;

	case WM_MBUTTONDOWN:
		{
			g_bMbuttonDown = true;
			POINT pt;
			GetCursorPos(&pt);
			ScreenToClient(GetWindowHandle(),&pt);
			g_Pt = pt;
		}
		break;

	case WM_MBUTTONUP:
		g_bMbuttonDown = false;
		break;

	case WM_RBUTTONDOWN:
		g_bRbuttonDown = true;
		break;

	case WM_RBUTTONUP:
		g_bRbuttonDown = false;
		break;

	case WM_LBUTTONDOWN:
		{
			g_bLbuttonDown = true;
			POINT pt;
			GetCursorPos(&pt);
			ScreenToClient(GetWindowHandle(),&pt);
			g_Pt = pt;
		}
		break;

	case WM_LBUTTONUP:
		g_bLbuttonDown = false;
		break;
	};

	return false;
}

void MyApplication::OnUpdate()
{
	PFC_THISFUNC;
	
	DWORD dwElapsedFactor = 1;
	if( IsKeyDown(VK_NUMPAD1) )
		dwElapsedFactor = 2;
	else if( IsKeyDown(VK_NUMPAD2) )
		dwElapsedFactor = 3;
	else if( IsKeyDown(VK_NUMPAD3) )
		dwElapsedFactor = 7;
	else if( IsKeyDown(VK_NUMPAD4) )
		dwElapsedFactor = 10;
	else if( IsKeyDown(VK_NUMPAD5) )
		dwElapsedFactor = 15;
	else if( IsKeyDown(VK_NUMPAD6) )
		dwElapsedFactor = 20;
	else if( IsKeyDown(VK_NUMPAD7) )
		dwElapsedFactor = 40;
	else if( IsKeyDown(VK_NUMPAD8) )
		dwElapsedFactor = 60;
 
	//REngine::GetTimer().Stop();
		
#define IsKeyDown(key) ((GetAsyncKeyState(key) & 0x8000)!=0)

	DWORD elapsed = GetDevice()->GetLastElapsedTime() * (IsKeyDown(VK_SHIFT) ? 10 : 1);

	//if( !REngine::GetTimer().IsStopped())
	//	REngine::GetTimer().UpdateCustom( elapsed * dwElapsedFactor);

	float fRotateStep = elapsed*0.001f;
	float fMoveStep = (float)elapsed;

	static float fElapAccum = 0;
	fElapAccum += fRotateStep;

	static float fff = 0;
	fff -= (float)(REngine::GetTimer().GetElapsedTime()) * 0.001f;
	if( fff <= 0)
		fff = 1;
	if( m_vecActor.size() > 1)
	{
//		m_vecActor[0]->SetNoiseRef( fff);
	}

	if( m_vecActor.size() > 2)
	{
//		m_vecActor[1]->SetMultiplyColor( RVector4( fff, 0,0,1));
	}



	if( m_pEnvironmentSet)
		m_pEnvironmentSet->Update( NULL);

	PFC_B("MOVEACTOR");

	// 액터 움직이게...
	if( g_bRbuttonDown)
	{
// 		for(int nIdx=0; nIdx<(int)m_vecActor.size();nIdx++)
// 		{
// 			RActor* pActor = m_vecActor[nIdx];
// 			pActor->SetAccumMotionBlur( true);
// 			pActor->SetVertexMotionBlurBasisFrame(10);
// 			int x = nIdx % ACTOR_MAX_ROW;
// 			int y = nIdx / ACTOR_MAX_ROW;
// 			//RVector3 actorPos( DEFAULT_X + ACTOR_GAP * x, DEFAULT_Y + ACTOR_GAP * y, 0);
// 			RVector3 actorPos = pActor->GetWorldPosition();
// 			RPICKINFO pickInfo;
// 			pickInfo.inPickOrigin = actorPos + RVector(0,0,300000);
// 			pickInfo.inPickTo = actorPos + RVector(0,0,-30000);
// 			pickInfo.outNormal = RVector( 0,0,1);
// 			//REngine::GetSceneManager().PickSceneNode( pickInfo);
// 			//actorPos.z = pickInfo.outPosition.z;
// 			//actorPos.z = DEFAULT_Z;
// 			//pActor->SetPosition( actorPos);
// 			pActor->SetPosition( actorPos + RVector3( sin(fElapAccum) * 100, cos(fElapAccum) * 100, 0));
// 		}
//		REngine::GetSceneManager().GetRenderingStrategy()->m_bDebug = true;
	}
 	else
 	{
// 		for(int nIdx=0; nIdx<(int)m_vecActor.size();nIdx++)
// 		{
// 			RActor* pActor = m_vecActor[nIdx];
// 			pActor->SetAccumMotionBlur( false);
// 			int x = nIdx % ACTOR_MAX_ROW;
// 			int y = nIdx / ACTOR_MAX_ROW;
// 			RVector3 actorPos( DEFAULT_X + ACTOR_GAP * x, DEFAULT_Y + ACTOR_GAP * y, 0);
// 			RPICKINFO pickInfo;
// 			pickInfo.inPickOrigin = actorPos + RVector(0,0,300000);
// 			pickInfo.inPickTo = actorPos + RVector(0,0,-30000);
// 			pickInfo.outNormal = RVector( 0,0,1);
// 			//REngine::GetSceneManager().PickSceneNode( pickInfo);
// 			//actorPos.z = pickInfo.outPosition.z;
// 			actorPos.z = DEFAULT_Z;
// 			pActor->SetPosition( actorPos);
// 		}
//		REngine::GetSceneManager().GetRenderingStrategy()->m_bDebug = false;
 	}
	PFC_E;


	PFC_B("LDN");
	if(IsActive() && g_bLbuttonDown)
	{
		POINT pt;
		GetCursorPos(&pt);
		ScreenToClient(GetWindowHandle(),&pt);
		// mouse control
		int iDeltaX, iDeltaY;
		iDeltaX = pt.x-GetDevice()->GetScreenWidth()/2;
		iDeltaY = pt.y-GetDevice()->GetScreenHeight()/2;

		float RotateAxisZ = -iDeltaX*0.01f;
		float RotateAxisRight = -iDeltaY*0.01f;

		// update direction
		RMatrix mat;
		mat.MakeIdentity();
		mat.SetRotationZ(RotateAxisZ);

		m_cameraDir =  m_cameraDir * mat;

		RVector vRight = m_pCamera->GetRight();

		mat.MakeIdentity();
		mat.SetRotationMatrixAxis(vRight, RotateAxisRight);

		m_cameraDir = m_cameraDir * mat;

		ResetCursor();
	}
	PFC_E;

	PFC_B("WTF");
	RVector posDiff(0,0,0);
	if(IsActive()){
		RVector dir = m_cameraDir;
		RVector right = RVector(0,0,1).CrossProduct(dir);
		right.Normalize();

		if(IsKeyDown('W')) posDiff+=fMoveStep*dir;
		if(IsKeyDown('S')) posDiff-=fMoveStep*dir;
		if(IsKeyDown('A')) posDiff+=fMoveStep*right;
		if(IsKeyDown('D')) posDiff-=fMoveStep*right;
		if(IsKeyDown(VK_SPACE)) posDiff+=fMoveStep*RVector(0,0,1);
	} // if(IsActive())

	if(posDiff.Length()>0) 
	{
		m_cameraPos += posDiff;
	}

	m_pCamera->Set(m_cameraPos, m_cameraDir);

 	RCameraSceneNode* pHDRCamera = (RCameraSceneNode*)m_pCamera;
 	static bool bValue[9] = { false,false,false,false,false,false,false,false,false};
// 	//////////////////////////////////////////////////////////////////////////
// 	if(IsActive())
// 	{
// 		//if (e.nKey == 'Z')
// 		//{
// 		//	m_pNPCMonitor->SaveLog();
// 		//	m_pNPCLog->SaveLog();
// 		//	return true;
// 		//}
// 
// 		//////////////////////////////////////////////////////////////////////////
// 		if( IsKeyDown(VK_NUMPAD1) )
// 		{
// 			REngine::GetSceneManagerPtr()->GetPrimaryCamera()->SetPassIgnore(RRP_SKYBOX, bValue[0]=!bValue[0]); Sleep(500);
// 		}
// 		else if( IsKeyDown(VK_NUMPAD2) )
// 		{
// 			REngine::GetSceneManagerPtr()->GetPrimaryCamera()->SetPassIgnore(RRP_SPEEDTREE, bValue[1]=!bValue[1]); Sleep(500);
// 		}
// 		else if( IsKeyDown(VK_NUMPAD3) )
// 		{
// 			REngine::GetSceneManagerPtr()->GetPrimaryCamera()->SetPassIgnore(RRP_ACTOR, bValue[2]=!bValue[2]); Sleep(500);
// 		}
// 		else if( IsKeyDown(VK_NUMPAD4) )
// 		{
// 			REngine::GetSceneManagerPtr()->GetPrimaryCamera()->SetPassIgnore(RRP_TERRAIN, bValue[3]=!bValue[3]); Sleep(500);
// 		}
// 		else if( IsKeyDown(VK_NUMPAD5) )
// 		{
// 			REngine::GetSceneManagerPtr()->GetPrimaryCamera()->SetPassIgnore(RRP_STATICMESH, bValue[4]=!bValue[4]); Sleep(500);
// 		}
// 		else if( IsKeyDown(VK_NUMPAD6) )
// 		{
// 			REngine::GetSceneManagerPtr()->GetPrimaryCamera()->SetPassIgnore(RRP_WATER, bValue[5]=!bValue[5]); Sleep(500);
// 		}
// 		else if( IsKeyDown(VK_NUMPAD7) )
// 		{
// 			REngine::GetSceneManagerPtr()->GetPrimaryCamera()->SetPassIgnore(RRP_PARTICLE, bValue[6]=!bValue[6]); Sleep(500);
// 		}
// 		else if( IsKeyDown(VK_NUMPAD8) )
// 		{
// 			REngine::GetSceneManagerPtr()->GetPrimaryCamera()->SetPassIgnore(RRP_SHADOWMAP, bValue[7]=!bValue[7]); Sleep(500);
// 		}
// 		else if( IsKeyDown(VK_NUMPAD9) )
// 		{
// 			static bool bIsIgnore = false;
// 			bIsIgnore = !bIsIgnore;
// 			if(bIsIgnore)
// 				REngine::GetSceneManagerPtr()->GetPrimaryCamera()->SetPassIgnore(RRP_SHADOWMAP, RRP_2D);
// 			else
// 				REngine::GetSceneManagerPtr()->GetPrimaryCamera()->RemoveAllIgnorePass();
// 
// 			 Sleep(500);
// 		}
// 
// 	} // if(IsActive())

	char szBuffer[512];
	char* pstrAddMode = NULL;
	char* szActor = "Actor";
	char* szEffect = "Effect";
	char* szLight = "PointL";
	char* szERR = "ERR!!!!";
	switch( nCtrlMode){
		case NODE_ADD_ACTOR: pstrAddMode = szActor;	break;
		case NODE_ADD_EFFECT: pstrAddMode = szEffect;	break;
		case NODE_ADD_LIGHT: pstrAddMode = szLight;	break;
		default: pstrAddMode = szERR;	break;
	}
	PFC_E;

// 	PFC_B("sprintf_s");
// 	sprintf_s(szBuffer, "TimeMode : %d\n	\
// 		FPS : %3.1f\n CamPos : %f %f %f\n CamDir : %f %f %f\n  \
// 		cnt(Key+-) === actor: %d  eff: %d  PLit : %d  \n add mode(Key*) : %s\n \
// 		SSAO(arrL):%d  HDR(arrD):%d  DOF(arrR):%d  Shadow(arrU):%d\n  \
// 		fog(/):%d   Normalmap(Del) : %d    skinning(End) : %d\n  \
// 		Instancing : %d    SoftParticle : %d  TerrDist(-Ins +Home) : %f \n	\
// 		Inferred :%d   Deferred :%d",
// 		!REngine::GetTimer().IsStopped(),
// 		GetDevice()->GetFrameRate(), 
// 		m_cameraPos.x, m_cameraPos.y, m_cameraPos.z, 
// 		m_cameraDir.x, m_cameraDir.y, m_cameraDir.z,
// 		nActorCnt, nEffectCnt, nLightCnt,
// 		pstrAddMode,
// 		REngine::GetSceneManager().IsUsingSSAO(), REngine::GetSceneManager().IsEnableHDR(), REngine::GetSceneManager().IsEnableDOF(), nShadowMode,
// 		m_pCamera->GetFogEnable(), REngine::GetSceneManager().IsUsingNormalMap(), REngine::m_bUseSkinning, 
// 		((RActorPassRenderer*)(REngine::GetSceneManager().GetPassRenderer( RRP_ACTOR)))->m_bUseInstancing, REngine::GetSceneManager().GetShaderCompositeManager()->GetDepthBiasBlending(), RTerrain::GetDetailDistance(),
// 		REngine::GetSceneManager().GetRealtimeRenderingMethod() == RENDERING_INFERRED, REngine::GetSceneManager().GetRealtimeRenderingMethod() == RENDERING_DEFERRED);
// 	PFC_E;
// 
// 

	sprintf_s( szBuffer, "CP:%f %f %f CD:%f %f %f DP:%d/%d ", 
		m_cameraPos.x, m_cameraPos.y, m_cameraPos.z, 
		m_cameraDir.x, m_cameraDir.y, m_cameraDir.z,
		REngine::GetResourceProfiler().GetDrawCallCount(PPT_ACTOR),
		REngine::GetResourceProfiler().GetTotalDrawCalls());

	PFC_B("SetText");
	SetText(szBuffer);
	PFC_E;

	if( IsKeyDown('N') )
		ActivateTestingLoader();
	if( IsKeyDown('M') )
		DeActivateTestingLoader();

	OnUpdateTestingLoader();
}

void MyApplication::OnRender()
{
	//RVt_pos arrTemp[4];
	//arrTemp[0].vPos = RVector3(0, 0, 1000);
	//arrTemp[1].vPos = RVector3(1000, 0, 1000);
	//arrTemp[2].vPos = RVector3(0, 0, 0);
	//arrTemp[3].vPos = RVector3(1000, 0, 0);

	//REngine::GetDevice().SetCullMode(RCULL_NONE);
	//REngine::GetDevice().SetFvF(RVt_pos::FVF);
	//REngine::GetDevice().DrawPrimitiveUP(RPT_TRIANGLESTRIP, 2, arrTemp, 0);
}

void MyApplication::ActivateTestingLoader()
{
	m_bTestingLoader = true;
	/*
	FILEINDEX* FileIndex = (FILEINDEX*)g_FileSystem->GetFileIndex();
	for(FILEINDEX::iterator itor = FileIndex->begin(); itor != FileIndex->end() ; itor++ )
	{
		const std::string& FileName = (*itor).first;
		if( FileName.find( ".elu") != string::npos )
		{
			int index = FileName.length() - 4;
			if( index >= 0 )
			{
				if( FileName[index+0]  == '.'  &&
					FileName[index+1]  == 'e'  &&
					FileName[index+2]  == 'l'  &&
					FileName[index+3]  == 'u'  )
				{
					m_EluFileList.push_back(FileName);
				}
			}
		}
	}
	*/
	
	CFileSearch::FILE2PATH FileToPath;
	std::string Dummy;
	CFileSearch::FindAllFile("./Data/",FileToPath,Dummy);

	for(CFileSearch::FILE2PATH::iterator itor = FileToPath.begin(); itor != FileToPath.end() ; itor++ )
	{
		const std::string& FileName = (*itor).first;
		if( FileName.find( ".elu") != string::npos )
		{
			int index = FileName.length() - 4;
			if( index >= 0 )
			{
				if( FileName[index+0]  == '.'  &&
					FileName[index+1]  == 'e'  &&
					FileName[index+2]  == 'l'  &&
					FileName[index+3]  == 'u'  )
				{
					m_EluFileList.push_back(FileName);
				}
			}
		}
	}

	m_CreatedActorList.resize(m_EluFileList.size());

}

void MyApplication::DeActivateTestingLoader()
{
	m_bTestingLoader = false;
	m_EluFileList.clear();
}

void MyApplication::OnUpdateTestingLoader()
{
	if( false == m_bTestingLoader )		return;
	
	static const int NUM_CREATE_ACTOR_PER_FRAME = 100;
	static const int NUM_DESTROY_ACTOR_PER_FRAME = 40;

	//생성
	int NumOfFile = m_EluFileList.size();
	static int Count = -1;
	Count++;
	if( Count >= NumOfFile ) Count = 0;
	for( int i=0;i<NUM_CREATE_ACTOR_PER_FRAME;i++)
	{
		if( NULL != m_CreatedActorList[Count] ) continue;

		std::string EluFileNameToCreate = m_EluFileList[Count];
		RActor *pActor = new RActor;
		pActor->Create(EluFileNameToCreate.c_str(),true);

		m_CreatedActorList[Count] = pActor;
		REngine::GetSceneManager().AddSceneNode(pActor);

		RVector3 actorPos = m_pCamera->GetWorldPosition();
		RPICKINFO pickInfo;
		pickInfo.inPickOrigin = actorPos + RVector(0,0,300000);
		pickInfo.inPickTo = actorPos + RVector(0,0,-30000);
		pickInfo.outNormal = RVector( 0,0,1);
		REngine::GetSceneManager().PickSceneNode( pickInfo);
		actorPos.z = pickInfo.outPosition.z;
		actorPos.x+= 500;
		actorPos.y+= 500;

		pActor->SetPosition( actorPos);

		pActor->Play();
	}


	//해제
	for( int i=0;i<NUM_DESTROY_ACTOR_PER_FRAME;i++)
	{
		int Index = rand() % NumOfFile;
		RActor *pActorToDestroy = m_CreatedActorList[Index];
		if( NULL != pActorToDestroy )
		{
			m_CreatedActorList[Index] = NULL;
			pActorToDestroy->RemoveFromParent();
			SAFE_DELETE( pActorToDestroy);
		}
	}
}

static float fTimer = 0;
static float fTotalElapsed = 0;
static int	 nLoop = 0;
static int	 nSkip = 0;
int APIENTRY _tWinMain(HINSTANCE hInstance,
					   HINSTANCE hPrevInstance,
					   LPTSTR    lpCmdLine,
					   int       nCmdShow)
{
	if( REngine::IsDevelopVersion() )
		MCreateFileSystem( MFILEACCESS_GENERIC, "EngineResDev;Data" );
	else
		MCreateFileSystem( MFILEACCESS_GENERIC, "EngineRes;Data" );

	MInitLog();
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	mlog("Application started\n");

	return g_App.Run();
}
