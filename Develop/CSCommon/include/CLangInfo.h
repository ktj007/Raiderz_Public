#pragma once

struct CLangNode
{
	wstring strName;		// 언어명
	wstring strCountry;		// 국가명
	wstring strLocale;		// 로케일명
	wstring strPath;		// 경로
};

class CLangInfo: public vector<CLangNode>
{
public:
	CLangInfo();
	virtual ~CLangInfo();

	bool Load(wstring strFileName);
	wstring GetLocale(void) const;

private:
	wstring m_strLocale;
};
