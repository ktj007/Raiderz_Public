#include "stdafx.h"
#include "MResourceManager.h"

namespace mint3 {

// MBitmapManager Implementation
//////////////////////////////
map<string, MBitmap*>		MBitmapManager::m_BitmapMap;
int							MBitmapManager::m_nNonameCounter=0;

#define MINT_BITMAP_NONAME_STR		"_noname"

void MBitmapManager::Destroy(void)
{
	Clear();
}

//void MBitmapManager::DestroyAniBitmap(void)
//{
//	for(int i=0; i<m_AniBitmaps.GetCount(); i++){
//		MAniBitmap* pAniBitmap = m_AniBitmaps.Get(i);
//		pAniBitmap->Destroy();
//	}
//	m_AniBitmaps.DeleteAll();
//}

void MBitmapManager::Add(MBitmap* pBitmap)
{
	string strName = pBitmap->GetName();

	if ( strName.empty() == true)
	{
		char szBuff[ 512];
		sprintf( szBuff, "%s%d", MINT_BITMAP_NONAME_STR, m_nNonameCounter++);	// 0부터 시작
		strName = szBuff;
	}

	m_BitmapMap.insert(map<string,MBitmap*>::value_type( strName, pBitmap));
}

//void MBitmapManager::Add(MAniBitmap* pAniBitmap)
//{
//	m_AniBitmaps.Add(pAniBitmap);
//}

void MBitmapManager::Delete(const char* szName)
{
	map<string, MBitmap*>::iterator itor = m_BitmapMap.find(string(szName));
	if (itor != m_BitmapMap.end())
	{
		delete (*itor).second;
		(*itor).second = NULL;
		m_BitmapMap.erase(itor);
		return;
	}
}

void MBitmapManager::Clear()
{
	for (map<string, MBitmap*>::iterator itor = m_BitmapMap.begin(); itor != m_BitmapMap.end(); ++itor)
	{
		(*itor).second->Destroy();
		// 삭제
		delete (*itor).second;
		(*itor).second = NULL;
	}
	m_BitmapMap.clear();
	m_nNonameCounter = 0;
}

MBitmap* MBitmapManager::Get(const char* szName)
{
	if (szName==NULL)
	{
		return NULL;
//		if (!m_BitmapMap.empty())
//		{
//			return (*m_BitmapMap.begin()).second;
//		}
	}

	map<string, MBitmap*>::iterator itor = m_BitmapMap.find(string(szName));
	if (itor != m_BitmapMap.end())
	{
		MBitmap* pBitmap = (*itor).second;
		if (pBitmap->IsCaching() && 
			pBitmap->GetTime() == INT_MAX)
		{
			if (pBitmap->CreateTexture())
			{
			}
		}

		return pBitmap;
	}

	return NULL;
}

MBitmap* MBitmapManager::Get(int i)
{
	char name[64];
	sprintf(name, "%s%d", MINT_BITMAP_NONAME_STR, i);	// 0부터 시작
	return Get(name);
}

int MBitmapManager::GetCount(void)
{
	return (int)m_BitmapMap.size();
}

bool MBitmapManager::IsExist( const char* szName)
{
	map<string, MBitmap*>::iterator itr = m_BitmapMap.find( string( szName));
	if ( itr == m_BitmapMap.end())		return false;
	return true;
}

void MBitmapManager::Update( float fDelta )
{
	map<string, MBitmap*>::iterator itor = m_BitmapMap.begin();
	for (;itor != m_BitmapMap.end(); ++itor)
	{
		MBitmap* pBitmap = (*itor).second;
		if (!pBitmap->IsCaching()) continue;
		if (pBitmap->GetTime() == INT_MAX) continue;

		//if ( timeGetTime() - pBitmap->GetTime() > 5000 )
		if ( timeGetTime() - pBitmap->GetTime() > 60000 )	//1분
		{
			pBitmap->Destroy();
		}
	}
}


//MAniBitmap* MBitmapManager::GetAniBitmap(const char* szBitmapName)
//{
//	if(szBitmapName==NULL)	// Default Font
//		if(m_AniBitmaps.GetCount()>0)
//			return m_AniBitmaps.Get(0);
//
//	for(int i=0; i<m_AniBitmaps.GetCount(); i++){
//		MAniBitmap* pBitmap = m_AniBitmaps.Get(i);
//		if(strcmp(pBitmap->m_szName, szBitmapName)==0) return pBitmap;
//	}
//
//	return NULL;
//}


// MFontManager Implementation
//////////////////////////////
//CMLinkedList<MFont> MFontManager::m_Fonts;
map<string,MFont*>	MFontManager::m_Fonts;
MFont *MFontManager::m_pDefaultFont=NULL;

void MFontManager::SetDefaultFont(MFont *pFont)
{
	m_pDefaultFont=pFont;
}

void MFontManager::Destroy(void)
{
	/*
	for(int i=0; i<m_Fonts.GetCount(); i++){
		MFont* pFont = m_Fonts.Get(i);
		pFont->Destroy();
	}
	m_Fonts.DeleteAll();
	*/

	while(!m_Fonts.empty())
	{
		delete m_Fonts.begin()->second;
		m_Fonts.erase(m_Fonts.begin());
	}
}

void MFontManager::Add(MFont* pFont)
{
//	m_Fonts.Add(pFont);

	map<string,MFont*>::iterator i=m_Fonts.find( pFont->GetName());
	if(i!=m_Fonts.end())
	{
		delete i->second;
		m_Fonts.erase(i);
	}

	m_Fonts.insert(map<string,MFont*>::value_type( pFont->GetName(), pFont));
}

MFont* MFontManager::Get(const char* szFontName)
{
	if(szFontName)
	{
		map<string,MFont*>::iterator i=m_Fonts.find(string(szFontName));
		if(i!=m_Fonts.end())
		{
			return i->second;
		}
	}

	if(m_pDefaultFont) return m_pDefaultFont;

	return NULL;

	/*
	if(szFontName==NULL)	// Default Font
		if(m_Fonts.GetCount()>0)
			return m_Fonts.Get(0);

	for(int i=0; i<m_Fonts.GetCount(); i++){
		MFont* pFont = m_Fonts.Get(i);
		if(strcmp(pFont->m_szName, szFontName)==0) return pFont;
	}

	return NULL;
	*/
}

/*
MFont* MFontManager::Get(int i)
{
	if(i<0 || i>=m_Fonts.GetCount()) return NULL;
	return m_Fonts.Get(i);
}
*/

int MFontManager::GetCount(void)
{
//	return m_Fonts.GetCount();
	return m_Fonts.size();
}

bool MFontManager::IsExist( const char* szName)
{
	map<string, MFont*>::iterator itr = m_Fonts.find( string( szName));
	if ( itr == m_Fonts.end())		return false;
	return true;
}


MCursor* MCursorSystem::m_pCursor;
//CMLinkedList<MCursor> MCursorSystem::m_Cursors;
map<string, MCursor*>	MCursorSystem::m_mapCursors;	// 내부에 관리되는 커서
bool MCursorSystem::m_bShow = true;

void MCursorSystem::Destroy(void)
{
//	m_Cursors.DeleteAll();
	while(!m_mapCursors.empty())
	{
		delete (*m_mapCursors.begin()).second;
		m_mapCursors.erase(m_mapCursors.begin());
	}
	m_mapCursors.clear();
	m_pCursor=NULL;
}

MCursor* MCursorSystem::Set(MCursor* pCursor)
{
	if ( m_pCursor == pCursor)		return m_pCursor;

	pCursor->OnSet();
	MCursor* temp = m_pCursor;
	m_pCursor = pCursor;
	return temp;
}

MCursor* MCursorSystem::Set(const char* szName)
{
	if(szName==NULL){
		//...
	}
	MCursor* pCursor = Get(szName);
	if(pCursor!=NULL) return Set(pCursor);
	return NULL;
}

MCursor* MCursorSystem::Get(void)
{
	return m_pCursor;
}

MCursor* MCursorSystem::Get(const char* szName)
{
// 	for(int i=0; i<m_Cursors.GetCount(); i++){
// 		MCursor* pCursor = m_Cursors.Get(i);
// 		if(strcmp(pCursor->m_szName, szName)==0) return pCursor;
// 	}
	map<string, MCursor*>::iterator itr = m_mapCursors.find(szName);
	if(itr==m_mapCursors.end())
		return NULL;

	return itr->second;
}

void MCursorSystem::Add(MCursor* pCursor)
{
//	m_Cursors.Add(pCursor);
	m_mapCursors.insert( map<string, MCursor*>::value_type( pCursor->GetName(), pCursor));
}

void MCursorSystem::Draw(MDrawContext* pDC, int x, int y)
{
	if(m_bShow==false) return;
	if(m_pCursor!=NULL) m_pCursor->Draw(pDC, x, y);
}

void MCursorSystem::Show(bool bShow)
{
	m_bShow = bShow;
}

bool MCursorSystem::IsVisible(void)
{
	return m_bShow;
}

bool MCursorSystem::IsExist( const char* szName)
{
	map<string, MCursor*>::iterator itr = m_mapCursors.find( string( szName));
	if ( itr == m_mapCursors.end())		return false;
	return true;
}

} // namespace mint3