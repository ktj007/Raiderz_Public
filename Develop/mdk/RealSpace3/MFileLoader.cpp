#include "stdafx.h"
#include "MFileLoader.h"
#include "MFileSystem.h"

#include <stdio.h>
#include "ddsfile.h"
#include "Dependencies/soil/SOIL.h"

namespace MFileLoad
{

// BMP 파일 읽기
unsigned char* CreateBitmapFrom24BMPFile(const char *filename, int *width, int *height, unsigned char fillAlpha)
{
	FILE *fp = NULL;
	fp = fopen(filename, "rb");
	if ( fp == NULL )
		return NULL;

	// bmp파일 헤더 읽기
	BITMAPFILEHEADER bmpHeader;
	BITMAPINFOHEADER bmpInfo;

	fread(&bmpHeader, sizeof(BITMAPFILEHEADER), 1, fp);
	if ( bmpHeader.bfType != 0x4d42 )
	{
		fclose(fp);
		return NULL;

	}
	//
	fread(&bmpInfo, sizeof(BITMAPINFOHEADER), 1, fp);
	if ( bmpInfo.biBitCount != 24 )
	{
		fclose(fp);
		return NULL;
	}

	int w = bmpInfo.biWidth;
	int h = bmpInfo.biHeight;
	int nPitch = w*3;
	int n = 0;
	int pn = ( nPitch % 4 );
	if ( pn )
		n = pn;

	nPitch += n;

	int nSize = w*nPitch;
	//int nSize = bmpHeader.bfSize - sizeof(BITMAPFILEHEADER);

	unsigned char *pDIB = new unsigned char[nSize];
	fread(pDIB, nSize, 1, fp);
	fclose(fp);

	unsigned char *pBuffer = new unsigned char[w*h*4];	// 1pixel = 32bit(4byte)
	unsigned char *pSrc = pDIB; // + sizeof(BITMAPINFOHEADER);
	unsigned char *pDst = pBuffer;

	for ( int y = 0; y<h; ++y )
	{
		for ( int x = 0; x<w; ++x )
		{
			*pDst = *pSrc;
			++pSrc;
			++pDst;
			*pDst = *pSrc;
			++pSrc;
			++pDst;
			*pDst = *pSrc;
			++pSrc;
			++pDst;

			*pDst = fillAlpha;
			++pDst;
		}
		pSrc += n;
	}

	delete[] pDIB;
	*width = w;
	*height = h;

	return pBuffer;
}
bool SaveBitmapFrom24bitRawTo24BMPFile(const char *filename, int width, int height, void *pRawdata)
{
	FILE *fp = NULL;
	fp = fopen(filename, "wb");
	if ( fp == NULL )
		return NULL;

	BITMAPFILEHEADER bmpHeader;
	BITMAPINFOHEADER bmpInfo;
	int nPitch = ( width*3 );
	int n = nPitch %4;
	if ( n )
		nPitch += ( 4-n );

	memset(&bmpHeader, 0, sizeof(bmpHeader));
	bmpHeader.bfType = 0x4d42;
	bmpHeader.bfOffBits = sizeof(bmpHeader) + sizeof(bmpInfo);
	bmpHeader.bfSize = nPitch*height + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPFILEHEADER);
	fwrite(&bmpHeader, sizeof(bmpHeader), 1, fp);

	memset(&bmpInfo, 0, sizeof(bmpInfo));
	bmpInfo.biSize = sizeof(bmpInfo);
	bmpInfo.biWidth = width;
	bmpInfo.biHeight = height;
	bmpInfo.biPlanes = 1;
	bmpInfo.biBitCount = 24;
	bmpInfo.biSizeImage = height*nPitch;
	fwrite(&bmpInfo, sizeof(bmpInfo), 1, fp);

	BYTE *pSrc =(BYTE*)pRawdata;
	BYTE *pLine = new BYTE[nPitch];

