#include "stdafx.h"
#include "GSQLExporter.h"
#include "GGlobal.h"
#include "GDBManager.h"



GSQLExporter::GSQLExporter()
{

}

GSQLExporter::~GSQLExporter()
{

}

bool GSQLExporter::ExportToDB( const std::vector<std::wstring>& vSQL )
{
	if (NULL == gsys.pDBManager)
		return false;

	for (size_t i = 0; i < vSQL.size(); ++i)
	{
		gsys.pDBManager->Execute(vSQL[i].c_str());
	}

	return true;
}

bool GSQLExporter::ExportToFile( const wstring& strFile, const std::vector<std::wstring>& vSQL )
{
	FILE* pFile;

	// 유니코드 쓰기 시, 로케일 캐릭터셋에 없는 문자를 표시하지 못하는 문제를 피하기 위해 이진모드로 씁니다.
	if (0 != _wfopen_s(&pFile, strFile.c_str(), L"wb"))
	{		
		return false;
	}


	// 유니코드 BOM 삽입
	BYTE bom[2] = {0xFF, 0xFE};		// UTF-16, little-endian	
	fwrite(bom, sizeof(BYTE), 2, pFile);	


	// 본문 쓰기
	for (size_t i = 0; i < vSQL.size(); ++i)
	{
		// 텍스트 모드에서 지원하는 \n -> \r\n 자동변경을 이진모드는 지원하지 않기때문에 수동변환한다.
		CString str = vSQL[i].c_str();
		str.Replace(L"\n", L"\r\n");

		size_t nWriteLenth = fwrite(str.GetString(), sizeof(wchar_t), str.GetLength(), pFile);

		if (str.GetLength() != nWriteLenth)
		{
			fclose(pFile);
			return false;
		}
	}

	fclose(pFile);
	return true;
}
