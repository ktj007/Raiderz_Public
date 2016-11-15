//
//		Global Resource Manager
//
//		MBitmapManager, MFontManager, MCursorSystem
//		MINT에서 사용되는 Bitmap, Font, Cursor Resource를 전역적으로 관리하는 클래스들
//		Application에서 독자적으로 리소스를 관리한다면 굳이 이 클래스들을 쓸 필요는 없다.
//
#pragma once

#include "MBitmap.h"
#include "MFont.h"
#include "MCursor.h"
#include "CMList.h"

#include <list>
#include <map>
#include <string>
using namespace std;

namespace mint3 {

class MAnimationTemplate;

// Global Bitmap Management static class
class MBitmapManager{
private:
	static map<string, MBitmap*>		m_BitmapMap;
	static int							m_nNonameCounter;
public:
	static void Destroy(void);
//	static void DestroyAniBitmap(void);
	static void Add(MBitmap* pBitmap);
//	static void Add(MAniBitmap* pAniBitmap);
	static void Delete(const char* szName);
	static void Clear();
	static MBitmap* Get(const char* szName);
	static MBitmap* Get(int i);
	static int GetCount(void);
	static bool IsExist( const char* szName);
//	static MAniBitmap* GetAniBitmap(const char* szBitmapName);
	static void Update(float fDelta);
};

// Global Font Management static class
class MFontManager{
	static map<string,MFont*>	m_Fonts;
	static MFont *m_pDefaultFont;
public:
	MFontManager();

	static void Destroy(void);
	static void Add(MFont* pFont);
	static MFont* Get(const char* szFontName);
//	static MFont* Get(int i);
	static int GetCount(void);
	static void SetDefaultFont(MFont *pFont);
	static bool IsExist( const char* szName);
};

// Default Font Set
#define MF_SMALL		"small"
#define MF_MEDIUM		"medium"
#define MF_SMALL_BOLD	"smallbold"
#define MF_MEDIUM_BOLD	"mediumbold"

// 기본적으로 사용되는 커서 이름
// 최소한 이 세개의 이름을 같는 커서는 정의 되어져야 한다.
#define MCURSOR_ARROW	"arrow"
#define MCURSOR_WAIT	"wait"
#define MCURSOR_INPUT	"input"

// Cursor System static class
class MCursorSystem{
	static MCursor*					m_pCursor;		// 현재 커서
	static map<string, MCursor*>	m_mapCursors;	// 내부에 관리되는 커서
	static bool						m_bShow;
public:
	static void Destroy(void);
	static MCursor* Set(MCursor* pCursor);
	static MCursor* Set(const char* szName);
	static MCursor* Get(void);
	static MCursor* Get(const char* szName);
	static void Add(MCursor* pCursor);
	static void Draw(MDrawContext* pDC, int x, int y);
	static void Show(bool bShow);
	static bool IsVisible(void);
	static bool IsExist( const char* szName);
};

template <class _TYPE>
class MResourceManager {
	map<string, _TYPE*> m_mapResources;

public:
	void Destroy(void)
	{
		while(!m_mapResources.empty())
		{
			delete (*m_mapResources.begin()).second;
			m_mapResources.erase(m_mapResources.begin());
		}
		m_mapResources.clear();
	}

	bool Add( _TYPE* pResource)
	{
		if( Get( pResource->GetName() ) !=NULL )
			return false;

		m_mapResources.insert( map<string, _TYPE*>::value_type( pResource->GetName(), pResource));
		return true;
	}

	_TYPE* Get( const string& strName )
	{
		map<string, _TYPE*>::iterator itr = m_mapResources.find(strName);
		if(itr==m_mapResources.end())
			return NULL;

		return itr->second;
	}

	void Delete ( const string& strName )
	{
		map<string, _TYPE*>::iterator itr = m_mapResources.find(strName);
		if(itr==m_mapResources.end())
			return NULL;
		delete itr->second;
		m_mapResources.erase(itr);
	}
};

class MAnimationTemplateManager : public MResourceManager<MAnimationTemplate> {
};

} // namespace mint3