
// SpaceViewerView.h : CSpaceViewerView 클래스의 인터페이스
//


#pragma once


class CSpaceViewerView : public CView
{
protected: // serialization에서만 만들어집니다.
	CSpaceViewerView();
	DECLARE_DYNCREATE(CSpaceViewerView)

// 특성입니다.
public:
	CSpaceViewerDoc* GetDocument() const;

// 작업입니다.
public:

// 재정의입니다.
public:
	virtual void OnDraw(CDC* pDC);  // 이 뷰를 그리기 위해 재정의되었습니다.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// 구현입니다.
public:
	virtual ~CSpaceViewerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 생성된 메시지 맵 함수
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
};

#ifndef _DEBUG  // SpaceViewerView.cpp의 디버그 버전
inline CSpaceViewerDoc* CSpaceViewerView::GetDocument() const
   { return reinterpret_cast<CSpaceViewerDoc*>(m_pDocument); }
#endif

