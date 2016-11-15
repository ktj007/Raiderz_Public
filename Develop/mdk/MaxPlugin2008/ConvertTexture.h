#pragma once

/*
dds 로 변환과 mipmap 생성 , dxtn 으로 압축

*/

/// targetFormat 으로 변환한다
bool ConvertTexture(LPDIRECT3DDEVICE9 device, LPDIRECT3DTEXTURE9 pSrcTexture, D3DFORMAT targetFormat, const char* szOutput);

/// 일반 = targetPainFormat , 알파가 있는경우 targetAlphaFormat 으로 변환한다
bool ConvertTexture(LPDIRECT3DDEVICE9 device, const char* szSrcFileName, D3DFORMAT targetPlainFormat, D3DFORMAT targetAlphaFormat, const char* szOutput);
