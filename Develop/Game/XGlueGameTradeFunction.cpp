#include "stdafx.h"
#include "XGlueGameTradeFunction.h"
#include "XCommonFunction.h"
#include "XPost_Trade.h"
#include "XInteractionInfo.h"

void XGlueGameTradeFunction::TradeStartReq()
{
	if (XGetInteractionInfo().InteractionTargetUID ==  gvar.MyInfo.MyUID)
	{
		global.ui->OnSystemMsg(L"자기 자신에게 거래를 신청할 수 없습니다.");
	}
	else
	{
		XPostTrade_Start(XGetInteractionInfo().InteractionTargetUID);

		wstring strPlayerName = XCommonFunction::GetPlayerName(XGetInteractionInfo().GetInteractionSearchUID());
		global.ui->OnSystemMsg(L"{0}님께 거래를 신청 하였습니다.", FSParam(strPlayerName));
	}
}

void XGlueGameTradeFunction::TradeRequiredAnswer( bool bAccept )
{
	//XPostTrade_StartAccept(bAccept);

	////char text[1024]="";
	////sprintf_s(text, "거래를 수락하지 않았습니다.");

	//// TODO : 거래 요청을 받고 바로 응답을 하게 되면 필요없으니 삭제.
	//if (false == bAccept)
	//{		
	//	gvar.MyInfo.TradeContents.Reset();		
	//}
}

void XGlueGameTradeFunction::TradeItemListUp( const char* szITEMUID )
{
	// ! GetItem(MUID) 함수가 제거되어, slotID로만 접근할 수 있습니다. - praptor, 2010.03.03
	// ! 사용하지 않는 함수로 판단되어 빌드에러 막고자, 이하 주석처리합니다. - praptor, 2010.03.03


	//if( szITEMUID == NULL) return;

	//MUID uidItem = _atoi64(szITEMUID);

	////일단 수량은 모두 ㅡ,ㅡ;;
	//int nPutUpItemQuantity = 1;// = static_cast<int>(atoi(argv[2]));

	//XItem* pInvenItem = gvar.MyInfo.Inventory.GetItem(uidItem);
	//if (NULL == pInvenItem) return;

	//nPutUpItemQuantity = pInvenItem->m_nAmount;

	//if (MAX_TRADE_ITEM_SPACE <= gvar.MyInfo.TradeContents.m_MyTradeContainer.GetSize())
	//{
	//	char text[1024];
	//	sprintf_s(text, 1024, "거래창에 올릴수 있는 최대 아이템의 종류는 %d개 입니다.", MAX_TRADE_ITEM_SPACE);
	//	global.ui->OnSystemMsg(text);
	//	return;
	//}

	//int nTradeItemQuantity = 0;
	//XItem* pTradeItem = gvar.MyInfo.TradeContents.m_MyTradeContainer.GetItem(pInvenItem->m_nSlotID);
	//if (NULL != pTradeItem)
	//{
	//	nTradeItemQuantity = pTradeItem->m_nAmount;
	//}

	//if (nPutUpItemQuantity > pInvenItem->m_nAmount - nTradeItemQuantity) return;

	//TD_ITEM tdItem;
	//tdItem.m_nDurability = pInvenItem->m_nDurability;
	//tdItem.m_nQuantity = nPutUpItemQuantity;
	//tdItem.m_nItemID = pInvenItem->m_pItemData->m_nID;
	//tdItem.m_nSlotID = pInvenItem->m_nSlotID;

	//XPostTrade_PutUpItem(&tdItem);
}

void XGlueGameTradeFunction::TradeItemListDel( const char* szITEMUID )
{
	// ! GetItem(MUID) 함수가 제거되어, slotID로만 접근할 수 있습니다. - praptor, 2010.03.03
	// ! 사용하지 않는 함수로 판단되어 빌드에러 막고자, 이하 주석처리합니다. - praptor, 2010.03.03


	//if( szITEMUID == NULL) return;

	//MUID uidItem = _atoi64(szITEMUID);
	//int nPutDownItemQuantity = 1;//static_cast<int>(atoi(argv[2]));

	//int nTradingItemQuantity = 0;
	//XItem* pTradeItem = gvar.MyInfo.TradeContents.m_MyTradeContainer.GetItem(uidItem);
	//if( pTradeItem == NULL) return;

	//if (NULL != pTradeItem)
	//{
	//	nTradingItemQuantity = pTradeItem->m_nAmount;
	//}

	////수량은 모두
	//nPutDownItemQuantity = pTradeItem->m_nAmount;

	//if (nPutDownItemQuantity > nTradingItemQuantity) return;

	//TD_ITEM tdItem;
	//tdItem.m_nDurability = pTradeItem->m_nDurability;
	//tdItem.m_nQuantity = nPutDownItemQuantity;
	//tdItem.m_nItemID = pTradeItem->m_pItemData->m_nID;
	//tdItem.m_nSlotID = pTradeItem->m_nSlotID;

	//XPostTrade_PutDownItem(&tdItem);
};

void XGlueGameTradeFunction::TradePutUpSilver(int nPutUpSilver)
{
	XPostTrade_PutUpSilver(nPutUpSilver);
}

void XGlueGameTradeFunction::TradeConfirmRequire()
{
	XPostTrade_Confirm();
}

void XGlueGameTradeFunction::TradeCancel()
{
	XPostTrade_Cancel();
}
