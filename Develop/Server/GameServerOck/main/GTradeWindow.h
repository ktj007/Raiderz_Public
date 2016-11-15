#ifndef _GTRADE_WINDOW_H_
#define _GTRADE_WINDOW_H_

#include "GItemContainer.h"

struct TD_ITEM;

class GTradeWindow
{
private:
	GEntityPlayer*		m_pOwner;				///< 본인
	MUID				m_uidOtherTrader;			///< 상대 거래자

	GItemContainer		m_myItemContainer;	
	bool				m_bMyConfirm;

	GItemContainer*		m_pOtherItemContainer;	
	bool*				m_pOtherConfirm;


public:
	GTradeWindow();
	virtual ~GTradeWindow();

	void	Init(GEntityPlayer* pOwnerPlayer);

	bool	Start(GEntityPlayer* pReqPlayer);
	bool	PutUpItem(TD_ITEM* pItem);
	bool	PutDownItem(TD_ITEM* pItem);
	bool	Confirm(TD_ITEM* pGiveItems, int nGiveItemCount, TD_ITEM* pTakeItems, int nTakeItemCount);
	bool	Reset();

	bool	IsTradeConclusion();
	bool	IsValidTrade();

	MUID	GetOtherTrader()						{ return m_uidOtherTrader; }
	void	SetOtherTrader(MUID uidPlayer)			
	{
		m_uidOtherTrader = uidPlayer; 
	}
		
	void	MakeTD_Item(TD_ITEM* pItems);

	void	SetOtherItemContainerAndConfirm(GItemContainer* pItemContainer, bool* pConfirm);
	void	GetMyItemContainerAndConfirm(GItemContainer*& pOutItemContainer, bool*& pOutConfirm);

	// Test
	void	TestSetConfirm(bool bConfirm)			{ m_bMyConfirm = bConfirm; }
	GItem*	TestGetItem(MUID uidItem)				{ return m_myItemContainer.GetItem(uidItem); }

};

#endif // _GTRADE_WINDOW_H_

