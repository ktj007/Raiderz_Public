#pragma  once

#include "UtilResource.h"
#include "UtilResourceManager.h"

namespace rs3
{
	class FixtureResourceManager : public testing::Test
	{
	protected:
		FixtureResourceManager() {}
		virtual ~FixtureResourceManager() {}

		virtual void SetUp() override
		{
			m_pResourceManager = UTIL_RESOURCE_MANAGER.CreateResourceManager();

			m_ResourceName = "Test_Resouce";
			m_NamedResourceCreationInfo				= UTIL_RESOURCE.CreateMockResourceCreationInfo(MOCKRESOURCETYPE::MOCKRESOURCE_VALID,m_ResourceName);
			m_NamedStateDescriptiveCreationInfo		= UTIL_RESOURCE.CreateMockResourceCreationInfo(MOCKRESOURCETYPE::MOCKRESOURCE_STATE_DESCRIPTIVE,m_ResourceName);
			m_NamedFillFailedCreationInfo			= UTIL_RESOURCE.CreateMockResourceCreationInfo(MOCKRESOURCETYPE::MOCKRESOURCE_FILL_FAILED,m_ResourceName);
			m_NamedLoadFailedCreationInfo			= UTIL_RESOURCE.CreateMockResourceCreationInfo(MOCKRESOURCETYPE::MOCKRESOURCE_LOAD_FAILED,m_ResourceName);
			m_NamedResourceCreationInfoHavingDependentResources = UTIL_RESOURCE.CreateMockResourceCreationInfo(MOCKRESOURCETYPE::MOCKRESOURCE_HAVE_DEPENENT_RESOURCES,m_ResourceName);

			static const std::string UNNAMED_RESOURCE_NAME = "";
			m_UnnamedResourceCreationInfo = UTIL_RESOURCE.CreateMockResourceCreationInfo(MOCKRESOURCETYPE::MOCKRESOURCE_VALID,UNNAMED_RESOURCE_NAME);
		}

		virtual void TearDown() override
		{
		}

		void		UpdateWork(void)
		{
			m_pResourceManager->GetResourceProvider()->GetWorker()->Update();
		}

		RResource*	UpdateWorkUntilGettingResource(RResourceWorkUnit* pResourceWorkUnit)
		{
			bool bGotResource = false;
			RResource* pResource = NULL;
			while( true )
			{
				UpdateWork();
				pResource = m_pResourceManager->TryGetResource(pResourceWorkUnit,bGotResource,0);
				if( true == bGotResource )	break;
			}
			return pResource;
		}

	protected:
		MockResourceCreationInfo	m_NamedResourceCreationInfo;
		MockResourceCreationInfo	m_NamedStateDescriptiveCreationInfo;
		MockResourceCreationInfo	m_UnnamedResourceCreationInfo;
		MockResourceCreationInfo	m_NamedFillFailedCreationInfo;
		MockResourceCreationInfo	m_NamedLoadFailedCreationInfo;
		MockResourceCreationInfo	m_NamedResourceCreationInfoHavingDependentResources;

		TResourceManager*			m_pResourceManager;
		std::string					m_ResourceName;

	private:
		UtilResource				UTIL_RESOURCE;
		UtilResourceManager			UTIL_RESOURCE_MANAGER;
		
	};
}