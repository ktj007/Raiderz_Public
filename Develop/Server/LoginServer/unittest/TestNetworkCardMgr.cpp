#include "stdafx.h"
#include "SNetworkCardMgr.h"

SUITE(NetworkCardMgr)
{
	struct Fixture
	{
		Fixture()
		{
			GetNetworkCardMgr()->Clear();

			char name[64] = {0,};
			gethostname(name, sizeof(name));

			hostent* pHostEnt = gethostbyname(name);
			for(int i = 0; pHostEnt->h_addr_list[i] != NULL; ++i)
			{
				if (AF_INET	!= pHostEnt->h_addrtype) continue;

				in_addr iaddr;
				memcpy(&iaddr, pHostEnt->h_addr_list[i], sizeof(iaddr));

				wchar_t addr[16] = {0,};
				wsprintfW(addr, L"%S", inet_ntoa(iaddr));				

				m_vecAddress.push_back(addr);
			}
		}
		~Fixture()
		{
			GetNetworkCardMgr()->Clear();
		}

		vector<wstring> m_vecAddress;
	};

	TEST_FIXTURE(Fixture, TestNetworkCardMgr_Init)
	{
		GetNetworkCardMgr()->Init();

		CHECK_EQUAL(m_vecAddress.size(),	GetNetworkCardMgr()->GetNetworkCardCount());
	}

	TEST_FIXTURE(Fixture, TestNetworkCardMgr_RegCard)
	{
		GetNetworkCardMgr()->Init();

		if (0 == GetNetworkCardMgr()->GetNetworkCardCount()) return;
		CHECK(0 == GetNetworkCardMgr()->GetRegistedNetworkCardCount());

		const int NETWORK_CARD_ID = 1;
		GetNetworkCardMgr()->RegCard(m_vecAddress[0], NETWORK_CARD_ID);
		CHECK_EQUAL(1,					GetNetworkCardMgr()->GetRegistedNetworkCardCount());		
		CHECK(m_vecAddress[0] == GetNetworkCardMgr()->GetIP(NETWORK_CARD_ID));

		const int INVALID_CARD_ID = 999;
		CHECK(L"" == GetNetworkCardMgr()->GetIP(INVALID_CARD_ID));
	}
}
