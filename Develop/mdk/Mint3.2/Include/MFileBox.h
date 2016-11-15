#pragma once

#include "MListBox.h"

#ifdef WIN32
	#pragma comment(lib, "Shlwapi.lib")
#endif

namespace mint3 {

/// 파일 리스트 박스
class MFileBox : public MListBox{
	char	m_szFilter[256];
	char	m_szBaseDir[256];
public:
	/// @param	szFilter	파일 필터 ( 예: *.exe )
	MFileBox(const char* szFilter, MWidget* pParent=NULL, MListener* pListener=NULL);
	virtual ~MFileBox(void);

	/// 필터값으로 목록 리프레쉬
	/// @param	szFilter	파일 필터 ( 예: *.exe )
	void Refresh(const char* szFilter=NULL);

	/// 파일리스팅의 베이스 디렉토리
	const char* GetBaseDir(void);

#define MINT_FILEBOX	"FileBox"
	virtual const char* GetClassName(void){ return MINT_FILEBOX; }
};


} // namespace mint3