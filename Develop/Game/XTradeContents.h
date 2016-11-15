#ifndef _XTrade_Contents_h_
#define _XTrade_Contents_h_

// XItem을 사용하는 것이 이상하면 TradeItem : public XItem 만들기
#include "XItemContainer.h"

class XTradeContainer: public XTradeItemContainer
{
public:
	XTradeContainer();
	virtual ~XTradeContainer();

	virtual void	PushItem(XItem* item);
	virtual XItem*	InsertItem(const TD_TRADE_ITEM_PUTUP& tdItemPutUp);
	bool			DecItem(const TD_TRADE_ITEM_PUTDOWN& tdItemPutDown);

	void			DeleteAll();	

	void			MakeTD_Item(vector<TD_ITEM>& vecTDItem);

	MWLua::table	GetTradeItemTable();

	XItem*			FindTradeItemForInvenSlotID(int nSlotID);
};

/// 거래창
class XTradeContents
{
private:		
	bool			m_bTrade;
public:
	XTradeContainer	m_MyTradeContainer;
	bool			m_bMyConfirm;
	int				m_nMySilver;

	XTradeContainer	m_OtherTradeContainer;	
	bool			m_bOtherConfirm;
	int				m_nOtherSilver;
	
public:

	XTradeContents();
	virtual ~XTradeContents();

	void	Reset();

	bool IsTrade() const	{ return m_bTrade; }
	void SetTrade(bool val) { m_bTrade = val; }
};

#endif //_XTrade_Contents_h_