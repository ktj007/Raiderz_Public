#include "stdafx.h"
#include "TestCutSceneCommon.h"

SUITE(CutSceneObjects)
{
	class CutSceneObjectsFixture
	{
	public:
		CutSceneObjectsFixture()
		{
			const char* pSzXml = 
				"<?xml version=\"1.0\" encoding=\"UTF-8\" ?>													\
				<maiet>																							\
					<clapper desc=\"ÄÁÅÙÃ÷ test\" field=\"1000\" duration=\"8.0\" in=\"none\" out=\"none\" />	\
					<camera/>																					\
				</maiet>";

			bool bRet = m_Contents.LoadStream(pSzXml);
			CHECK(bRet);

			m_pCutSceneBuilder = new TestCutSceneContentBuilder;
			m_pCutSceneContent = new XCutSceneContent;
		}

		~CutSceneObjectsFixture()
		{
			delete m_pCutSceneContent;
			delete m_pCutSceneBuilder;
		}

		std::string GetStringFromXML()
		{
			MXmlPrinter visitor;
			m_Contents.Doc()->Accept(&visitor);
			return std::string(visitor.CStr());
		}

		bool AddNode(const char* _pSzNodeContents)
		{
			MXmlElement element("");
			const char* pRet = element.Parse(_pSzNodeContents, NULL, TIXML_ENCODING_UTF8);
			if(NULL == pRet)
				return false;

			MXmlElement* pMaiet = m_Contents.Doc()->RootElement();
			CHECK(pMaiet!=NULL);

			pMaiet->InsertEndChild(element);
			return true;
		}

		XCutSceneContent* BuildCutSceneContents()
		{
			USES_CONVERSION_EX;
			if (m_pCutSceneBuilder->Build(A2W_EX(GetStringFromXML().c_str(), 0), m_pCutSceneContent))
			{
				return m_pCutSceneContent;
			}

			return NULL;
		}

		MXml m_Contents;
		TestCutSceneContentBuilder* m_pCutSceneBuilder;
		XCutSceneContent* m_pCutSceneContent;
	};

	TEST_FIXTURE(CutSceneObjectsFixture, CutSceneObjectCamera)
	{
		XCutSceneContent* pCutSceneContent = BuildCutSceneContents();
		CHECK(pCutSceneContent);

		// check object
		VEC_CUTSCNEOBJ& rObjects = pCutSceneContent->GetCutSceneObjects();
		CHECK_EQUAL(1, rObjects.size());

		XCutSceneObject* pObject = *rObjects.begin();
		CHECK_EQUAL(XCutSceneObject::E_CAMERA, pObject->GetType());
		CHECK( pObject->GetName() == XCutSceneObject::GetTypeStr(XCutSceneObject::E_CAMERA));
		CHECK( std::wstring(pObject->GetTypeStr()) == XCutSceneObject::GetTypeStr(XCutSceneObject::E_CAMERA));
		CHECK( std::wstring(L"camera") == pObject->GetTypeStr());
	}

	TEST_FIXTURE(CutSceneObjectsFixture, CutSceneObjectActor)
	{
		AddNode("<actor name=\"test_actor\" type=\"npc_31\" />");
		XCutSceneContent* pCutSceneContent = BuildCutSceneContents();
		CHECK(pCutSceneContent);

		// check object
		VEC_CUTSCNEOBJ& rObjects = pCutSceneContent->GetCutSceneObjects();
		CHECK_EQUAL(2, rObjects.size());

		XCutSceneObject* pObject = *(rObjects.begin() + 1);
		CHECK_EQUAL(XCutSceneObject::E_ACTOR, pObject->GetType());
		CHECK( pObject->GetTypeStr() == XCutSceneObject::GetTypeStr(XCutSceneObject::E_ACTOR));
		CHECK( pObject->GetName() == wstring(L"test_actor") );
	}

	TEST_FIXTURE(CutSceneObjectsFixture, CutSceneObjectEvent)
	{
		AddNode("<event />");
		XCutSceneContent* pCutSceneContent = BuildCutSceneContents();
		CHECK(pCutSceneContent);

		// check object
		VEC_CUTSCNEOBJ& rObjects = pCutSceneContent->GetCutSceneObjects();
		CHECK_EQUAL(2, rObjects.size());

		XCutSceneObject* pObject = *(rObjects.begin() + 1);
		CHECK_EQUAL(XCutSceneObject::E_EVENT, pObject->GetType());
		CHECK( std::wstring(pObject->GetTypeStr()) == XCutSceneObject::GetTypeStr(XCutSceneObject::E_EVENT));
		CHECK( std::wstring(L"event") == pObject->GetTypeStr());
	}
}
