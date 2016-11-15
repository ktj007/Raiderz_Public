// eluValidator.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"

#include "shlwapi.h"
#include "MDebug.h"
#include "RMesh.h"
#include "RMeshMgr.h"
#include "REngine.h"
#include "RDeviceD3D.h"
#include "RSceneManager.h"
#include "MFileSystem.h"

using namespace std;
using namespace rs3;

char g_szModuleFileName[_MAX_DIR] = {0,};

bool ValidateEluFileVersion( RMesh* pMesh )
{
	_ASSERT( pMesh );
	_ASSERT( pMesh->GetFileVersion() <= EXPORTER_CURRENT_MESH_VER );

	if( pMesh->GetFileVersion() < EXPORTER_CURRENT_MESH_VER )
	{
		mlog( "\t최신 버전 파일이 아님.\n",
			pMesh->GetName().c_str(), pMesh->GetFileVersion(), EXPORTER_CURRENT_MESH_VER );

		return false;
	}

	return true;
}

bool IsMeshUseMaterial( RMesh* pMesh, RMaterial* pMaterial )
{
	_ASSERT( pMesh );
	_ASSERT( pMaterial );

	const size_t nodeCount = pMesh->GetNodeCount();
	if( 0 == nodeCount )
		return true;

	for( size_t i = 0; i < nodeCount; ++i )
	{
		RMeshNode* pMeshNode = pMesh->GetNode( i );

		size_t materialCount = pMeshNode->GetMtrlCount();
		for( size_t j = 0; j < materialCount; ++j )
		{
			if( pMeshNode->GetMtrl(j) == pMaterial )
				return true;
		}
	}

	return false;
}

// 메시에서 마테리얼 리소스를 실제로 사용하는지 체크한다
unsigned int g_countValidateEluUsingMaterialFailed = 0;
bool ValidateEluUsingMaterial( RMesh* pMesh )
{
	_ASSERT( pMesh );

	bool bValid = true;

	size_t meshMaterialCount = pMesh->GetMaterialResourceCount();
	for( size_t i = 0; i < meshMaterialCount; ++i )
	{
		RMaterial* pMaterial = pMesh->GetMaterialResource(i)->m_pMaterial;
		if( false == IsMeshUseMaterial( pMesh, pMaterial ) )
		{
			if( bValid )
			{
				g_countValidateEluUsingMaterialFailed++;
				mlog( "### %s\n", pMesh->GetName().c_str() );
			}
			bValid = false;

			mlog( "\t!! 사용하지 않는 마테리얼 %s가 있습니다.\n", pMaterial->Name.c_str() );
		}
	}

	if( false == bValid )
		mlog( "\n" );

	return bValid;
}

// 메시 노드가 2개 이상의 마테리얼을 사용하는지 체크.
bool IsMeshUseMultiSubMaterial( RMesh* pMesh )
{
	_ASSERT( pMesh );

	const size_t nodeCount = pMesh->GetNodeCount();
	if( 0 == nodeCount )
		return false;

	bool bUseMultiSubMaterial = false;

	for( size_t i = 0; i < nodeCount; ++i )
	{
		RMeshNode* pMeshNode = pMesh->GetNode( i );

		size_t materialCount = pMeshNode->GetMtrlCount();
		if( materialCount > 1 )
		{
			if( false == bUseMultiSubMaterial )
			{
				mlog( "### %s - 멀티/서브 마테리얼을 사용.\n", pMesh->GetName().c_str() );
			}
			bUseMultiSubMaterial = true;

			mlog( "\t 액터 노드 %s\n", pMeshNode->GetName() );
			for( int j = 0; j < pMeshNode->GetMtrlCount(); ++j )
			{
				mlog( "\t\t 마테리얼 %s\n", pMeshNode->GetMtrl( j )->Name.c_str() );
			}
		}
	}

	if( true == bUseMultiSubMaterial )
		mlog( "\n" );

	return bUseMultiSubMaterial;
}

// HIDE 된 충돌 메시 노드가 있는지 체크
bool IsMeshExistHidingCollisionMeshNode( RMesh* pMesh )
{
	_ASSERT( pMesh );

	const size_t nodeCount = pMesh->GetNodeCount();
	if( 0 == nodeCount )
		return false;

	bool bExist = false;

	for( size_t i = 0; i < nodeCount; ++i )
	{
		RMeshNode* pMeshNode = pMesh->GetNode( i );

		if( false == pMeshNode->isVisibleMesh() && pMeshNode->isCollisionMesh() )
		{
			if( false == bExist )
			{
				mlog( "### %s - 숨겨진 충돌 메시노드가 있음.\n", pMesh->GetName().c_str() );
			}
			bExist = true;

			mlog( "\t 숨겨진 충돌 메시 노드 %s\n", pMeshNode->GetName() );
		}
	}

	if( true == bExist )
		mlog( "\n" );

	return bExist;
}

