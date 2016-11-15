#include "stdafx.h"
#include "ManagedUtil.h"
#include "RDeviceD3D.h"

namespace EffectTool_NET {
	System::Drawing::Bitmap^ MHelp::BitmapFromTexture(rs3::RDeviceD3D *pd3dDevice, rs3::RTexture *pTexture)
	{
		using namespace rs3;

		LPDIRECT3DBASETEXTURE9 lpTexture = pd3dDevice->GetD3DTexture(pTexture);
		if ( lpTexture == NULL )
			return nullptr;

		LPD3DXBUFFER pBuffer = NULL;
		pd3dDevice->D3DXSaveTextureToFileInMemory(&pBuffer, D3DXIFF_BMP, lpTexture);
		
		// 헤더를 읽고
		BITMAPFILEHEADER *pHeader = (BITMAPFILEHEADER*)pBuffer->GetBufferPointer();
		BITMAPINFOHEADER *pInfo = (BITMAPINFOHEADER*)((BYTE*)pBuffer->GetBufferPointer() + sizeof(BITMAPFILEHEADER));
		BITMAPINFO bmpInfo;
		memset(&bmpInfo, 0, sizeof(bmpInfo));
		bmpInfo.bmiHeader = *pInfo;
		void *pData = (void*)((BYTE*)pBuffer->GetBufferPointer() + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER));

		if ( bmpInfo.bmiHeader.biBitCount < 24 )
		{
			System::Windows::Forms::MessageBox::Show(L"24비트 이하의 텍스쳐입니다.", "에러");
			return nullptr;
		}

		System::Drawing::Bitmap ^bitmap = gcnew Bitmap(bmpInfo.bmiHeader.biWidth, bmpInfo.bmiHeader.biHeight, bmpInfo.bmiHeader.biWidth*4, Imaging::PixelFormat::Format32bppRgb, IntPtr(pData));
		
		//Format32bppArgb
		bitmap->RotateFlip(RotateFlipType::RotateNoneFlipY);
		
		SAFE_RELEASE(pBuffer);
		return bitmap;
	}

	System::Drawing::Bitmap^ MHelp::BitmapFromTextureAlpha(rs3::RDeviceD3D *pd3dDevice, rs3::RTexture *pTexture)
	{
		using namespace rs3;

		LPDIRECT3DBASETEXTURE9 lpTexture = pd3dDevice->GetD3DTexture(pTexture);
		if ( lpTexture == NULL )
			return nullptr;

		LPD3DXBUFFER pBuffer = NULL;
		pd3dDevice->D3DXSaveTextureToFileInMemory(&pBuffer, D3DXIFF_BMP, lpTexture);

		// 헤더를 읽고
		BITMAPFILEHEADER *pHeader = (BITMAPFILEHEADER*)pBuffer->GetBufferPointer();
		BITMAPINFOHEADER *pInfo = (BITMAPINFOHEADER*)((BYTE*)pBuffer->GetBufferPointer() + sizeof(BITMAPFILEHEADER));
		BITMAPINFO bmpInfo;
		memset(&bmpInfo, 0, sizeof(bmpInfo));
		bmpInfo.bmiHeader = *pInfo;
		void *pData = (void*)((BYTE*)pBuffer->GetBufferPointer() + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER));

		if ( bmpInfo.bmiHeader.biBitCount < 24 )
		{
			System::Windows::Forms::MessageBox::Show(L"24비트 이하의 텍스쳐입니다.", "에러");
			return nullptr;
		}

		using namespace System::Drawing;

		Bitmap ^bitmap = gcnew Bitmap(bmpInfo.bmiHeader.biWidth, bmpInfo.bmiHeader.biHeight, bmpInfo.bmiHeader.biWidth*4
			, Imaging::PixelFormat::Format32bppArgb, IntPtr(pData));

		bitmap->RotateFlip(RotateFlipType::RotateNoneFlipY);
		SAFE_RELEASE(pBuffer);

		return bitmap;
	}
}