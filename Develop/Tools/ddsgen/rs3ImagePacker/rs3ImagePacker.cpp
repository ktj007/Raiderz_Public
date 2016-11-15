// rs3ImagePacker.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include <conio.h>
#include <direct.h>
#include <crtdbg.h>
#include <d3dx9.h>

#include "ddsgen.h"

#include "MTypes.h"
#include "MImagePacker.h"

#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")

#include <map>
#include <string>

int _tmain(int argc, _TCHAR* argv[])
{
	// for texture generation
	DDSGen aDDSGen;

	// for packer
	typedef std::map< int, std::pair< std::string, LPDIRECT3DTEXTURE9 > > PACK_ELEMENTS;
	PACK_ELEMENTS packElemets;

	// packer node visitor
	struct sNodeVisitor : public MImagePackerNode::MImagePackerNodeVisitor
	{
		virtual void Visit( MImagePackerNode::MImage& _img, const MRect& _imgRange )
		{
			PACK_ELEMENTS::iterator itr = m_pPackElement->find(_img.nID);
			_ASSERT( itr != m_pPackElement->end() );

			D3DLOCKED_RECT d3dlr_big;
			if( m_pBigTexture->LockRect(0, &d3dlr_big, NULL, NULL)  == S_OK)
			{
				D3DLOCKED_RECT d3dlr;
				if(itr->second.second->LockRect( 0, &d3dlr, NULL, NULL ) == S_OK )
				{
					for(int i=0;i< _img.nHeight; ++i)
					{
						DWORD* pSrc = (DWORD*)((BYTE*)d3dlr.pBits + i*d3dlr.Pitch);
						DWORD* pDest = (DWORD*)((BYTE*)d3dlr_big.pBits + ( _imgRange.top + i) * d3dlr_big.Pitch + _imgRange.left);
						for(int j=0;j<_img.nWidth;++j)
						{
							pDest[j] = pSrc[j];
						}
					}
					itr->second.second->UnlockRect(0);
				}
				m_pBigTexture->UnlockRect(0);
			}

			fprintf(m_pFile, "%s, %d, %d, %d, %d\n", itr->second.first.c_str(), _imgRange.top, _imgRange.left, _img.nWidth, _img.nHeight);
		}

		PACK_ELEMENTS*		m_pPackElement;
		LPDIRECT3DDEVICE9	m_pDevice;
		LPDIRECT3DTEXTURE9	m_pBigTexture;
		FILE*				m_pFile;

	} aPacker;

	if (argc < 3)
		goto usage;

	int nTotalWidth = atoi( (const char*)argv[1] );
	int nTotalHeight = atoi( (const char*)argv[2] );
	if (nTotalHeight < 0 || nTotalWidth < 0 || nTotalHeight > 2048 || nTotalWidth > 2048 )
		goto usage;

	// for texture, init d3d
	if (!aDDSGen.InitD3D())
	{
		printf("fail to create D3DDevice\n");
		return 0;
	}

	{
		// goto 때문에 에러 나서 {}로 묶음 , 펑션 등으로 빼는게 나을듯.
		MImagePackerNode aRootNode( MRect(0,0,nTotalWidth,nTotalHeight) );

		// build packer node
		for(int i=3;i<argc;i++)
		{
			// by pok. ddsgen에 있는것을 가져다 씀. 아이좋아.
			char* name = (char*)argv[i];
			int nLength=(int)strlen(name);
			if(nLength<4)
			{
				printf("%s : unexpected name \n",name);
				continue;
			}

			char *extpos=name+nLength-4;
			//D3DFORMAT format = D3DFMT_DXT3;
			D3DFORMAT format = D3DFMT_A8R8G8B8;

			int nMode = 0;
			if( _stricmp(extpos,".bmp")==0 ) {
				nMode = 1;
				//format = D3DFMT_DXT1;
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
					printf("%s loading fail : GetImageInfoFromFile\n",name);
					continue;
				}

				/*if (nMode == 1 && image_info.Format != D3DFMT_R8G8B8)
				{
					format = image_info.Format;
				}*/

				width = image_info.Width;
				height = image_info.Height;

				// image packing 가능한지 여부
				MImagePackerNode::MImage img;
				img.nID = i;			// 0 은 안됨
				img.nWidth = width;		// i번째 이미지 너비
				img.nHeight = height;	// i번째 이미지 높이
				MImagePackerNode* pNode = aRootNode.Insert(img);
				if (pNode == NULL)
				{
					printf("%s packing fail\n",name);
					continue;
				}

				bool bGenMip = false;
				if( FAILED(D3DXCreateTextureFromFileEx(

					aDDSGen.GetDevice() ,name, 
					width, height, 
					bGenMip ? D3DX_DEFAULT : 1, 
					0, format,D3DPOOL_SYSTEMMEM,
					D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR, 
					D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR, 
					0, NULL, NULL, &pTex )))
				{
					printf("%s loading fail : D3DXCreateTextureFromFileEx \n",name);
					continue;
				}
				else
				{
					packElemets.insert( PACK_ELEMENTS::value_type( i, pair<std::string, LPDIRECT3DTEXTURE9>(name, pTex) ) );
				}
			}
		}

		// set visitor
		aPacker.m_pPackElement = &packElemets;
		aPacker.m_pDevice = aDDSGen.GetDevice();
		aPacker.m_pFile = fopen("packed.txt", "w+");
		_ASSERT(aPacker.m_pFile);
		aDDSGen.GetDevice()->CreateTexture(nTotalWidth, nTotalHeight, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &aPacker.m_pBigTexture, NULL);
		_ASSERT(aPacker.m_pBigTexture);

		// traverse packer node with visitor
		aRootNode.Traverse( &aPacker );

		// save and close file
		fclose(aPacker.m_pFile);

		// save texture
		D3DXSaveTextureToFile(_T("packed.dds") ,D3DXIFF_DDS,aPacker.m_pBigTexture, NULL);

		// release texture
		aPacker.m_pBigTexture->Release();
		for( PACK_ELEMENTS::iterator itr = packElemets.begin(); itr != packElemets.end(); ++itr)
			itr->second.second->Release();
	}

	// close device
	aDDSGen.CloseD3D();

	return 0;

usage:
	printf("\n usage : 가로 세로 이미지1 이미지2\n");
	printf("   -옵션 :  아직없음\n");
	printf(" Copyright (C) MAIET Entertainment\n");
	return 0;
}
