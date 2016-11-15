#include "../sampleApp.h"
#include "RAmbientCubeController.h"
#include "RShaderCompositeManager.h"
#include "RDirectionalLightSceneNode.h"
#include "RRenderHelper.h"

#include "../../../Tools/AmbientCubeMaker/AmbientCubeMaker.h"

using namespace rs3;
using namespace TXML;

const char* ACTOR_MESH = "TestModel/Sphere.elu";

class RAmbientCubeTest : public RAmbientCube
{
public:
	void InitTest()
	{
		m_aabb.vmin = RVector( -1000.f, -1000.f, -1000.f );
		m_aabb.vmax = RVector(  1000.f,  1000.f,  1000.f );

		m_gridSize = RVector( 2000.f, 2000.f, 2000.f );
		m_halfGridSize = RVector( 1000.f, 1000.f, 1000.f );
		m_dataCount = 1;
		m_index = new int[1];
		m_index[0] = 0;
		m_data = new RVector[DEGREE];
		for( int i = 0; i < DEGREE; ++ i )
		{
			m_data[i] = RVector( 1.f, 1.f, 1.f );
		}
		m_gridCountX = 1;
		m_gridCountY = 1;
		m_gridCountZ = 1;
		m_gridCountXY = 1;
	}
};

class myApplication : public sampleApp
{
public:
	myApplication() : sampleApp(), m_pActor(NULL), m_pAmbientCube(NULL), m_pLightSceneNode(NULL) {}
	char *GetName() { return "rs3 ambient cube sample application"; }

	bool OnCreate();
	void OnDestroy();
	void OnUpdate();
	void OnRender();
	bool OnWndMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT *pReturn);

private:
	void LoadActors();
	void IncreaseAmbientCubeData( int index );
	void DecreaseAmbientCubeData( int index );

private:
	RActor*						m_pActor;
	RAmbientCubeTest*			m_pAmbientCube;
	RDirectionalLightSceneNode* m_pLightSceneNode;

} g_App;

bool myApplication::OnWndMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT *pReturn)
{
 	switch( message )
 	{
	case WM_KEYDOWN:
		switch( wParam )
		{
		case 'L': IncreaseAmbientCubeData( 0 ); break;
		case 'H': DecreaseAmbientCubeData( 0 ); break;

		case 'J': IncreaseAmbientCubeData( 1 ); break;
		case 'F': DecreaseAmbientCubeData( 1 ); break;

		case 'I': IncreaseAmbientCubeData( 2 ); break;
		case 'T': DecreaseAmbientCubeData( 2 ); break;

		case 'K': IncreaseAmbientCubeData( 3 ); break;
		case 'G': DecreaseAmbientCubeData( 3 ); break;

		case 'U': IncreaseAmbientCubeData( 4 ); break;
		case 'R': DecreaseAmbientCubeData( 4 ); break;

		case 'O': IncreaseAmbientCubeData( 5 ); break;
		case 'Y': DecreaseAmbientCubeData( 5 ); break;
		}
	}
	return sampleApp::OnWndMessage(hWnd,message,wParam,lParam,pReturn);
}

bool myApplication::OnCreate()
{
	RSceneManager* pSceneManager = GetSceneManager();
	_ASSERT( pSceneManager );
	
	SetClearColor( D3DCOLOR_ARGB( 255, 64, 64, 64 ) );

	RDirectionalLightSceneNode* pDirLight = pSceneManager->GetLightManager()->GetDefaultLight();
	if( pDirLight )
		pDirLight->RemoveFromParent();

	m_pLightSceneNode = new RDirectionalLightSceneNode;
	m_pLightSceneNode->SetLightDiffuse( 0.f, 0.f, 0.f, 0.f );
	m_pLightSceneNode->SetLightAmbient( 0.f, 0.f, 0.f, 0.f );
	m_pLightSceneNode->SetDirection( RVector( 0.f, 1.f, 0.f ) );
	pSceneManager->AddSceneNode( m_pLightSceneNode );

	m_pAmbientCube = new RAmbientCubeTest();
	m_pAmbientCube->InitTest();
	pSceneManager->SetAmbientCube( m_pAmbientCube );

	m_fDistToCamera = 200;
	m_cameraTargetPos = RVector( 0, 0, 0 );
	m_fCameraZ = 1.6f;
	m_fCameraX = 2.0f;

	REngine::GetSceneManager().GetPrimaryCamera()->SetNearFarZ( 5.0f, 50000.0f );
	REngine::GetSceneManager().GetPrimaryCamera()->SetFov( DegreeToRadian(60.f) );

	LoadActors();

	return true;
}

void myApplication::OnUpdate()
{
	sampleApp::OnUpdate();

	if( m_pActor )
	{
		m_pActor->Update();

	//	RCameraSceneNode* pCamera = REngine::GetSceneManager().GetPrimaryCamera();
	//	RVector pos = pCamera->GetPosition() + pCamera->GetDirection() * 400.f;
	//	m_pActors[0].SetPosition( pos );

		char szBuffer[MAX_PATH*2] = "";

		if( m_pActor->GetAmbientCubeController() )
		{
			const RVector4* v = m_pActor->GetAmbientCubeController()->GetData();
			sprintf( szBuffer, "%g %g %g\n%g %g %g\n%g %g %g\n%g %g %g\n%g %g %g\n%g %g %g\n",
				v[0].x, v[0].y, v[0].z,
				v[1].x, v[1].y, v[1].z,
				v[2].x, v[2].y, v[2].z,
				v[3].x, v[3].y, v[3].z,
				v[4].x, v[4].y, v[4].z,
				v[5].x, v[5].y, v[5].z );
		}

		SetText( szBuffer );
	}
}

void myApplication::OnRender()
{
	RMatrix mat;
	mat.MakeIdentity();
	RRenderHelper::RenderAxis( mat, 30.f );
}

void myApplication::OnDestroy()
{
	GetSceneManager()->SetAmbientCube( NULL );

	if( m_pActor )
		m_pActor->RemoveFromParent();

	if( m_pLightSceneNode )
	{
		m_pLightSceneNode->RemoveFromParent();
		SAFE_DELETE( m_pLightSceneNode );
	}

	SAFE_DELETE( m_pActor );
	SAFE_DELETE( m_pAmbientCube );
}

void myApplication::LoadActors()
{
	RSceneManager* pSceneManager = GetSceneManager();
	_ASSERT( pSceneManager );

	m_pActor = new RActor;
	pSceneManager->AddSceneNode( m_pActor );
	m_pActor->Create( ACTOR_MESH );
	m_pActor->Update();	// aabb 업데이트를 위해
}

void myApplication::IncreaseAmbientCubeData( int index )
{
	if( m_pAmbientCube )
	{
		m_pAmbientCube->m_data[index] += RVector( 0.1f, 0.1f, 0.1f );
		if( m_pAmbientCube->m_data[index].x > 1.0f )
			m_pAmbientCube->m_data[index] = RVector( 1.0f, 1.0f, 1.0f );
	}
}

void myApplication::DecreaseAmbientCubeData( int index )
{
	if( m_pAmbientCube )
	{
		m_pAmbientCube->m_data[index] -= RVector( 0.1f, 0.1f, 0.1f );
		if( m_pAmbientCube->m_data[index].x < 0.0f )
			m_pAmbientCube->m_data[index] = RVector::ZERO;
	}
}

int APIENTRY _tWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow )
{
	g_App.Run();
	PFC_FINALANALYSIS("rs3ambientcube.txt");
}