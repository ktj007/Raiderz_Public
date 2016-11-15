#pragma once

class XDebugInfoBuilder
{
private:
	wstring		m_strBuf;
	void AddLine(wstring& str);
	wstring _Build_Header();
	wstring _Build_Tail();
	wstring _Build_Server();
	wstring _Build_Field();
	wstring _Build_Character();
public:
	XDebugInfoBuilder()
	{

	}
	~XDebugInfoBuilder()
	{

	}

	void Build();

	wstring& GetInfo() { return m_strBuf; }
	wstring GetInfoForClipboard();
};

/*

#접속한 서버
#현재 위치(필드, 좌표), 현재 시간, 날씨, 
#캐릭터 이름, 캐릭터 레벨 , 성별 , 캐릭터 스탠스

장착중인 장비
현재 전투중인 몬스터 정보
캐릭터 두잉 상태값, 파티 여부, 파티라면 파티 정보(멤버 목록)

해상도 , 창모드 여부 , 
*/