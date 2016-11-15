
// SpaceViewerDoc.cpp : CSpaceViewerDoc 클래스의 구현
//

#include "stdafx.h"
#include "SpaceViewer.h"

#include "SpaceViewerDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSpaceViewerDoc

IMPLEMENT_DYNCREATE(CSpaceViewerDoc, CDocument)

BEGIN_MESSAGE_MAP(CSpaceViewerDoc, CDocument)
END_MESSAGE_MAP()


// CSpaceViewerDoc 생성/소멸

CSpaceViewerDoc::CSpaceViewerDoc()
{
	// TODO: 여기에 일회성 생성 코드를 추가합니다.

}

CSpaceViewerDoc::~CSpaceViewerDoc()
{
}

BOOL CSpaceViewerDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 여기에 재초기화 코드를 추가합니다.
	// SDI 문서는 이 문서를 다시 사용합니다.

	return TRUE;
}




// CSpaceViewerDoc serialization

void CSpaceViewerDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 여기에 저장 코드를 추가합니다.
	}
	else
	{
		// TODO: 여기에 로딩 코드를 추가합니다.
	}
}


// CSpaceViewerDoc 진단

#ifdef _DEBUG
void CSpaceViewerDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CSpaceViewerDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CSpaceViewerDoc 명령
