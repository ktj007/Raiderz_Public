#pragma once

class XGlueGameGuildStorage
{
public:
	int GetGuildStorageMoney()						{ return gvar.MyInfo.Guild.GetStorageMoney();	}

	int GetGuildStorageItemCount() const			{ return gvar.MyInfo.GuildStorage.GetSize();}
	int GetGuildStorageItemMaxCount() const			{ return gvar.MyInfo.GuildStorage.GetSlotSize();}
	int GetGuildStorageItemID( int nSlotID) const;
	int GetGuildStorageItemQuantity( int nSlotID ) const;
	bool DepositGuildMoney(int nMoney) const;
	bool WithdrawGuildMoney( int nMoney ) const;
	bool DepositGuildItem( int nSrcSlotID, int nTarSlotID, int nQuantity) const;
	bool WithdrawGuildItem( int nSrcSlotID, int nTarSlotID, int nQuantity) const;
	bool MoveGuildStorageItem( int nSrcSlotID, int nTarSlotID) const;
	//bool DropGuildStorageItem(int nSlotID) const;
};
