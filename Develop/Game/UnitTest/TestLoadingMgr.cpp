#include "stdafx.h"
#include "XLoadingMgr.h"

SUITE(LoadingMgr)
{
	class XLoadingChecker_Mock : public XLoadingChecker
	{
	private:
		bool m_bLoaded;
		bool m_bCompleted;

		bool m_bGetEvent;
	protected:
		virtual void OnEvent(XEvent& evt)
		{
			m_bGetEvent = true;
		}
	public:
		XLoadingChecker_Mock() : m_bCompleted(false), m_bLoaded(false), m_bGetEvent(false) {}
		virtual ~XLoadingChecker_Mock() {}
		void SetCompleted(bool bComeleted) { m_bCompleted = bComeleted; }
		virtual bool IsCompleted() { return m_bCompleted; }
		void SetWorldLoaded(bool b) { m_bLoaded = b; }
		virtual bool IsWorldLoaded() { return m_bLoaded; }
		bool IsGetEvent() { return m_bGetEvent; }
	};

	TEST(TestLoadingMgr_CheckCompleted)
	{
		XLoadingMgr loadingMgr;

		XLoadingChecker_Mock* pMockLoadingChecker1 = new XLoadingChecker_Mock();
		XLoadingChecker_Mock* pMockLoadingChecker2 = new XLoadingChecker_Mock();

		loadingMgr.AddChecker(pMockLoadingChecker1);
		loadingMgr.AddChecker(pMockLoadingChecker2);

		CHECK_EQUAL(XLoadingMgr::STATE_READY, loadingMgr.GetState());

		loadingMgr.Start();

		CHECK_EQUAL(false, loadingMgr.CheckCompleted());
		CHECK_EQUAL(XLoadingMgr::STATE_STARTED, loadingMgr.GetState());


		// world loading
		pMockLoadingChecker1->SetWorldLoaded(true);
		pMockLoadingChecker2->SetWorldLoaded(true);
		CHECK_EQUAL(false, loadingMgr.CheckCompleted());
		CHECK_EQUAL(XLoadingMgr::STATE_WORLD_LOADED, loadingMgr.GetState());



		pMockLoadingChecker1->SetCompleted(true);

		// 모든 체커가 완료되어야 한다.
		CHECK_EQUAL(false, loadingMgr.CheckCompleted());


		pMockLoadingChecker2->SetCompleted(true);
		CHECK_EQUAL(true, loadingMgr.CheckCompleted());
		CHECK_EQUAL(XLoadingMgr::STATE_COMPLETED, loadingMgr.GetState());
	}

	TEST(TestLoadingMgr_Event)
	{
		XLoadingMgr loadingMgr;

		XLoadingChecker_Mock* pMockLoadingChecker = new XLoadingChecker_Mock();
		loadingMgr.AddChecker(pMockLoadingChecker);


		CHECK_EQUAL(false, pMockLoadingChecker->IsGetEvent());
	}
}