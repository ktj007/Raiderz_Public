#include "stdafx.h"
#include "GExportDBStringTable.h"
#include "CStringResMgr.h"
#include "CLangInfo.h"

GExportDBStringTable::GExportDBStringTable()
: m_pTable(NULL)
{
	// do nothing
}

GExportDBStringTable::~GExportDBStringTable()
{
	SAFE_DELETE(m_pTable);
}

bool GExportDBStringTable::Load(wstring strLocale)
{
	// 생성
	if (m_pTable == NULL)
	{
		m_pTable = new CStringResMgr();
	}


	// 클라이언트 폴더 참조 - Runtime내 실행을 가정
	CLangNode node;
	node.strLocale = strLocale;
	node.strPath = L"..\\..\\Data\\system\\lang\\" + strLocale + L"\\";


	// SQL Export에서 참조하는 이름 파일
	vector<wstring> fieldList;
	
	fieldList.push_back(L"name_xitem.xml");
	fieldList.push_back(L"name_quest.xml");
	fieldList.push_back(L"name_talent.xml");

	
	// 읽기 - 실패하면 문자열 테이블은 공백문자열만 반환
	if (!m_pTable->Init(node, fieldList))
	{
		mlog("Failed to loading files for sql export string tables.");
		return false;
	}


	return true;
}

wstring GExportDBStringTable::GetString(wstring strKey) const
{
	if (m_pTable == NULL)	return L"";

	return m_pTable->GetString(strKey);
}
