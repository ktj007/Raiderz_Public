#pragma once


inline void XPostStorage_DepositMoney(int nMoney)
{
	XPOSTCMD1(MC_STORAGE_DEPOSIT_MONEY_REQ, MCommandParameterInt(nMoney));
}

inline void XPostStorage_WithdrawMoney(int nMoney)
{
	XPOSTCMD1(MC_STORAGE_WITHDRAW_MONEY_REQ, MCommandParameterInt(nMoney));
}