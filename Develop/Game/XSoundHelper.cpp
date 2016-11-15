#include "StdAfx.h"
#include "XSoundHelper.h"
#include "XItemManager.h"
#include "XStrings.h"
#include "XItem.h"

string XSoundHelper::GetItemPutDownSound( int nItemID )
{
	XItemData* pItemData = info.item->GetItemData( nItemID);
	if ( pItemData == NULL)		return "";

	wstring strSound;

	if ( pItemData->m_ItemSoundType != SOUND_TYPE_MAX)							strSound = L"item_" + wstring( XStrings::SoundTypeToString( pItemData->m_ItemSoundType)) + L"_down";
	if ( global.sound->IsExistSample( MLocale::ConvUTF16ToAnsi(strSound.c_str()).c_str()) == false)	strSound = L"item_universal_down";

	return MLocale::ConvUTF16ToAnsi(strSound.c_str());
}