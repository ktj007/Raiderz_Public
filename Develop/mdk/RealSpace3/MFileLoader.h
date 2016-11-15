#ifndef _MFILELOAD_H
#define _MFILELOAD_H

/**
 Helper class 입니다. Tool에서만 쓰시기 바랍니다. ( fopen으로 직접 파일을 읽고 쓰는 부분이 있으므로 ... )
 */
namespace MFileLoad
{
	// 24비트 BMP 파일을 로드하여 크기 정보를 돌려주고, BYTE Array를 생성한다.
	RS_API unsigned char* CreateBitmapFrom24BMPFile(const char *filename, int *width, int *height, unsigned char fillAlpha = 0x00);

	// 8Bit-Raw를 24비트 BMP 파일로 저장한다.
	RS_API bool SaveBitmapFrom8bitRawTo24BMPFile(const char *filename, int width, int height, void *pRawdata);

	// 24(32)Bit-Raw를 24비트 BMP 파일로 저장한다.
	RS_API bool SaveBitmapFrom24bitRawTo24BMPFile(const char *filename, int width, int height, void *pRawdata);
	
	// (n)Bpp 파일을 DDS 파일로 저장한다.
	RS_API bool SaveFromNBitRawToDDSFile(const char *filename, int width, int height, int bpp, void *pRawdata);

	// ARGB(32bit) 비압축 DDS 파일을 읽어들인다.
	RS_API BYTE* CreateARGBFromDDS(const char *filename, int *width, int *height);

	/**
	 * 24/32bit 파일만 사용하세요. (3, 4 Channel 용도 ) 다른 비트 수 파일은 검증되지 않았습니다.
	 * Wrap 을 시키면서, 내부적으로 malloc/free를 사용하는 것을 new 로 재할당해서 사용하도록 했습니다.
	 */
	
	// SOIL 라이브러리를 이용하여, 파일을 읽는다. ( bmp, tga, dds, png, jpg 지원 )
	RS_API BYTE* SOIL_CreateRawFromFile(const char *filename, int *width, int *height, int *channel);
	
	// SOIL 라이브러리를 이용하여 DXT1/5 포맷으로 저장한다.
	RS_API bool SOIL_SaveRawToDXT(const char *filename, BYTE *pData, int width, int height, int channel);
};

#endif