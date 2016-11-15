// EffectToolDoc.cpp : CEffectToolDoc 클래스의 구현
//

#include "stdafx.h"
#include "EffectTool.h"

#include "EffectToolDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CEffectToolDoc

IMPLEMENT_DYNCREATE(CEffectToolDoc, CDocument)

BEGIN_MESSAGE_MAP(CEffectToolDoc, CDocument)
END_MESSAGE_MAP()


// CEffectToolDoc 생성/소멸

CEffectToolDoc::CEffectToolDoc()
{
	// TODO: 여기에 일회성 생성 코드를 추가합니다.

}

CEffectToolDoc::~CEffectToolDoc()
{
}

BOOL CEffectToolDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 여기에 다시 초기화 코드를 추가합니다.
	// SDI 문서는 이 문서를 다시 사용합니다.

	return TRUE;
}




// CEffectToolDoc serialization

void CEffectToolDoc::Serialize(CArchive& ar)
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


// CEffectToolDoc 진단

#ifdef _DEBUG
void CEffectToolDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CEffectToolDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CEffectToolDoc 명령
