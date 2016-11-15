//
//		Cursor
//
//		Cursor, BitmapCursor, Animation Cursor
//
#pragma once

#include "MResource.h"

namespace mint3 {

class MBitmap;
class MAniBitmap;
class MAnimationTemplate;
class MDrawContext;

enum MCURSORTYPE {
	MCT_NONE,
	MCT_BITMAP,
	MCT_ANIBITMAP
};

// Abstract Cursor class
class MCursor : public MResource {
public:
	MCursor(const char* szName);
	virtual ~MCursor(void){}

	void	SetBitmap(MBitmap* pBitmap);
	void	SetAnimationTemplate(MAnimationTemplate* pAnimationTemplate);

protected:
	friend class MCursorSystem;

	MBitmap*			m_pBitmap;
	MAnimationTemplate*	m_pAnimationTemplate;

	// 애니메이션을 위한 변수
	int					m_nSceneNum;			// 현재 씬 번호
	DWORD				m_dwTimer;				// 타이머 변수

	void Draw(MDrawContext* pDC, int x, int y);

	virtual void OnSet() {}
	virtual void OnDraw(MDrawContext* pDC, int x, int y) {}
	virtual void OnSetBitmap(MBitmap* pBitmap) {}
	virtual void OnSetAnimationTemplate(MAnimationTemplate* pAnimationTemplate) {}
};

// Bitmap Cursor class
class MDefaultCursor : public MCursor{
protected:
	virtual void OnDraw(MDrawContext* pDC, int x, int y);
public:
	MDefaultCursor(const char* szName);
};

/*
// Bitmap Cursor class
class MBitmapCursor : public MCursor{
	MBitmap*	m_pBitmap;
protected:
	virtual void Draw(MDrawContext* pDC, int x, int y);
public:
	MBitmapCursor(const char* szName, MBitmap* pBitmap);
};

// Animation Bitmap Cursor class
class MAniBitmapCursor : public MCursor{
	MAnimation*	m_pAnimation;
protected:
	virtual void Draw(MDrawContext* pDC, int x, int y);
public:
	MAniBitmapCursor(const char* szName, MAniBitmap* pAniBitmap);
	virtual ~MAniBitmapCursor(void);
};
*/

} // namespace mint3