#ifndef _XFRAME_H
#define _XFRAME_H

#include <map>
#include <string>
#include "MEvent.h"

using namespace std;
using namespace mint3;


/// 게임 프레임 기본클래스
class XGameFrameManger;
class XBaseFrame
{
protected:
	friend class XGameFrameManager;
	XGameFrameManager* m_pFrameManager;
public:
						XBaseFrame() : m_pFrameManager(NULL) {}			///< 생성자
	virtual				~XBaseFrame()			{}						///< 파괴자
	virtual void		Update(float fDelta);							///< 업데이트
	virtual void		OnPreRender();									///< 렌더 이전
	virtual void		OnRender();										///< 화면에 그리기
	virtual void		OnPreFlip();									///< Flip 이전
	virtual bool		OnEvent(MEvent& e);								///< 이벤트 처리
	virtual bool		Open( int nPrevFrameID, const wchar_t* szArgs)	{ return true; }		///< 게임프레임 진입시 필요한 사항을 넣도록 한다.
	virtual void		Close( int nNextFrameID) {}						///< 게임프레임 해제시 필요한 사항을 넣도록 한다.
	virtual void		OnActive(bool bActive)	{}						///< 활성화시, 비활성화시
	virtual void		Message(const wchar_t* args) {}					///< 프레임에 메세지 보냄
	virtual void		OnPreLoading()		{}							///< 로딩 시작 이전
	virtual void		OnLoading()			{}							///< 로딩 시작
	virtual void		OnPaused()			{}							///< 로딩 완료 후 정지
	virtual void		OnLoaded()			{}							///< 로딩 종료
	virtual bool		CheckCloseState()		{ return true;}			///< 닫을 수 있는 상태인가?
	virtual int			GetID() = 0;									///< 프레임 ID
	virtual const wchar_t* GetName()				{ return L""; }			///< 프레임 이름, 테스트 프레임인 경우 실행 파일 뒤에 인자로 이 이름을 넣어주면 그 프레임으로 바로 실행한다.
};



#endif