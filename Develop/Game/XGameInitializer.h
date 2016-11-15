#pragma once

class XGlobalInfoInstance;
class XGlobalInstance;

class XGameInitializer
{
protected:
	bool	m_bInited_Pre_CreatingWindow;
	bool	m_bInited_Post_CreatingWindow;
	bool	m_bInited_While_LoadingLoginCampaign;
	bool	m_bInited_Pre_LoadingLoginCampaign;

	void InitConfig(XGlobalInfoInstance* pGlobalInfoInstance);
	void ReadCountryLanguages(XGlobalInfoInstance* pGlobalInfoInstance);
	void InitRS3();
	bool InitResources(XGlobalInstance* pGlobalInstance);
public:
	XGameInitializer();
	/// 윈도우 생성되기 전에 초기화
	bool Init_Pre_CreatingWindow(XGlobalInfoInstance* pGlobalInfoInstance, XGlobalInstance* pGlobalInstance);

	/// 윈도우 생성된 후에 초기화
	bool Init_Post_CreatingWindow(XBaseApplication* pApp, XGlobalInfoInstance* pGlobalInfoInstance, XGlobalInstance* pGlobalInstance);

	/// 캐릭터 선택 하기 전에 초기화
	bool Init_Pre_LoadingLoginCampaign();

	/// 캐릭터 선택 하기 전에 로딩 화면에서 초기화
	bool Init_While_LoadingLoginCampaign();

#ifndef _PUBLISH
	/// 국가 언어 텍스트 다시 읽기
	void ReloadCountryLanguages(XGlobalInfoInstance* pGlobalInfoInstance)
	{
		ReadCountryLanguages( pGlobalInfoInstance);
	}
#endif
};

