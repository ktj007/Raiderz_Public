#define _CRT_SECURE_NO_DEPRECATE

#include <windows.h>
#include <string.h>

#include "crtdbg.h"
#include "../sampleApp.h"

#include "MPathFinder.h"
#include "MPath.h"
#include "RRenderHelper.h"
#include "RDevice.h"
#include <MMSystem.h>

#pragma comment(lib,"winmm.lib")

class PathBenchmark : public sampleApp
{
protected:
	MPathFinder m_pathFinder;

public:
	PathBenchmark();
	virtual ~PathBenchmark();

	char *GetName()
	{
		return "MPathFinder benchmark";
	}
	
	void OnInit();
	void OnUpdate();
	bool OnCreate();
	void OnRender();
	void OnDestroy();

	static MVector3 getRandomPosition(iMesh *pMesh, iShape *pShape, iCollisionContext *pContext)
	{
		_ASSERT(pMesh);
		_ASSERT(pShape);
		_ASSERT(pContext);

		cPosition pos;
		while ( true )
		{
			pos = pMesh->generateRandomPosition();
			if ( !pMesh->testPointCollision(pShape, pContext, pos))
				break;
		}

		float fZ = pMesh->heightAtPositionF(pos);
		float fX = static_cast<float>(pos.x);
		float fY = static_cast<float>(pos.y);

		return MVector3(fX, fY, fZ);
	}

	virtual bool OnWndMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT *pReturn);

	std::vector<MVector3> m_vecRandomPositions;
	string m_strNavigationMeshName;

	static const int RANDOM_COUNT = 10000;
};

bool PathBenchmark::OnWndMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT *pReturn)
{
	return sampleApp::OnWndMessage(hWnd, message, wParam, lParam, pReturn);
}

PathBenchmark::PathBenchmark()
{
	m_pathFinder.Create();
}

PathBenchmark::~PathBenchmark()
{
}

PathBenchmark g_App;

void PathBenchmark::OnInit()
{
	sampleApp::OnInit();

	ChangeDirectoryToModule();
	MInitLog(MLOG_LEVEL_DEBUG, MLOG_FILE|MLOG_DEBUG);

	{
		using namespace rs3;

		REngine::GetConfig().m_nWidth = 640;
		REngine::GetConfig().m_nHeight = 400;
	}
}

bool PathBenchmark::OnCreate()
{
	_ASSERT(!m_strNavigationMeshName.empty());

	if (!m_pathFinder.AddZone(0, m_strNavigationMeshName))
		return false;

	m_vecRandomPositions.resize(RANDOM_COUNT*2);

	iPathEngine *pathengine = m_pathFinder.GetPathEngineInterface();
	iMesh *mesh = m_pathFinder.GetIMesh(0);
	_ASSERT(pathengine);
	_ASSERT(mesh);

	const char *SHAPE_NAME = "small";

	iShape *shape = m_pathFinder.GetShape(SHAPE_NAME);

	for ( int i = 0; i<RANDOM_COUNT*2; ++i )
		m_vecRandomPositions[i] = getRandomPosition(mesh, shape, NULL);

	mlog("begin \n");
	DWORD dwStartTime = timeGetTime();
	for ( int i = 0; i<RANDOM_COUNT*2; )
	{
		MPath path;
		m_pathFinder.FindPath(0, m_vecRandomPositions[i++], m_vecRandomPositions[i++], path, SHAPE_NAME);
		if ( !path.IsValid())
			mlog("- invalid path\n");
	}

	mlog("end \n");
	
	DWORD dwElapsedTime = timeGetTime() - dwStartTime;
	mlog("elapsed time %d, averageTime %f\n", dwElapsedTime, (float)dwElapsedTime/(float)RANDOM_COUNT);
	
	return false;
}

void PathBenchmark::OnRender()
{
	RRenderHelper::SetRenderState();
	RRenderHelper::EndRenderState();
}

void PathBenchmark::OnUpdate()
{
	sampleApp::OnUpdate();
}

void PathBenchmark::OnDestroy()
{
	sampleApp::OnDestroy();
}

int APIENTRY _tWinMain(HINSTANCE hInstance,
					   HINSTANCE hPrevInstance,
					   LPTSTR    lpCmdLine,
					   int       nCmdShow)
{
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );

	if (strlen(lpCmdLine) == 0)
	{
		::MessageBox(NULL,"Command Line에 Zone 이름을 넣어주십시오.","Info",MB_OK);
		return -1;
	}

	g_App.m_strNavigationMeshName = lpCmdLine;
	return g_App.Run();
}