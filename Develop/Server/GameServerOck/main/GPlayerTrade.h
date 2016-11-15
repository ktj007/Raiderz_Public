#ifndef _GTRADE_CONTENTS_H_
#define _GTRADE_CONTENTS_H_

class GEntityPlayer;
class GItem;
struct TD_ITEM;

class GTRADE_ITEM
{
public :
	GTRADE_ITEM()
		: m_nSlotID(0), m_nSlotIDTo(0), m_nItemID(0), m_nIUID(0), m_nStackAmt(0), m_nTradeAmt(0)
		, m_nDura(0), m_nMaxDura(0), m_nEnchCnt(0), m_nColor(0), m_nSoulCnt(0), m_nRemainUsagePeriod(0), m_nAddExpiSec(-1)
	{

	}

	GTRADE_ITEM(const int16 nSlotID, const int16 nToSlotID, const int nItemID, const int64 nIUID, const int16 nStackAmt, const int nTradeStackAmt
		, const uint8 nDura, const uint8 nMaxDura, const uint8 nEnchCnt, const int nColor, const uint8 nSoulCnt
		, const int nRemainUsagePeriod, const int nAddExpiSec)
		: m_nSlotID(nSlotID), m_nSlotIDTo(nToSlotID), m_nItemID(nItemID), m_nIUID(nIUID), m_nStackAmt(nStackAmt), m_nTradeAmt(nTradeStackAmt)
		, m_nDura(nDura), m_nMaxDura(nMaxDura), m_nEnchCnt(nEnchCnt), m_nColor(nColor), m_nSoulCnt(nSoulCnt)
		, m_nRemainUsagePeriod(nRemainUsagePeriod), m_nAddExpiSec(nAddExpiSec)
	{

	}

	int16	m_nSlotID;
	int16	m_nSlotIDTo;
	int		m_nItemID;
	int64	m_nIUID;
	int16	m_nStackAmt;
	int16	m_nTradeAmt;
	uint8	m_nDura;
	uint8	m_nMaxDura;
	uint8	m_nEnchCnt;
	int		m_nColor;
	uint8	m_nSoulCnt;
	int		m_nRemainUsagePeriod;
	int		m_nAddExpiSec;
};

typedef vector<GTRADE_ITEM> GTRADE_ITEM_VEC;

class GTRADE_CHAR
{
public :
	GTRADE_CHAR(const int64 nAID, const MUID& uidPlayer, const int64 nCID, const int nXP, const int nMoney, int nModMoney, const int nCharPtm
			  , int nDeltaCharPtm, uint8 nLevel, const int nFieldID, const float fPosX, const float fPosY, const float fPosZ)
		: m_nAID(nAID), m_uidPlayer(uidPlayer), m_nCID(nCID), m_nXP(nXP), m_nMoney(nMoney), m_nModMoney(nModMoney), m_nCharPtm(nCharPtm)
		, m_nDeltaCharPtm(nDeltaCharPtm), m_nLevel(nLevel), m_nFieldID(nFieldID), m_fPosX(fPosX), m_fPosY(fPosY), m_fPosZ(fPosZ)
	{
		m_vItem.resize(MAX_TRADE_ITEM_SPACE);
	}

public :
	int64			m_nAID;
	MUID			m_uidPlayer;
	int64			m_nCID;
	int				m_nXP;
	int				m_nMoney;
	int				m_nModMoney;
	int				m_nCharPtm;
	int				m_nDeltaCharPtm;
	uint8			m_nLevel;
	int				m_nFieldID;
	float			m_fPosX;
	float			m_fPosY;
	float			m_fPosZ;

	GTRADE_ITEM_VEC m_vItem;
};

typedef vector<GTRADE_CHAR> GTRADE_CHAR_VEC;

class GTradeItem : public MTestMemPool<GTradeItem>
{
public:
	GTradeItem(int nSlotID, int nAmount, int nID);
	~GTradeItem();

	void	Increse(int nAmount);
	void	Decrease(int nAmount);

	int		GetSlotID();
	int		GetAmount();
	int		GetID();

private:	
	int		m_nSlotID;
	int		m_nAmount;
	int		m_nID;
};

class GPlayerTrade : public MTestMemPool<GPlayerTrade>
{
public:
	GPlayerTrade(GEntityPlayer* pOwner);
	virtual ~GPlayerTrade();
	
	void	Reset();

	void	TradeRequested(MUID nRequesterUID);
	void	TradeStart(MUID nTraderUID);
	void	TradeReject();

	void	AddItem(int nSlotID, int nAmount, int nID);
	void	RemoveItem(int nSlotID, int nAmount);
	void	SetMoney(int nMoney);

	void	Confirm();
	void	Unconfirm();	
	
	bool	IsTradeRequester(MUID nPlayerUID);	///< 거래 요청한 사람인지?
	bool	IsTrader(MUID nPlayerUID);			///< 거래중인 사람인지?
	bool	IsExistItemByID(int nItemID);
	bool	IsExistItemBySlotID(int nSlotID);
	bool	IsItemFull();
	bool	IsConfirm();

	vector<GTradeItem*> GetAllItem();
	GTradeItem*			GetItem(int nSlotID);
	int		GetItemAmount(int nSlotID);
	int		GetItemCount();
	int		GetMoney();
	MUID	GetTrader();
	MUID	GetTradeRequesterUID();

private:
	GEntityPlayer*			m_pOwner;
	map<int, GTradeItem*>	m_mapTradeItem;
	
	bool		m_bConfirm;
	int			m_nMoney;
	MUID		m_uidTrader;			///< 상대 거래자(거래중)
	MUID		m_nTradeRequesterUID;	///< 거래시작을 요청한 사람(아직 거래중 아님)	
};

#endif // _GTRADE_CONTENTS_H_

