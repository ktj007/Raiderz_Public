#pragma once

/*
= 정의 =
파츠 마스크 맵 : 파츠에 염색을 하는 텍스쳐로, 255이면 전혀 안섞이고,
0 이면 염색색깔만 나옵니다.

= 구현 =
파츠 마스크맵과 알파 테스트를 같이 사용할 경우 하나의 텍스쳐로 나타내기 위해
0~255 의 알파값중 적당한 ( 기본은 8 ) 값을 포기하고,
0~246 을 파츠 마스크맵으로 쓰고 247~255 을 알파 테스트를 위한 영역으로 정하고,
이를 섞어주는 프로그램이다.  

= 알려진 문제점 =
parts 색깔이 전혀 안섞이는 부분을 알파테스트로 없앨땐 경계가 자연스러우나 
parts 색깔이 섞이는 부분을 따낼때는 반텍셀 만큼  경계에 원래 diffuse 색이 묻어나오는 문제가 있다.

부가적인 기능으로 dds 로 전환과 mipmap 생성 , dxtn 으로 압축이 있다

*/

// 알파테스트를 위해 할당할 영역,, 현재 16으로 설정했으나 필요하다면 옵션으로 설정가능하도록 하자
const int REMAP_ALPHA_RANGE = 16;

// diffuse map 과 parts 를 섞어서 하나의 맵으로 만든다 ( 위 설명 참조 )
bool MakeCompositeMap(LPDIRECT3DDEVICE9 device, const char* szDiffuseMapName, const char* szPartsColorMap, const char* szOutput);

/// targetFormat 으로 변환한다
bool ConvertTexture(LPDIRECT3DDEVICE9 device, LPDIRECT3DTEXTURE9 pSrcTexture, D3DFORMAT targetFormat, const char* szOutput);

/// 일반 = targetPainFormat , 알파가 있는경우 targetAlphaFormat 으로 변환한다
bool ConvertTexture(LPDIRECT3DDEVICE9 device, const char* szSrcFileName, D3DFORMAT targetPlainFormat, D3DFORMAT targetAlphaFormat, const char* szOutput);

/// 보통 알파 테스트를 사용하면 parts color 가 섞여 나오므로 알파를 0~255 -> (255-REMAP_ALPHA_RANGE)~255 로 리매핑해주는 펑션
bool RemapAlphaAndConvertTexture(LPDIRECT3DDEVICE9 device, const char* szSrcFileName, D3DFORMAT targetFormat, const char* szOutput);
