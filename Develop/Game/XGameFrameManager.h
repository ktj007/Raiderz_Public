#pragma once

#include "XGameFrame.h"

/// 게임 프레임 관리자
class XGameFrameManager
{
protected:
	map<int, XBaseFrame*>		m_FrameMap;
	int							m_nNextFrame;
	int							m_nPrevFrame;
	bool						m_bReserved;
	XBaseFrame*					m_pCurFrame;

	bool						PerformChange( int nFrameID, const wchar_t* szArgs =NULL);
public:
	/// 생성자
	XGameFrameManager();
	/// 파괴자
	virtual						~XGameFrameManager();
	/// 게임 프레임 리스트 로드
	void						Load(const wchar_t* szFileName);		// 리팩토링 대상
	/// 메세지 처리
	bool						OnEvent(MEvent& e);
	/// 게임 프레임을 추가
	void						Add(XBaseFrame* pGameFrame);
	/// 다른 게임프레임으로 이동
	void						Change(int nFrameID);
	/// 맨 처음 게임프레임 진입.
	/// 프레임 제일 처음 시작할때에만 한번 실행한다.
	bool						Start(int nFrameID, const wchar_t* szArgs=NULL);
	/// 모든 게임프레임을 삭제한다. 메모리해체까지 해줌.
	void						DeleteAll();
	/// 업데이트
	void						Update(float fDelta);
	/// 렌더 준비
	void						OnPreRender();
	/// 렌더
	void						OnRender();
	/// Flip전
	void						OnPreFlip();
	/// 활성화, 비활성화 시
	void						OnActive(bool bActive);
	/// 현재 프레임에게 메세지 보냄
	void						Message(const wchar_t* szMsg);
	/// 현재 프레임 ID 반환
	int							GetCurrFrameID();
	/// 이전 프레임 ID 반환
	int							GetPrevFrameID();
	/// 프래임 반환
	XBaseFrame*					GetFrame( int nFrameID);
	/// 프레임 이름으로 프레임 반환
	XBaseFrame*					GetFrameFromName(const wchar_t* szFrameName);
	/// 현재 프레임 반환
	XBaseFrame*					GetCurrFrame()	{ return m_pCurFrame; }
};
