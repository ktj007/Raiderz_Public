#pragma once

class XCrossHair
{
private:
	struct CROSSHAIR_LIST
	{
		wstring				m_strCrossHairListName;
		vector<wstring>		m_vecCrossHairList;
	};

private:
	vector<CROSSHAIR_LIST>	m_vecCrossHairList;

	bool					m_bAwaysCrossHair;

private:
	bool					CreateCrossHairImage(wstring& strName, wstring& strFileName);
	
	bool					SetCrossHairImage(wstring& strCrossHairName);

	void					GetCustomCrossHairList(vector<CROSSHAIR_LIST>& vecList);
	void					GetCustomCrossHairListData(wstring strPath, vector<CROSSHAIR_LIST>* pvecList, vector<wstring>* pvecListData);

	bool					CheckExistCrossHairFile(wstring strListName, wstring strFileName);

public:
	XCrossHair() {} 
	virtual ~XCrossHair() {}

	void					Init();
	
	int						GetCrossHairCount();
	wstring					GetCrossHairName(int nIndex);
	wstring					GetCurrentCrossHairName();
	wstring					GetCurrentCrossHairPickName();
	wstring					GetCurrentCrossHairAwaysName();

	bool					StartCrossHair_Aways(vec2 vPos);
	bool					StartCrossHair(vec2 vPos);
	bool					StartCrossHair_Pick(vec2 vPos);
	void					EndCrossHair();
};