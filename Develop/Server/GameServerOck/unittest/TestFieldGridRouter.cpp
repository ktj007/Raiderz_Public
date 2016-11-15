#include "stdafx.h"
#include "GFieldGridRouter.h"
#include "GTestForward.h"
#include "MCommandParameter.h"
#include "GEntityPlayer.h"
#include "GGlobal.h"
#include "GPlayerObjectManager.h"

SUITE(FieldGridRouter)
{
	struct TestFiedlGridRouter
	{
		GFieldGridRouter m_router;
		int m_nDivideNumber;

		TestFiedlGridRouter(): m_nDivideNumber(50)
		{			
		}

		void InitValue(vector<int>& vecValue, int nInitCount)
		{
			for(int i = 0; i < nInitCount; i++)
			{
				vecValue.push_back(i);
			}
		}

		template<typename T>
		bool GetBlob(MCommandParameter* pParam, vector<T>& vecT)
		{
			if(pParam->GetType()!=MPT_BLOB) return false;
			T* pPointer = static_cast<T*>(pParam->GetPointer());
			int nCount = pParam->GetBlobCount();

			_ASSERT(NULL != pPointer || 0 >= nCount);
			if (NULL == pPointer && 0 < nCount) return false;

			vecT.insert(vecT.end(), pPointer, pPointer + nCount);

			return true;
		}
	};

	TEST_FIXTURE(TestFiedlGridRouter, NewSplitParameter_0)
	{
		vector<int> vecValue;
		InitValue(vecValue, 0);
				
		MCommandParameter* pParameter = m_router.NewSplitParameter(vecValue, 0, m_nDivideNumber);

		vector<int> vecResult;
		ASSERT_CHECK(GetBlob(pParameter, vecResult));
		CHECK(vecValue == vecResult);
	}

	TEST_FIXTURE(TestFiedlGridRouter, NewSplitParameter_1)
	{
		vector<int> vecValue;
		InitValue(vecValue, 1);

		MCommandParameter* pParameter = m_router.NewSplitParameter(vecValue, 0, m_nDivideNumber);

		vector<int> vecResult;
		ASSERT_CHECK(GetBlob(pParameter, vecResult));
		CHECK(vecValue == vecResult);
	}

	TEST_FIXTURE(TestFiedlGridRouter, NewSplitParameter_2)
	{
		vector<int> vecValue;
		InitValue(vecValue, 2);

		MCommandParameter* pParameter = m_router.NewSplitParameter(vecValue, 0, m_nDivideNumber);

		vector<int> vecResult;
		ASSERT_CHECK(GetBlob(pParameter, vecResult));
		CHECK(vecValue == vecResult);
	}
	
	TEST_FIXTURE(TestFiedlGridRouter, NewSplitParameter_49)
	{
		vector<int> vecValue;
		InitValue(vecValue, 49);

		MCommandParameter* pParameter = m_router.NewSplitParameter(vecValue, 0, m_nDivideNumber);

		vector<int> vecResult;
		ASSERT_CHECK(GetBlob(pParameter, vecResult));
		CHECK(vecValue == vecResult);
	}

	TEST_FIXTURE(TestFiedlGridRouter, NewSplitParameter_50)
	{
		vector<int> vecValue;
		InitValue(vecValue, 50);

		MCommandParameter* pParameter = m_router.NewSplitParameter(vecValue, 0, m_nDivideNumber);

		vector<int> vecResult;
		ASSERT_CHECK(GetBlob(pParameter, vecResult));
		CHECK(vecValue == vecResult);
	}

	TEST_FIXTURE(TestFiedlGridRouter, NewSplitParameter_51)
	{
		vector<int> vecValue;
		InitValue(vecValue, 51);


		MCommandParameter* pParameter_0 = m_router.NewSplitParameter(vecValue, 0, m_nDivideNumber);
		MCommandParameter* pParameter_1 = m_router.NewSplitParameter(vecValue, 1, m_nDivideNumber);


		vector<int> vecResult_0;
		ASSERT_CHECK(GetBlob(pParameter_0, vecResult_0));		
		CHECK(vector<int>(vecValue.begin(), vecValue.begin() + 50) == vecResult_0);

		vector<int> vecResult_1;
		ASSERT_CHECK(GetBlob(pParameter_1, vecResult_1));
		CHECK(vector<int>(vecValue.begin() + 50, vecValue.end()) == vecResult_1);
	}

	TEST_FIXTURE(TestFiedlGridRouter, NewSplitParameter_150)
	{
		vector<int> vecValue;
		InitValue(vecValue, 101);


		MCommandParameter* pParameter_0 = m_router.NewSplitParameter(vecValue, 0, m_nDivideNumber);
		MCommandParameter* pParameter_1 = m_router.NewSplitParameter(vecValue, 1, m_nDivideNumber);
		MCommandParameter* pParameter_2 = m_router.NewSplitParameter(vecValue, 2, m_nDivideNumber);


		vector<int> vecResult_0;
		ASSERT_CHECK(GetBlob(pParameter_0, vecResult_0));		
		CHECK(vector<int>(vecValue.begin(), vecValue.begin() + 50) == vecResult_0);

		vector<int> vecResult_1;
		ASSERT_CHECK(GetBlob(pParameter_1, vecResult_1));
		CHECK(vector<int>(vecValue.begin() + 50, vecValue.begin() + 100) == vecResult_1);

		vector<int> vecResult_2;
		ASSERT_CHECK(GetBlob(pParameter_2, vecResult_2));		
		CHECK(vector<int>(vecValue.begin() + 100, vecValue.end()) == vecResult_2);
	}
}
