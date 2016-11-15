#include "stdafx.h"
#include "TestCutSceneCommon.h"

SUITE(CutSceneContents)
{
	class CutSceneContentsFixture
	{
	public:
		CutSceneContentsFixture()
		{
			m_pClapperElement = NULL;
		}

		~CutSceneContentsFixture()
		{
			delete m_pClapperElement;
		}

		MXmlElement* ResetClapperXML(const std::wstring& _rStrContents)
		{
			if (m_pClapperElement)
			{
				delete m_pClapperElement;
			}

			m_pClapperElement = new MXmlElement("clapper");
			std::string strContents = MLocale::ConvUTF16ToUTF8(_rStrContents.c_str());
			m_pClapperElement->Parse(strContents.c_str(), NULL, TIXML_ENCODING_UTF8);
			return m_pClapperElement;
		}

	private:
		MXmlElement* m_pClapperElement;
	};

	TEST_FIXTURE(CutSceneContentsFixture, Clapper)
	{
		XCutSceneClapper clapper;

		// check init-value
		CHECK(wstring(L"") == clapper.GetCutSceneDesc());
		CHECK_EQUAL(-1, clapper.GetField());
		CHECK_EQUAL(-1.f, clapper.GetDuration());

		// check setter / getter
		clapper.SetCutSceneDesc(L"hi");
		clapper.SetField(10);
		clapper.SetDuration(10.f);
		CHECK(wstring(L"hi") == clapper.GetCutSceneDesc());
		CHECK_EQUAL(10, clapper.GetField());
		CHECK_EQUAL(10.f, clapper.GetDuration());

		// check helper function
		std::wstring strClapper = XCutSceneClapper::CreateXMLString(L"ÇÑ±Û cutcene", 200, 8.f);
		CHECK(wstring(L"<clapper desc=\"ÇÑ±Û cutcene\" field=\"200\" duration=\"8.000\" in=\"none\" out=\"none\" />") == strClapper);

		// check parsing
		bool bRet = clapper.ParseFromElement(ResetClapperXML(strClapper));
		CHECK(bRet);
		CHECK(wstring(L"ÇÑ±Û cutcene") == clapper.GetCutSceneDesc());
		CHECK_EQUAL(200, clapper.GetField());
		CHECK_CLOSE(8.f, clapper.GetDuration(), 0.001f);
	}

	TEST_FIXTURE(CutSceneContentsFixture, Content)
	{
		const wchar_t* pSzXml = 
			L"<?xml version=\"1.0\" encoding=\"UTF-8\" ?>													\
			<maiet>																							\
				<clapper desc=\"ÄÁÅÙÃ÷ test\" field=\"1000\" duration=\"8.0\" in=\"none\" out=\"none\" />	\
				<camera/>																					\
			</maiet>";

		// build
		XCutSceneContent* pCutSceneContent = new XCutSceneContent;
		TestCutSceneContentBuilder cutSceneBuilder;
		bool bRet = cutSceneBuilder.Build(pSzXml, pCutSceneContent);
		CHECK(bRet);

		// check clapper board
		XCutSceneClapper* pClapper = pCutSceneContent->GetClapper();
		CHECK(wstring(L"ÄÁÅÙÃ÷ test") == pClapper->GetCutSceneDesc());
		CHECK_EQUAL(1000, pClapper->GetField());
		CHECK_CLOSE(8.f, pClapper->GetDuration(), 0.001f);

		// check object
		VEC_CUTSCNEOBJ& rObjects = pCutSceneContent->GetCutSceneObjects();
		CHECK_EQUAL(1, rObjects.size());
		CHECK_EQUAL(XCutSceneObject::E_CAMERA, rObjects[0]->GetType());

		// Run Content
		pCutSceneContent->Run(0.3f);
		CHECK_CLOSE(0.3f, pCutSceneContent->GetCurrnetRunningTime(), 0.001f);
		pCutSceneContent->Run(0.3f);
		CHECK_CLOSE(0.6f, pCutSceneContent->GetCurrnetRunningTime(), 0.001f);

		pCutSceneContent->ResetRunning();
		CHECK_CLOSE(0.0f, pCutSceneContent->GetCurrnetRunningTime(), 0.001f);

		delete pCutSceneContent;
	}
}
