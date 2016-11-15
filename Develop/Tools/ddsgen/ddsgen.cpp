#include "stdafx.h"

#include <conio.h>
#include <crtdbg.h>

#include "ddsgen.h"

bool DDSGen::InitD3D()
{
	HRESULT hr;

	if( NULL == ( m_pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
		return false;

	ZeroMemory( &m_d3dpp, sizeof(m_d3dpp) );
	m_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	m_d3dpp.BackBufferWidth = 640;
	m_d3dpp.BackBufferHeight = 480;
	m_d3dpp.BackBufferCount = 1;
	m_d3dpp.Windowed   = true;
	m_d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
	m_d3dpp.Flags=NULL;

	DWORD BehaviorFlags=D3DCREATE_FPU_PRESERVE | D3DCREATE_SOFTWARE_VERTEXPROCESSING;

	hr = m_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,GetDesktopWindow(),BehaviorFlags,&m_d3dpp,&m_pd3dDevice);
	if( FAILED( hr ) )
	{
		m_pD3D->Release();
		return false;
	}
	return true;
}

void DDSGen::CloseD3D()
{
	_ASSERT(m_pd3dDevice);
	_ASSERT(m_pD3D);

	m_pd3dDevice->Release();
	m_pD3D->Release();
}

bool DDSGen::CheckAndGen(char *name)
{
	int nLength=(int)strlen(name);
	if(nLength<4) return false;

	char *extpos=name+nLength-4;

	if(m_bUndds)
	{
		if(_stricmp(extpos,".dds")==0)
		{
			LPDIRECT3DTEXTURE9 pTex;
			if( FAILED(D3DXCreateTextureFromFileEx(

				m_pd3dDevice,name, 
				D3DX_DEFAULT, D3DX_DEFAULT, 
				m_bGenMip ? D3DX_DEFAULT : 1, 
				0, D3DFMT_UNKNOWN,D3DPOOL_SYSTEMMEM,
				D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR, 
				D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR, 
				0, NULL, NULL, &pTex )))
			{
				printf("%s loading fail\n",name);
				return false;
			}

			char tgatexturefile[MAX_PATH];
			strcpy_s(tgatexturefile, MAX_PATH, name);
			strcpy_s(tgatexturefile+nLength-3, MAX_PATH, "bmp");
			HRESULT hr=D3DXSaveTextureToFile(tgatexturefile,D3DXIFF_BMP,pTex,NULL);
			_ASSERT(hr==D3D_OK);

			pTex->Release();
			return true;
		}
	}
	else
	{
		int nMode = 0;

		D3DFORMAT format = D3DFMT_DXT3;

		if( _stricmp(extpos,".bmp")==0 ) {
			nMode = 1;
			format = D3DFMT_DXT1;
		}
		else if( _stricmp(extpos,".tga")==0 ) nMode = 2;
		else if( _stricmp(extpos,".png")==0 ) nMode = 3;

		if( nMode )
		{
			LPDIRECT3DTEXTURE9 pTex;
			UINT width = D3DX_DEFAULT, height = D3DX_DEFAULT;
			D3DXIMAGE_INFO image_info;
			memset(&image_info,0,sizeof(D3DXIMAGE_INFO));

			if( FAILED( D3DXGetImageInfoFromFile( name,&image_info) ) )
			{
				return false;
			}

			if (nMode == 1 && image_info.Format != D3DFMT_R8G8B8)
			{
				format = image_info.Format;
			}

			width = max(1, image_info.Width  / (1 << m_nTexLevel));
			height = max(1, image_info.Height / (1 << m_nTexLevel));


			if( FAILED(D3DXCreateTextureFromFileEx(

				m_pd3dDevice,name, 
				width, height, 
				m_bGenMip ? D3DX_DEFAULT : 1, 
				0, format,D3DPOOL_SYSTEMMEM,
				D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR, 
				D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR, 
				0, NULL, NULL, &pTex )))
			{
				printf("%s loading fail\n",name);
				return false;
			}

			char ddstexturefile[MAX_PATH];
			sprintf_s(ddstexturefile,"%s.dds",name);
			D3DXSaveTextureToFile(ddstexturefile,D3DXIFF_DDS,pTex,NULL);

			pTex->Release();
			return true;
		}
	}
	return false;
}

void DDSGen::GenDDS(const char *szPath)
{
	m_nRecursionLevel++;
	WIN32_FIND_DATA FileData; 
	HANDLE hSearch; 

	BOOL fFinished = FALSE; 

	char filetofind[MAX_PATH];
	sprintf_s(filetofind,"%s/*.*",szPath);

	hSearch = FindFirstFile(filetofind, &FileData); 
	if (hSearch == INVALID_HANDLE_VALUE) 
	{ 
		m_nRecursionLevel--;
		return;
	}

	while (!fFinished) 
	{ 
		char childpath[MAX_PATH];
		sprintf_s(childpath,"%s/%s",szPath,FileData.cFileName);

		if(FileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY &&
			strcmp(".",FileData.cFileName)!=0 &&
			strcmp("..",FileData.cFileName)!=0)
		{
			//			chdir(FileData.cFileName);
			GenDDS(childpath);
			//			chdir("..");
		}

		if(CheckAndGen(childpath))
		{
			for(int i=0;i<m_nRecursionLevel;i++)
				printf("   ");
			printf("%s\n",FileData.cFileName);
		}

		if (!FindNextFile(hSearch, &FileData)) 
		{
			if (GetLastError() == ERROR_NO_MORE_FILES) 
			{ 
				fFinished = TRUE; 
			} 
			else 
			{ 
				printf("Couldn't find next file."); 
				m_nRecursionLevel--;
				return;
			} 
		}
	} 

	m_nRecursionLevel--;
}
