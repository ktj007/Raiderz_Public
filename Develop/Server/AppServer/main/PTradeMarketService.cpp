#include "stdafx.h"
#include "PTradeMarketService.h"
#include "CCommandTable_App.h"
#include "CTransData.h"
#include "SDef.h"
#include "PGlobal.h"
#include "PCommandCenter.h"
#include "SProxyPlayerManager.h"
#include "PServer.h"
#include "SProxyPlayer.h"
#include "PGameServerObject.h"
#include "PGameServerObjectManager.h"

PTradeMarketService::PTradeMarketService()
{
	m_iocp = new MIOCP();
	if (NULL == m_iocp)
		return;
	m_iocp->Init(1);
	m_curInsertPageIdx = 0;
	m_totalCnt = 0;
}

PTradeMarketService::~PTradeMarketService()
{
	if (NULL == m_iocp)
		return;
	m_iocp->Term();
	delete m_iocp;
	m_iocp = NULL;
	m_playersFilters.clear();
	
	SEARCH_FILTER::release();
}

void PTradeMarketService::NotifyAddItem(minet::MCommand* const cmd) const
{
	OnCmd(cmd);
}

void PTradeMarketService::NotifyDelItem(minet::MCommand* const cmd) const
{
	OnCmd(cmd);
}

void PTradeMarketService::RequestSearch(minet::MCommand* const cmd) const
{
	OnCmd(cmd);
}

void PTradeMarketService::RequestSearchNext(minet::MCommand* const cmd) const
{
	OnCmd(cmd);
}

void PTradeMarketService::OnCmd(minet::MCommand* const cmd) const
{
	IOMSG* const msg = new IOMSG;
	if (NULL == msg)
		return;
	msg->cmd = cmd;
	m_iocp->Post(*this, msg);
}

void PTradeMarketService::OnIO(const DWORD& numOfByteTransfered,OVERLAPPED* const overlapped)
{
	IOMSG* const msg = static_cast<IOMSG*>(overlapped);
	if (NULL == msg)
	{
		mlog("PTradeMarketService::OnIO() - error null msg\n");
		return;
	}
	if (NULL == msg->cmd)
	{
		mlog("PTradeMarketService::OnIO() - error null cmd\n");
		delete msg;
		return;
	}
	
	switch (msg->cmd->GetID())
	{
	case MPC_GA_TRADEMARKET_ADD:
		OnTradeMarketAdd(*(msg->cmd));
		break;

	case MPC_GA_TRADEMARKET_DEL:
		OnTradeMarketDel(*(msg->cmd));
		break;

	case MPC_GA_TRADEMARKET_SEARCH:
		OnTradeMarketSearch(*(msg->cmd));
		break;

	case MPC_GA_TRADEMARKET_SEARCH_NEXT:
		OnTradeMarketSearchNext(*(msg->cmd));
		break;
	}

	delete msg->cmd;
	delete msg;
}

void PTradeMarketService::OnError(const int& errCode)
{
	mlog("PTradeMarketService::OnError() - error, code : %d\n", errCode);
}

void PTradeMarketService::OnTradeMarketAdd(minet::MCommand& cmd)
{
	TD_TRADEMARKETITEM* item = NULL;
	if (false == cmd.GetSingleBlob(item, 0))
		return;

	if (NULL == item)
		return;

	if (m_itemsByDBID.end() != m_itemsByDBID.find(item->dbid))
	{
		mlog("PTradeMarketService::OnTradeMarketAdd() - 같은 dbid의 item을 두번 add하려 했다, dbid : %I64d\n", item->dbid);
		return;
	}

	Add(*item);
}

void PTradeMarketService::OnTradeMarketDel(minet::MCommand& cmd)
{
	ITEM_TYPEID typeID = 0;
	ITEM_DBID dbid = 0;
	if (false == cmd.GetParameter(&typeID, 0, minet::MPT_INT))
		return;

	if (false == cmd.GetParameter(&dbid, 1, minet::MPT_INT64))
		return;

	ITEMS_BY_DBID::iterator delIt = m_itemsByDBID.find(dbid);
	if (delIt == m_itemsByDBID.end())
	{
		mlog("PTradeMarketService::OnTradeMarketDel() - 없는 dbid의 아이템을 지우려 했다\n", dbid);
		return;
	}

	const TD_TRADEMARKETITEM& item(delIt->second);
	ITEMS_ORDER_BY_PRICE& itemsOrderByPrice(m_itemsByTypeID[typeID]);
	ITEMS_ORDER_BY_PRICE::iterator lIt = itemsOrderByPrice.lower_bound(item);
	ITEMS_ORDER_BY_PRICE::iterator uIt = itemsOrderByPrice.upper_bound(item);
	for (; lIt != uIt; ++lIt)
	{
		const TD_TRADEMARKETITEM& curItem(*lIt);
		if (dbid != curItem.dbid)
			continue;
		
		itemsOrderByPrice.erase(lIt);
		m_itemsByDBID.erase(delIt);
		--m_totalCnt;
		return;
	}

	mlog("지우려는 아이템이 dbid컨테이너에는 있지만, typeid컨테이너에는 없다? dbid : %I64d, typeID : %d\n", dbid, typeID);
}