	for ( int y = 0; y<height; ++y )
	{
		BYTE *pDst = pLine;

		for ( int x = 0; x<width; ++x )
		{
			*pDst = *pSrc;
			++pSrc;
			++pDst;
			*pDst = *pSrc;
			++pSrc;
			++pDst;
			*pDst = *pSrc;
			++pSrc;
			++pDst;

			++pSrc;
			//++pDst;
		}

		fwrite(pLine, nPitch, sizeof(BYTE), fp);
	}

	fclose(fp);  
	delete [] pLine;

	return true;
}

bool SaveBitmapFrom8bitRawTo24BMPFile(const char *filename, int width, int height, void *pRawdata)
{
	FILE *fp = NULL;
	fp = fopen(filename, "wb");
	if ( fp == NULL )
		return NULL;

	BITMAPFILEHEADER bmpHeader;
	BITMAPINFOHEADER bmpInfo;
	int nPitch = ( width*3 );
	int n = nPitch %4;
	if ( n )
		nPitch += ( 4-n );

	memset(&bmpHeader, 0, sizeof(bmpHeader));
	bmpHeader.bfType = 0x4d42;
	bmpHeader.bfOffBits = sizeof(bmpHeader) + sizeof(bmpInfo);
	bmpHeader.bfSize = nPitch*height + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPFILEHEADER);
	fwrite(&bmpHeader, sizeof(bmpHeader), 1, fp);

	memset(&bmpInfo, 0, sizeof(bmpInfo));
	bmpInfo.biSize = sizeof(bmpInfo);
	bmpInfo.biWidth = width;
	bmpInfo.biHeight = height;
	bmpInfo.biPlanes = 1;
	bmpInfo.biBitCount = 24;
	bmpInfo.biSizeImage = height*nPitch;
	fwrite(&bmpInfo, sizeof(bmpInfo), 1, fp);

	BYTE *pSrc =(BYTE*)pRawdata;
	BYTE *pLine = new BYTE[nPitch];

	for ( int y = 0; y<height; ++y )
	{
		BYTE *pDst = pLine;

		for ( int x = 0; x<width; ++x )
		{
			*pDst = *pSrc;
			//++pSrc;
			++pDst;

			*pDst = *pSrc;
			//++pSrc;
			++pDst;

			*pDst = *pSrc;
			//++pSrc;
			++pDst;

			++pDst;
		}

		fwrite(pLine, nPitch, sizeof(BYTE), fp);
	}

	fclose(fp);  
	delete [] pLine;

	return true;
}

BYTE *CreateARGBFromDDS(const char *filename, int *width, int *height)
{
	// .dds를 직접 읽음
	MFile zFile;
	if (!zFile.Open(filename))
		return NULL;

	int nSize = zFile.GetLength();
	if (nSize == 0)
		return NULL;

	BYTE *pData = new BYTE[nSize];
	zFile.ReadAll((void*)pData, nSize);
	zFile.Close();

	// 헤더 분석
	MDDSDESC2 *pddsdesc = (MDDSDESC2*)(pData + 4);
	if ( !( pData[0] == 'D' && pData[1] == 'D' && pData[2] =='S' ) 
		|| pddsdesc->dwSize != 124 )
	{
		delete [] pData;
		return NULL;
	}

	if ( pddsdesc->ddpfPixelFormat.dwRGBBitCount != 32 )
	{
		delete [] pData;
		return NULL;
	}

	// main dds data
	if ( width )
		*width = pddsdesc->dwWidth;
	if ( height )
		*height = pddsdesc->dwHeight;
	
	BYTE *bData = pData + 4 + sizeof(MDDSDESC2);
	DWORD dwSize = pddsdesc->dwWidth*pddsdesc->dwHeight*4;
	BYTE *pTextureData = new BYTE[dwSize];
	memcpy(pTextureData, bData, dwSize);
	delete [] pData;
	pData = NULL;

	return pTextureData;
}

