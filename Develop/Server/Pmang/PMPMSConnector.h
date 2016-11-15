#pragma once

#include "PmLib.h"
#include "MTstring.h"

interface IPMSObject;
class PmRegionStat;

class PMANG_API PMPMSConnector
{
protected:
	class TokenPszConverter
	{
	public:
		TokenPszConverter(const wstring strCommandLine);
		~TokenPszConverter();

		DWORD Argc() const;
		LPSTR* Argv() const;
	private:
		vector<wstring> m_vecTokens;
		LPSTR* pszTokens;
	};

public:
	PMPMSConnector();
	~PMPMSConnector();

	// 게임정보 설정
	void SetGameInfo(DWORD dwSSN, DWORD dwCategory);
	// PMS 연결, 반환값은 PMSCONN_ERRORCODE 참조
	DWORD	 Connect( DWORD argc, LPSTR argv[], IPMSObject* pCallbackObj );
	// PMS 연결, 반환값은 PMSCONN_ERRORCODE 참조
	DWORD	Connect( const wstring strCommandLine );
	// PMS 연결끊기
	void	Stop();
	// PMS로 Warning 메시지를 보내기
	BOOL	SendWarningMsg(DWORD dwErrLvl, LPCTSTR pszWarningMsg, LPCTSTR pszTreatMsg) const;
	// PMS로 Warning 메시지를 보내기
	void	SendWarningMsg() const;
	// PMS로 넘겨진 성능 정보 Config 파일의 이름 반환
	LPCSTR	GetConfigFileName();
	// PMSConn의 현재 상태를 가져 오기 위한 함수, 반환값은 PMSCONN_STATUS_CODE 참조
	DWORD	GetStatus();
	// 서비스 번호 반환
	DWORD	GetSSN() const;
	// 게임 카테고리 반환
	DWORD	GetCategory() const;
	// 지역 정보 반환
	PmRegionStat* GetRegionStat() const;
protected:
	// PMS 콜백 오브젝트 구하기
	virtual IPMSObject* GetPMSObject() = 0;
private:
	// 연결 콜백
	void OnConnected();
	// 연경끊기 콜백
	void OnStopped();
private:
	bool		m_bConnected;
	DWORD		m_dwSSN;
	DWORD		m_dwCategory;
	PmRegionStat* m_pRegionStat;
};
