/************************************************************************
MAnimationTemplate
 애니메이션 빗맵 데이터만을 담고 있는 클래스
*************************************************************************/

#pragma once

#include "MResource.h"

namespace mint3
{

/*	MPLAYMODE
	애니메이션 플레이 모드를 정의
*/
enum MPLAYMODE
{
	MPM_FORWARD			= 0,				// 정방향으로 반복
	MPM_BACKWARD,							// 역방향으로 반복
	MPM_FORWARDONCE,						// 정방향으로 한번만
	MPM_BACKWARDONCE,						// 역방향으로 한번만
	MPM_FORWARDBACKWARD						// 앞뒤로 반복
};


/*	MAnimationTemplate
	Animation 템플릿 클래스
*/
class MAnimationTemplate : public MResource 
{
protected:
	MBitmap*		m_pBitmap;				// 비트맵 이미지 포인터
	int				m_nSplitCount;			// 이미지를 몇개의 씬으로 나눴는지 저장하는 변수
	vector<int>		m_Delay;				// 각각의 씬이 보여지는 시간 딜레이를 저장하는 변수

public:
	MAnimationTemplate();
	virtual ~MAnimationTemplate();

	// 씬의 딜레이를 설정하는 함수
	void SetDelay( string strDelay);

	// 지정한 씬의 딜레이를 설정하는 함수
	void SetDelay( int nSceneNum, int nDelay);

	// 전체 씬 수를 구하는 함수
	int GetSplitCount();

	MBitmap* GetBitmap();
	void SetBitmap(MBitmap* pBitmap);

	// 업데이트
	void UpdateSceneNum( MPLAYMODE ePlayMode, int& inoutSceneNum , bool& inoutbPlay, int& inoutPlayDir, DWORD& inoutTimer);
};

//////////////////////////////////////////////////////////////////////////
// inline functions

inline int MAnimationTemplate::GetSplitCount() { return m_nSplitCount; }
inline MBitmap* MAnimationTemplate::GetBitmap() { return m_pBitmap; }
inline void MAnimationTemplate::SetBitmap(MBitmap* pBitmap) { m_pBitmap = pBitmap; }

} // namespace mint3