bool SaveFromNBitRawToDDSFile(const char *filename, int width, int height, int bpp, void *pRawdata)
{
	FILE *fp = fopen(filename,"wb");
	if ( fp == NULL )
		return false;

	fwrite("DDS ",4,1, fp);

	MDDSDESC2 ddsd;
	ZeroMemory(&ddsd, sizeof(MDDSDESC2));
	ddsd.dwSize = sizeof(MDDSDESC2);
	ddsd.dwFlags = DDSD_CAPS | DDSD_PIXELFORMAT | DDSD_WIDTH | DDSD_HEIGHT;
	ddsd.dwWidth = width;
	ddsd.dwHeight = height;

	ddsd.ddpfPixelFormat.dwSize = sizeof(MDDSPIXELFORMAT);
	ddsd.ddpfPixelFormat.dwFlags = DDPF_RGB | ( bpp == 4 ? DDPF_ALPHAPIXELS : 0 );
	ddsd.ddpfPixelFormat.dwFourCC = 0;
	ddsd.ddpfPixelFormat.dwRGBBitCount = bpp*8;
	ddsd.ddpfPixelFormat.dwRBitMask = 0x00ff0000;
	ddsd.ddpfPixelFormat.dwGBitMask = 0x0000ff00;
	ddsd.ddpfPixelFormat.dwBBitMask = 0x000000ff;
	ddsd.ddpfPixelFormat.dwRGBAlphaBitMask = ( bpp == 4 ? 0xff000000 : 0 );

	ddsd.ddsCaps.dwCaps = DDSCAPS_TEXTURE | (bpp == 4 ? DDSCAPS_ALPHA : 0);

	fwrite((void*)&ddsd,sizeof(MDDSDESC2), 1, fp);
	fwrite(pRawdata, width*height*bpp, 1, fp);
	fclose(fp);

	return true;
}

//////////////////////////////////////////////////////////////////////////
//

// 일반적인 RGBA 32bit 가 실제 메모리에서는 BGRA
// soil 라이브러리에서 요구하는 포맷이 RGBA
// r,b 가 바뀌는것이므로 양쪽방향으로 다 쓸수 있다
BYTE* CreateCopy_BGRA_to_RGBA(BYTE* pSource, int nPixels)
{
	// argb -> abgr
	BYTE *pConverted = new BYTE[ nPixels * 4 ];
	for( int i=0; i<nPixels; i++ )
	{
		pConverted[i*4]		= pSource[i*4+2];	// b
		pConverted[i*4+1]	= pSource[i*4+1];	// g
		pConverted[i*4+2]	= pSource[i*4];		// r
		pConverted[i*4+3]	= pSource[i*4+3];	// a
	}

	return pConverted;
}

BYTE* SOIL_CreateRawFromFile(const char *filename, int *width, int *height, int *channel)
{
	_ASSERT(width);
	_ASSERT(height);
	_ASSERT(channel);

	MFile zFile;
	if ( !zFile.Open(filename))
		return NULL;

	int nSize = zFile.GetLength();
	if (nSize == 0)
		return NULL;

	BYTE *pData = new BYTE[nSize];
	zFile.ReadAll((void*)pData, nSize);
	zFile.Close();

	BYTE *pImageBuffer = SOIL_load_image_from_memory(pData, nSize, width, height, channel, SOIL_LOAD_RGBA);
	
	delete [] pData;
	pData = NULL;

	if ( pImageBuffer == NULL )
		return NULL;

	if ( *width == 0 || *height == 0 || *channel == 0 )
	{
		SOIL_free_image_data(pImageBuffer);
		return NULL;
	}

	BYTE *pConverted = MFileLoad::CreateCopy_BGRA_to_RGBA( pImageBuffer, *width * *height );

	SOIL_free_image_data(pImageBuffer);
	return pConverted;
}

bool SOIL_SaveRawToDXT(const char *filename, BYTE *pData, int width, int height, int channel)
{
	BYTE *pConverted = MFileLoad::CreateCopy_BGRA_to_RGBA( pData, width * height );

	int nResult = SOIL_save_image(filename, SOIL_SAVE_TYPE_DDS, width, height, channel, pConverted);

	delete pConverted;

	return ( nResult ? true : false );
}

};