void PTradeMarketService::OnTradeMarketSearch(minet::MCommand& cmd)
{
	CID requester;
	VALID(true == cmd.GetParameter(&requester, 0, minet::MPT_INT));
	SEARCH_FILTER* const filter = new SEARCH_FILTER();
	VALID(NULL!=filter);
	if (false == cmd.GetBlob(filter->wantedSearchTypeIDs, 1))
	{
		mlog("PTradeMarketService::OnTradeMarketSearch() - get blob failed\n");
		delete filter;
		return;
	}

	if (true == filter->wantedSearchTypeIDs.empty())
	{
		mlog("PTradeMarketService::OnTradeMarketSearch() - empty searching\n");
		delete filter;
		return;
	}

	m_playersFilters[requester] = FilterPtr(filter);
	Search(requester);
}

void PTradeMarketService::OnTradeMarketSearchNext(minet::MCommand& cmd)
{
	CID requester;
	VALID(true == cmd.GetParameter(&requester, 0, minet::MPT_INT));
	Search(requester);
}

void PTradeMarketService::Add(const TD_TRADEMARKETITEM& item)
{
	++m_totalCnt;
	m_itemsByDBID.insert( ITEMS_BY_DBID::value_type(item.dbid, item) );
	m_itemsByTypeID[item.typeID].insert(item);
}

void PTradeMarketService::Search(const CID& player)
{
	PLAYER_FILTERS::iterator it = m_playersFilters.find(player);
	VALID(m_playersFilters.end() != it);
	
	SEARCH_FILTER* const filter((it->second).get());
	VALID(false == filter->wantedSearchTypeIDs.empty());

	int findedCnt = 0;
	int lastFindedPrice = 0;
	bool seekLastestFindedTypeID = true;	// 마지막으로 찾았던 typeID의 
	vector<TD_TRADEMARKETITEM> findedItems;
	int lastFindItemTypeID = 0;

	for each (const int& wantedFindTypeID in filter->wantedSearchTypeIDs)
	{
		if (-1 == wantedFindTypeID)
		{
			// -1은 찾아서 제거한 아이디
			continue;
		}
		lastFindItemTypeID = wantedFindTypeID;

		for each (const TD_TRADEMARKETITEM& item in m_itemsByTypeID[wantedFindTypeID])
		{
			if (item.price < filter->lastFildedPrice)
				continue;

			if (filter->findedDBID.end() != filter->findedDBID.find(item.dbid))
				continue;

			filter->findedDBID.insert(item.dbid);
			filter->lastFildedPrice = item.price;
			findedItems.push_back(item);
			++findedCnt;

			if (findedCnt >= TRADEMARKET_ITEM_SEARCH_PAGE_SIZE)
				break;
		}

		if (findedCnt >= TRADEMARKET_ITEM_SEARCH_PAGE_SIZE)
			break;
	}

	// 찾아낸 id들은 -1로 채워넣는다.
	for (size_t i = 0; i < filter->wantedSearchTypeIDs.size() ; ++i)
	{
		if (filter->wantedSearchTypeIDs[i] == lastFindItemTypeID)
			break;
		filter->wantedSearchTypeIDs[i] = -1;
	}

	// 마지막까지 다 찾았다면 필터를 제거한다
	if (-1 == filter->wantedSearchTypeIDs[filter->wantedSearchTypeIDs.size() - 1])
	{
		m_playersFilters.erase(player);
	}

	Route(player, findedItems);
}

void PTradeMarketService::Route(const CID& playerCID,vector<TD_TRADEMARKETITEM>& findedItems)
{
	SProxyPlayerManager* const pPlayerManager = gsys.pServer->GetPlayerManager();
	SProxyPlayer* const pPlayer = pPlayerManager->FindPlayer(playerCID);

	if (NULL == pPlayer)
	{
		return;
	}

	int nGameServerID = pPlayer->GetGameServerID();
	PGameServerObject* const pGameServer = gmgr.pGameServerObjectManager->Find(nGameServerID);

	if (NULL == pGameServer)
	{
		return;
	}

	minet::MCommand* const cmd = gsys.pCommandCenter->MakeNewCommand(MPC_AG_TRADEMARKET_SEARCH_RESULT, 2, NEW_INT(playerCID), NEW_BLOB(findedItems));
	VALID(NULL != cmd);
	gsys.pCommandCenter->SendCommand(pGameServer->GetUID(), cmd);
}
