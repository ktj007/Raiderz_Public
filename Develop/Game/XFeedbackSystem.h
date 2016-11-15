#pragma once

class XFeedbackFetcher;
class XFeedbackInfoMgr;

/// 피드백 관련 Facade
class XFeedbackSystem
{
protected:
	XFeedbackInfoMgr*		m_pFeedbackInfoMgr;
	XFeedbackFetcher*		m_pFeedbackFetcher;
	float					m_fUpdateObjectsElapsedTime;
	float					m_fUpdateFetchElapsedTime;

	void MakeFeedbackObjects();
	void UpdateFeedbackObjects(float fDelta);
	void UpdateFetch(float fDelta);
public:
	XFeedbackSystem();
	virtual ~XFeedbackSystem();

	void Update(float fDelta);

	/// 웹으로 피드백 xml 정보를 가져온다.
	void Fetch();

	void SendFeedback(const wchar_t* szText);

	void ReloadFeedbackFromWeb();
	void FetchFeedbackWeb();
	void SetShowFeedback( bool bShow );
};