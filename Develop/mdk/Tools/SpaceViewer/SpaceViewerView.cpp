
// SpaceViewerView.cpp : CSpaceViewerView 클래스의 구현
//

#include "stdafx.h"
#include "SpaceViewer.h"

#include "SpaceViewerDoc.h"
#include "SpaceViewerView.h"
#include "Frame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSpaceViewerView

IMPLEMENT_DYNCREATE(CSpaceViewerView, CView)

BEGIN_MESSAGE_MAP(CSpaceViewerView, CView)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
END_MESSAGE_MAP()

// CSpaceViewerView 생성/소멸

CSpaceViewerView::CSpaceViewerView()
{
	// TODO: 여기에 생성 코드를 추가합니다.

}

CSpaceViewerView::~CSpaceViewerView()
{
}

BOOL CSpaceViewerView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return CView::PreCreateWindow(cs);
}

// CSpaceViewerView 그리기

void CSpaceViewerView::OnDraw(CDC* /*pDC*/)
{
	CSpaceViewerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 여기에 원시 데이터에 대한 그리기 코드를 추가합니다.
	CFrame& frame = CFrame::GetReference();
	frame.Render();
}


// CSpaceViewerView 진단

#ifdef _DEBUG
void CSpaceViewerView::AssertValid() const
{
	CView::AssertValid();
}

void CSpaceViewerView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CSpaceViewerDoc* CSpaceViewerView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSpaceViewerDoc)));
	return (CSpaceViewerDoc*)m_pDocument;
}
#endif //_DEBUG


// CSpaceViewerView 메시지 처리기

int CSpaceViewerView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.
	CDC *pDC = GetDC();

	CFrame& frame = CFrame::GetReference();
	frame.Init(pDC->m_hDC);

	return 0;
}

void CSpaceViewerView::OnDestroy()
{
	CView::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CFrame& frame = CFrame::GetReference();
	frame.Release();
}

void CSpaceViewerView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CFrame& frame = CFrame::GetReference();
	frame.OnSize(cx,cy);
}
