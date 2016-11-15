#pragma once

#include "CMList.h"
#include "MintTypes.h"
#include <string>
using namespace std;

namespace mint3 {

#define MBM_Normal 0
#define MBM_FlipLR 1
#define MBM_FlipUD 1<<1
#define MBM_RotL90 1<<2
#define MBM_RotR90 1<<3

////////////////////////////////////////////////////
// Bitmap abstract class
//		시스템에 맞는 class로 override해서 사용한다.
#define MBITMAP_NAME_LENGTH		128
class MBitmap{
public:
protected:
	char	m_szName[MBITMAP_NAME_LENGTH];
	DWORD	m_DrawMode;
public:
	MBitmap(void);
	virtual ~MBitmap(void);

	virtual bool Create(const char* szName);
	virtual void Destroy(void);

	virtual void SetDrawMode(DWORD md) { m_DrawMode = md; }
	virtual DWORD GetDrawMode() { return m_DrawMode; }

	virtual void SetTexCoord(float x1, float y1, float x2, float y2) {}
	virtual int GetWidth(void) = 0;
	virtual int GetHeight(void) = 0;
	

	const char* GetName() const { return m_szName; }
};


class MAniBitmap{
protected:
public:
	char	m_szName[MBITMAP_NAME_LENGTH];
private:
	CMLinkedList<MBitmap>	m_Bitmaps;
protected:
	int		m_nCurFrame;
	int		m_nDelay;
public:
	MAniBitmap(void);
	virtual ~MAniBitmap(void);
	bool Create(const char* szName);
	void Destroy(void);

	void Add(MBitmap* pBitmap);
	MBitmap* Get(int nFrame);
	MBitmap* Get(void);

	int GetFrameCount(void);
	int GetCurFrame(void);

	bool MoveNext(void);
	bool MovePrevious(void);
	void MoveFirst(void);
	void MoveLast(void);
	bool Move(int nFrame);

	int GetDelay(void);
	void SetDelay(int nDelay) { m_nDelay = nDelay; }
};

} // namespace mint3