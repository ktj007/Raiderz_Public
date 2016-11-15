#include "StdAfx.h"
#include "GNPCShopSystem.h"
#include "GNPCShopShower.h"
#include "GNPCShopBuyer.h"
#include "GNPCShopSeller.h"
#include "GNPCShopRepairer.h"

GNPCShopSystem::GNPCShopSystem(void)
{
	m_pShower = new GNPCShopShower;
	m_pBuyer = new GNPCShopBuyer;
	m_pSeller = new GNPCShopSeller;
	m_pRepairer = new GNPCShopRepairer;
}

GNPCShopSystem::~GNPCShopSystem(void)
{
	SAFE_DELETE(m_pShower);
	SAFE_DELETE(m_pBuyer);
	SAFE_DELETE(m_pSeller);
	SAFE_DELETE(m_pRepairer);
}

GNPCShopShower& GNPCShopSystem::GetShower()
{
	return (*m_pShower);
}

GNPCShopBuyer& GNPCShopSystem::GetBuyer()
{
	return (*m_pBuyer);
}

GNPCShopSeller& GNPCShopSystem::GetSeller()
{
	return (*m_pSeller);
}

GNPCShopRepairer& GNPCShopSystem::GetRepairer()
{
	return (*m_pRepairer);
}