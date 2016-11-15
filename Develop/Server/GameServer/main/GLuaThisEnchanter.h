#pragma once

#include <fstream>
#include <deque>

// 스크립트 문자열에 추가적인 처리를 하는 클래스
class GLuaThisEnchanter
{
public:
	// 변환될 멤버함수명 추가
	void AddMatchMemFunc(string strName);

	// this 키워드를 함수 맨처음 인자로 넣음
	bool ConvertFromFile(string strFilename, string& outstrBuffer);

	// this 키워드를 함수 맨처음 인자로 넣음
	bool ConvertFromStream(string strStream, string& outstrBuffer);

	// this 키워드를 함수 맨처음 인자로 넣음
	string ConvertLine(string strLine);

	// 문자열을 파일로 저장
	bool SaveToFile(string strFileName, const string& strText);

private:
	bool IsMatchMemFunc(string strLine);
	bool IsDeclFuncLine(string strLine);
	bool HasComma(string strLine);
	bool HasColon(string strLine);
private:
	// 변환될 멤버함수명 모음
	vector<string>	m_vecMatchMemFuncs;
};
