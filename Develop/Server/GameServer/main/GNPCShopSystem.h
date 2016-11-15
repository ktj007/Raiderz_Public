#ifndef _GNPC_SHOP_SYSTEM_H
#define _GNPC_SHOP_SYSTEM_H

class GNPCShopShower;
class GNPCShopBuyer;
class GNPCShopSeller;
class GNPCShopRepairer;

class GNPCShopSystem : public MTestMemPool<GNPCShopSystem>
{
public:
	GNPCShopSystem(void);
	~GNPCShopSystem(void);

	GNPCShopShower&		GetShower();
	GNPCShopBuyer&		GetBuyer();
	GNPCShopSeller&		GetSeller();
	GNPCShopRepairer&	GetRepairer();

private:
	GNPCShopShower*		m_pShower;
	GNPCShopBuyer*		m_pBuyer;
	GNPCShopSeller*		m_pSeller;
	GNPCShopRepairer*	m_pRepairer;
};

#endif //_GNPC_SHOP_SYSTEM_H
