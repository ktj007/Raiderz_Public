#include "stdafx.h"
#include "XResourceRefMgr.h"
#include "RResourceID.h"

SUITE(ResourceRefMgr)
{
	class FResourceRefMgr
	{
	public:
		FResourceRefMgr()
		{
			m_Mgr.Create();
		}
		~FResourceRefMgr()
		{
			m_Mgr.Destroy(NULL);
		}
		XResourceRefMgr m_Mgr;
	};

	TEST(TestResourceRefMgr_CreateAndDestroy)
	{
		XResourceRefMgr mgr;
		mgr.Create();

		CHECK_EQUAL(true, mgr.IsCreated());

		mgr.Destroy(NULL);

		CHECK_EQUAL(false, mgr.IsCreated());
	}

	TEST_FIXTURE(FResourceRefMgr, TestResourceRefMgr_ReserveAddRef)
	{
		const wchar_t* szRes1 = L"RESOURCE1";

		CHECK_EQUAL(false, m_Mgr.IsReservedAddRef(szRes1));

		m_Mgr.ReserveAddRef(szRes1);
		CHECK_EQUAL(true, m_Mgr.IsReservedAddRef(szRes1));

		m_Mgr.ReleaseAddRef(szRes1);
		CHECK_EQUAL(false, m_Mgr.IsReservedAddRef(szRes1));
	}

	TEST_FIXTURE(FResourceRefMgr, TestResourceRefMgr_Grab)
	{
		const char* szRes1 = "RESOURCE1";
		RResource* pNewResource = new RResource(szRes1, RRESOURCETYPE::RR_TEXTURE);

		CHECK_EQUAL(1, pNewResource->GetRefCount());

		m_Mgr.Grab(pNewResource);

		CHECK_EQUAL(2, pNewResource->GetRefCount());

		m_Mgr.Drop(NULL, pNewResource);
		CHECK_EQUAL(1, pNewResource->GetRefCount());

		delete pNewResource;
	}

}