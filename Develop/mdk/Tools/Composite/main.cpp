#include "stdafx.h"

#include "windows.h"
#include "d3dx9.h"
#include "crtdbg.h"

#include "Composite.h"

#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")


LPDIRECT3D9				g_pD3D=NULL;
LPDIRECT3DDEVICE9		g_pd3dDevice=NULL;
D3DPRESENT_PARAMETERS	g_d3dpp; 


bool init_d3d()
{
	HRESULT hr;

	if( NULL == ( g_pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
		return false;

	ZeroMemory( &g_d3dpp, sizeof(g_d3dpp) );
	g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	g_d3dpp.BackBufferWidth = 640;
	g_d3dpp.BackBufferHeight = 480;
	g_d3dpp.BackBufferCount = 1;
	g_d3dpp.Windowed   = true;
	g_d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
	g_d3dpp.Flags=NULL;

	DWORD BehaviorFlags=D3DCREATE_FPU_PRESERVE | D3DCREATE_SOFTWARE_VERTEXPROCESSING;

	hr = g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,GetDesktopWindow(),BehaviorFlags,&g_d3dpp,&g_pd3dDevice);
	if( FAILED( hr ) )
	{
		g_pD3D->Release();
		return false;
	}

	return true;
}

void close_d3d()
{
	g_pd3dDevice->Release();
	g_pD3D->Release();
}

void Message(const char* szMessage)
{
	MessageBox(NULL,szMessage,"notice",MB_OK);
}

int _tmain(int argc, _TCHAR* argv[])
{
	// 	if(argc)
	// 	{
	// 		for(int i=0;i<argc;i++)
	// 		{
	// 			char buffer[256];
	// 			sprintf_s(buffer,"%d %s",i,argv[i]);
	// 			MessageBox(NULL,buffer,"test",MB_OK);
	// 		}
	// 	}

	if(argc==3)
	{
		init_d3d();

		const char* szDiffuseMapName = argv[1];
		const char* szPartsMaskMapName = argv[2];

		char szOutputName[256];
		sprintf_s(szOutputName,"%s.dds",szDiffuseMapName );
		if(!MakeCompositeMap(g_pd3dDevice, szDiffuseMapName , szPartsMaskMapName, szOutputName))
		{
			Message("failed");
		}

		close_d3d();
	}else
	{
		printf("usage : Composite DiffuseMapName MaskMapName\n");
		printf("        output file name is DiffuseMapName.dds\n");

	}
	return 0;
}
