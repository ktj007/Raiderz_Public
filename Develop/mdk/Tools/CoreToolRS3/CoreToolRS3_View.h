// SDIdocView.h : CCoreToolRS3View 클래스의 인터페이스
//


#pragma once

class CControlMediator;
class CCoreToolRS3Doc;
class CCoreToolRS3View : public CView
{
protected: // serialization에서만 만들어집니다.
	CCoreToolRS3View();
	DECLARE_DYNCREATE(CCoreToolRS3View)

// 특성입니다.
public:
	CCoreToolRS3Doc* GetDocument() const;

// 구현입니다.
public:
	virtual ~CCoreToolRS3View();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	virtual void OnDraw(CDC* pDC);  // 이 뷰를 그리기 위해 재정의되었습니다.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct); ///< 생성
	afx_msg void OnMouseMove(UINT nFlags, CPoint point); ///< 마우스 처리
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt); ///< 마우스 처리
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point); ///< 마우스 처리
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);   ///< 마우스 처리
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point); ///< 마우스 처리
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);   ///< 마우스 처리
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point); ///< 마우스 처리
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);   ///< 마우스 처리
	afx_msg void OnSize(UINT nType, int cx, int cy); ///< 리사이즈 처리
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnDropFiles(HDROP hDropInfo);

	DECLARE_MESSAGE_MAP()

public:
	void SetControlMediator(CControlMediator* _pControlMediator){m_pControlMediator_ = _pControlMediator;}
	void SetUserTimer(DWORD dwElpasedTime);
	void KillUserTimer();
	void OnUserTimer();	///<메인 렌더링

private:
	struct MOUSE_ACT_EVENT {
		static const UINT INIT =	0x00000;
		static const UINT MOVE =	0x10000;
		static const UINT M_WHEEL = 0x20000;

		static const UINT L_BN =	0x01000;
		static const UINT R_BN =	0x02000;
		static const UINT M_BN =	0x04000;

		static const UINT BN_DOWN =	0x00001;
		static const UINT BN_UP	=	0x00002;
	};

	CControlMediator* m_pControlMediator_;
	struct MOUSE_BUFFER {
		MOUSE_BUFFER() : nEvent(MOUSE_ACT_EVENT::INIT){}
		UINT nFlags; ///< MFC 마우스 플래그
		UINT nEvent; ///< 마우스 액션 종류
		LONG x;      ///< 마우스 x좌표
		LONG y;      ///< 마우스 y좌표
		LONG zDelta; ///< 마우스 휠
	} m_eventedMouseBuffer_, m_previousMouseBuffer_;
	
	// 타이머 플래그
	bool m_bTimer;
	DWORD m_dwTimerElpasedTime;
	
	//워크벤치의 크기
	UINT m_wbWidth_;
	UINT m_wbHeight_;

	//마우스 상태 결정 함수
	void WB_DecideMouseState_();
};

#ifndef _DEBUG  // SDIdocView.cpp의 디버그 버전
inline CCoreToolRS3Doc* CCoreToolRS3View::GetDocument() const
   { return reinterpret_cast<CCoreToolRS3Doc*>(m_pDocument); }
#endif
