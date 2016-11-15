// Composite.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "d3dx9.h"
#include "crtdbg.h"
#include "Composite.h"


bool MakeCompositeMap(LPDIRECT3DDEVICE9 device, const char* szDiffuseMapName, const char* szPartsColorMap, const char* szOutput)
{
	LPDIRECT3DTEXTURE9 pTexDiffuse;
	D3DXIMAGE_INFO diffuseInfo;
	if( FAILED(D3DXCreateTextureFromFileEx(
		device,szDiffuseMapName, 
		D3DX_DEFAULT, D3DX_DEFAULT, 
		1 , // mipmap
		0 , // usage
		D3DFMT_A8R8G8B8,	// 32bit로 생성
		D3DPOOL_SYSTEMMEM,
		D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR, 
		D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR, 
		0, &diffuseInfo, NULL, &pTexDiffuse )))
	{
		return false;
	}

	LPDIRECT3DTEXTURE9 pTexParts;
	D3DXIMAGE_INFO partsInfo;
	if( FAILED(D3DXCreateTextureFromFileEx(
		device,szPartsColorMap, 
		D3DX_DEFAULT, D3DX_DEFAULT, 
		1 , // mipmap
		0, D3DFMT_UNKNOWN,D3DPOOL_SYSTEMMEM,
		D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR, 
		D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR, 
		0, &partsInfo, NULL, &pTexParts )))
	{
		pTexDiffuse->Release();
		return false;
	}

	if(diffuseInfo.Width != partsInfo.Width || diffuseInfo.Height != partsInfo.Height)
	{
		pTexParts->Release();
		pTexDiffuse->Release();
		return false;
	}

	HRESULT hr;
	D3DLOCKED_RECT d3dlr_diffuse;
	if( hr = pTexDiffuse->LockRect(0, &d3dlr_diffuse, NULL, NULL) == D3D_OK )
	{

		D3DLOCKED_RECT d3dlr_parts;
		if( hr = pTexParts->LockRect( 0, &d3dlr_parts, NULL, NULL) == D3D_OK  )
		{
			for(size_t i=0; i< diffuseInfo.Height; i++)
			{
				DWORD* pDiffuse = (DWORD*)((BYTE*)d3dlr_diffuse.pBits + d3dlr_diffuse.Pitch * i);
				DWORD* pParts = (DWORD*)((BYTE*)d3dlr_parts.pBits + d3dlr_parts.Pitch * i);
				for(size_t j=0; j< diffuseInfo.Width; j++)
				{
					BYTE diffuseAlpha = (BYTE)(pDiffuse[j] >> 24);
					BYTE partsAlpha = (BYTE)(pParts[j] >> 24);

					const BYTE ALPHA_REF = 128;

					DWORD combinedAlpha;
					// parts mask 가 있으면 diffuse 컬러를 없애준다
					if(partsAlpha<255)
					{
						if(diffuseAlpha < ALPHA_REF)
						{
							// 알파테스트를 less 를 사용할것이므로 diffuseAlpha 의 역상을 취한다
							int alphaTestScaledAlpha = (int)(255-diffuseAlpha) * REMAP_ALPHA_RANGE / 255;
							_ASSERT(alphaTestScaledAlpha<=REMAP_ALPHA_RANGE);
							combinedAlpha = alphaTestScaledAlpha + (255-REMAP_ALPHA_RANGE);
						}
						else
						{
							DWORD originalColor = pDiffuse[j];
							DWORD grey = (((originalColor & 0xff0000) >> 16) 
								+ ((originalColor & 0xff00) >> 8)
								+ (originalColor & 0xff ))/3;
							grey = grey * partsAlpha / 255;
							_ASSERT(grey<256);
							DWORD newColor = grey << 16 | grey << 8 | grey;

							pDiffuse[j] &= 0xff000000;
							pDiffuse[j] |= newColor;

							int partsScaledAlpha = (int)partsAlpha * (255-REMAP_ALPHA_RANGE) / 255;
							_ASSERT(partsScaledAlpha<=(255-REMAP_ALPHA_RANGE));
							combinedAlpha = partsScaledAlpha;
						}
					}
					else
					{
						// 알파테스트를 less 를 사용할것이므로 diffuseAlpha 의 역상을 취한다
						int alphaTestScaledAlpha = (int)(255-diffuseAlpha) * REMAP_ALPHA_RANGE / 255;
						_ASSERT(alphaTestScaledAlpha<=REMAP_ALPHA_RANGE);
						combinedAlpha = alphaTestScaledAlpha + (255-REMAP_ALPHA_RANGE);
					}
					_ASSERT(combinedAlpha <= 255);

					// pDiffuse[j] 의 alpha 에 combined Alpha 를 적용
					pDiffuse[j] = DWORD(combinedAlpha) << 24 | ( pDiffuse[j] & 0xffffff );
				}
			}
			pTexParts->UnlockRect(0);
		}else
			return false;

		pTexDiffuse->UnlockRect(0);
	}else
		return false;
	pTexParts->Release();

	if(!ConvertTexture(device, pTexDiffuse, D3DFMT_DXT5, szOutput))
	{
		pTexDiffuse->Release();
		return false;
	}

	pTexDiffuse->Release();
	return true;
}

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

bool RemapAlphaAndConvertTexture(LPDIRECT3DDEVICE9 device, const char* szSrcFileName, D3DFORMAT targetFormat, const char* szOutput)
{
	LPDIRECT3DTEXTURE9 pTexDiffuse;
	D3DXIMAGE_INFO diffuseInfo;
	if( FAILED(D3DXCreateTextureFromFileEx(
		device,szSrcFileName, 
		D3DX_DEFAULT, D3DX_DEFAULT, 
		1 , // mipmap
		0 , // usage
		D3DFMT_A8R8G8B8,	// 32bit로 생성
		D3DPOOL_SYSTEMMEM,
		D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR, 
		D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR, 
		0, &diffuseInfo, NULL, &pTexDiffuse )))
	{
		return false;
	}

	HRESULT hr;
	D3DLOCKED_RECT d3dlr_diffuse;
	if( hr = pTexDiffuse->LockRect(0, &d3dlr_diffuse, NULL, NULL) == D3D_OK )
	{
		for(size_t i=0; i< diffuseInfo.Height; i++)
		{
			DWORD* pDiffuse = (DWORD*)((BYTE*)d3dlr_diffuse.pBits + d3dlr_diffuse.Pitch * i);
			for(size_t j=0; j< diffuseInfo.Width; j++)
			{
				BYTE diffuseAlpha = (BYTE)(pDiffuse[j] >> 24);

				int alphaTestScaledAlpha = (int)(diffuseAlpha) * REMAP_ALPHA_RANGE / 255;
				DWORD remappedAlpha = alphaTestScaledAlpha + (255-REMAP_ALPHA_RANGE);

				// pDiffuse[j] 의 alpha 에 combined Alpha 를 적용
				pDiffuse[j] = DWORD(remappedAlpha) << 24 | ( pDiffuse[j] & 0xffffff );
			}
		}
		pTexDiffuse->UnlockRect(0);
	}else
		return false;

	bool bSuccess = ConvertTexture(device, pTexDiffuse, targetFormat, szOutput);
	pTexDiffuse->Release();
	return bSuccess;
}
