// V_EditEntityMaterial.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "VR_EntityMaterial.h"


// CRollupEntityMaterial 대화 상자입니다.

IMPLEMENT_DYNAMIC(CRollupEntityMaterial, CDialog)

CRollupEntityMaterial::CRollupEntityMaterial(CWnd* pParent /*=NULL*/)
	: CDialog(CRollupEntityMaterial::IDD, pParent)
{

}

CRollupEntityMaterial::~CRollupEntityMaterial()
{
}

void CRollupEntityMaterial::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CRollupEntityMaterial, CDialog)
END_MESSAGE_MAP()


// CRollupEntityMaterial 메시지 처리기입니다.

BOOL CRollupEntityMaterial::OnInitDialog()
{
	CDialog::OnInitDialog();
	return TRUE;
}

void CRollupEntityMaterial::Listen(CoreMessage& _message)
{

}