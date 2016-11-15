#include "stdafx.h"
#include "GDBTaskDataItemEquip.h"
#include "CSItemData.h"


GDBT_ITEM_EQUIP::GDBT_ITEM_EQUIP()
: uidPlayer(0), nCID(0)
{

}

GDBT_ITEM_EQUIP::GDBT_ITEM_EQUIP( MUID uidPlayer, int64 nCID, GDBT_EQUIP_ITEM_SLOT& equip, GDBT_UNEQUIP_ITEM_SLOT& unequip1, GDBT_UNEQUIP_ITEM_SLOT& unequip2 )
: uidPlayer(uidPlayer), nCID(nCID), equip(equip), unequip1(unequip1), unequip2(unequip2)
{

}

GDBT_ITEM_UNEQUIP::GDBT_ITEM_UNEQUIP()
: uidPlayer(0), nCID(0)
{

}

GDBT_ITEM_UNEQUIP::GDBT_ITEM_UNEQUIP( MUID uidPlayer, int64 nCID, GDBT_UNEQUIP_ITEM_SLOT& unequip1, GDBT_UNEQUIP_ITEM_SLOT& unequip2)
: uidPlayer(uidPlayer), nCID(nCID), unequip1(unequip1), unequip2(unequip2)
{

}