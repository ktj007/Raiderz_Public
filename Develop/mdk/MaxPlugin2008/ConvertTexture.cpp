#include "stdafx.h"
#include "d3dx9.h"

bool ConvertTexture(LPDIRECT3DDEVICE9 device, LPDIRECT3DTEXTURE9 pSrcTexture, D3DFORMAT targetFormat, const char* szOutput)
{
	//////////////////////////////////////////////////////////////////////////
	// 만든 텍스쳐를 메모리에 저장해둔다
	LPD3DXBUFFER pBuffer = NULL;
	D3DXSaveTextureToFileInMemory(&pBuffer,D3DXIFF_DDS, pSrcTexture, NULL);

	//////////////////////////////////////////////////////////////////////////
	// 밉맵을 만들고 압축텍스쳐로 변환한다

	D3DXIMAGE_INFO diffuseInfo;
	LPDIRECT3DTEXTURE9 pTargetTexture;
	if( FAILED(D3DXCreateTextureFromFileInMemoryEx(

		device,pBuffer->GetBufferPointer(), pBuffer->GetBufferSize(), 

		D3DX_DEFAULT, D3DX_DEFAULT, 
		D3DX_DEFAULT , // mipmap
		0 , // usage
		targetFormat ,D3DPOOL_SYSTEMMEM,
		D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR, 
		D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR, 
		0, &diffuseInfo, NULL, &pTargetTexture )))
	{
		_ASSERT(FALSE);
		return false;
	}

	// dxt3 or dxt5

	bool bSuccess = (D3D_OK == D3DXSaveTextureToFile(szOutput,D3DXIFF_DDS, pTargetTexture, NULL));

	pTargetTexture->Release();
	pBuffer->Release();

	return bSuccess;
}

bool ConvertTexture(LPDIRECT3DDEVICE9 device, const char* szSrcFileName, D3DFORMAT targetPlainFormat, D3DFORMAT targetAlphaFormat, const char* szOutput)
{
	D3DXIMAGE_INFO imageInfo;
	if( FAILED(D3DXGetImageInfoFromFile(szSrcFileName,&imageInfo)) )
		return false;

	D3DFORMAT targetFormat = D3DFMT_UNKNOWN;
	if(imageInfo.Format == D3DFMT_A8R8G8B8)
		targetFormat = targetAlphaFormat;
	else
		targetFormat = targetPlainFormat;

	LPDIRECT3DTEXTURE9 pTexture;
	if( FAILED(D3DXCreateTextureFromFileEx(
		device,szSrcFileName, 
		D3DX_DEFAULT, D3DX_DEFAULT, 
		1 , // mipmap
		0 , // usage
		D3DFMT_UNKNOWN,D3DPOOL_SYSTEMMEM,
		D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR, 
		D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR, 
		0, &imageInfo, NULL, &pTexture )))
	{
		return false;
	}

	bool bSuccess = ConvertTexture(device,pTexture,targetFormat,szOutput);

	pTexture->Release();

	return bSuccess;
}