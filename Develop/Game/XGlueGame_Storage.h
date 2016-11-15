#pragma once

class XGlueGameStorage
{
public:
	// Storage
	int GetStorageMoney()						{ return gvar.MyInfo.ChrInfo.GetStorageMoney();	}
	int GetStorageItemCount() const				{ return gvar.MyInfo.Storage.GetSize();}
	int GetStorageItemMaxCount() const			{ return gvar.MyInfo.Storage.GetSlotSize();}
	int GetStorageItemID( int nSlotID) const;
	int GetStorageItemQuantity( int nSlotID ) const;
	bool DepositMoney(int nMoney) const;
	bool WithdrawMoney( int nMoney ) const;
	bool DepositItem( int nSrcSlotID, int nTarSlotID, int nQuantity) const;
	bool WithdrawItem( int nSrcSlotID, int nTarSlotID, int nQuantity) const;
	bool MoveStorageItem( int nSrcSlotID, int nTarSlotID) const;
	bool DropStorageItem(int nSlotID) const;
};