void CreateAndValidateMeshFile( const string& fileName )
{
	RMesh* pMesh = REngine::GetSceneManager().CreateResource<RMesh>( fileName.c_str() );

	if( pMesh->BuildAll() )
	{
	//	ValidateEluFileVersion( pMesh );
		ValidateEluUsingMaterial( pMesh );
	//	IsMeshUseMultiSubMaterial( pMesh );
	//	IsMeshExistHidingCollisionMeshNode( pMesh );
	}
	else
	{
		mlog( "!!! %s 파일 로딩 실패\n", fileName.c_str() );
	}

	REngine::GetSceneManager().ReleaseResource( pMesh );
}

void ValidateMeshFiles( const string& strSubDir )
{
	string filter = strSubDir;
	filter += "*";

	bool bResult = true;
	WIN32_FIND_DATA wfd;
	HANDLE hSrch = FindFirstFile( filter.c_str(), &wfd );
	if( hSrch == INVALID_HANDLE_VALUE )
		bResult = false;

	while( bResult )
	{
		if( (wfd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) == false )
		{
			if( wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
			{
				if( (strcmp(wfd.cFileName, ".") != 0 ) && (strcmp(wfd.cFileName, "..") != 0 ) )
				{
					string strNewPath( strSubDir );

					if( strSubDir.empty() == false )
						strNewPath += "\\";

					strNewPath += wfd.cFileName;
					strNewPath += "\\";
					ValidateMeshFiles( strNewPath );
				}
			}
			else
			{
				static char drive[_MAX_DRIVE], dir[_MAX_DIR], fname[_MAX_FNAME], ext[_MAX_EXT];
				_splitpath_s( wfd.cFileName, drive, _MAX_DRIVE, dir, _MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT );

				if( _strnicmp( ext, ".elu", 4 ) == 0 )
				{
					string strMeshFileName( g_szModuleFileName );
					strMeshFileName += "\\";
					strMeshFileName += strSubDir;
					strMeshFileName += "\\";
					strMeshFileName += wfd.cFileName;
					CreateAndValidateMeshFile( strMeshFileName );
				}
			}
		}

		bResult = FindNextFile(hSrch, &wfd) ? true : false;
	}
	FindClose(hSrch);
}

bool Init()
{
	// 디바이스 생성
	HWND hWnd = GetDesktopWindow();
	RDeviceD3D* pDevice = new RDeviceD3D;
	bool bRet = pDevice->Create(hWnd);
	if(!bRet)
	{
		_ASSERT(false);
		delete pDevice;
		return false;
	}
	REngine::RegisterDevice(pDevice);

	// 씬 매니저 생성
	RSceneManager* pSceneManager = new RSceneManager;
	REngine::RegisterSceneManager(pSceneManager);
	bRet = pSceneManager->Create(pDevice);
	_ASSERT(bRet == true);
//	bRet = REngine::InitSharedResource();
	if(!bRet)
	{
		return false;
	}
	_ASSERT(bRet == true);
	pSceneManager->SetSSAO( false);

	if(!RMesh::InitSharedResources(REngine::GetDevicePtr()))
		return false;

	return true;
}

void Destroy()
{
	RSceneManager* pSceneManager = REngine::GetSceneManagerPtr();
	_ASSERT(pSceneManager != NULL);

	// 씬매니저
	pSceneManager->Clear();

	RMesh::DestroySharedResources();
//	REngine::DestroySharedResource();
	pSceneManager->Destroy();
	delete pSceneManager;
	REngine::RegisterSceneManager(NULL);

	// 디바이스 종료
	RDevice* pDevice = REngine::GetDevicePtr();
	_ASSERT(pDevice != NULL);
	pDevice->Destroy();
	delete pDevice;
	REngine::RegisterDevice(NULL);
}

int _tmain(int argc, _TCHAR* argv[])
{
	GetModuleFileName(NULL, g_szModuleFileName, _MAX_DIR);
	// Current Directory를 맞춤
	PathRemoveFileSpec(g_szModuleFileName);
	SetCurrentDirectory(g_szModuleFileName);

	if( REngine::IsDevelopVersion() )
		MCreateFileSystem( MFILEACCESS_GENERIC, "EngineResDev;Data" );
	else
		MCreateFileSystem( MFILEACCESS_GENERIC, "EngineRes;Data" );

	MInitLog( MLOG_LEVEL_DEBUG, MLOG_FILE | MLOG_DEBUG | MLOG_CONSOLE, NULL, "eluValidator.txt" );

	if( Init() == false )
		return 1;

	ValidateMeshFiles( string( "Data\\" ) );
	mlog( "마테리얼 리소스가 올바르지 않은 elu파일의 개수 - %d\n", g_countValidateEluUsingMaterialFailed );

	Destroy();
	return 0;
}