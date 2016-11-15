#ifndef _XMINT_LOADER_H
#define _XMINT_LOADER_H

#include "MintLoader.h"

using namespace mint3;

class XMintLoader : public MintLoader
{
private:

protected:
	virtual MWidget* ParseWidgets(MXmlElement* pElement, MWidget* pParent, const char* szWidgetName);
	void			ParseMapViewer(MXmlElement* pElement, MWidget* pWidget);
	void			ParseKeyMonitor(MXmlElement* pElement, MWidget* pWidget);
	void			ParseInventoryListBox(MXmlElement* pElement, MWidget* pWidget);
	void			ParseMinimap(MXmlElement* pElement, MWidget* pWidget);
	void			ParseColorPallete(MXmlElement* pElement, MWidget* pWidget);

	// New UI widgets
//	void			ParseInvenListCtrl( MXmlElement* pElement, MWidget* pWidget);
	void			ParseEquippedItemSlot( MXmlElement* pElement, MWidget* pWidget);
	void			ParsePaletteSlot( MXmlElement* pElement, MWidget* pWidget);
	void			ParseTalentSlot( MXmlElement* pElement, MWidget* pWidget);
	void			ParseChattingBox( MXmlElement* pElement, MWidget* pWidget);
	void			ParseJournalIndicator( MXmlElement* pElement, MWidget* pWidget);
	void			ParseFadeBox( MXmlElement* pElement, MWidget* pWidget);
	void			ParseMiniMap( MXmlElement* pElement, MWidget* pWidget);
	void			ParseFieldMap( MXmlElement* pElement, MWidget* pWidget);
	void			ParseBuffViewer( MXmlElement* pElement, MWidget* pWidget);
	void			ParsePresentationBox( MXmlElement* pElement, MWidget* pWidget);
	void			ParseGauge( MXmlElement* pElement, MWidget* pWidget);
	void			ParseControlKey( MXmlElement* pElement, MWidget* pWidget);
	void			ParseTray( MXmlElement* pElement, MWidget* pWidget);

	// New UI looks
	virtual void	ParseLook(MXmlElement* pElement);
	void			ParseLookListCtrlCustom( MXmlElement* pElement);
	void			ParseLookCategoryCtrlCustom( MXmlElement* pElement);
	void			ParseLookResizableFrameCustom( MXmlElement* pElement);
	void			ParseLookEquippedItemSlot( MXmlElement* pElement);
	void			ParseLookPaletteSlot( MXmlElement* pElement);
	void			ParseLookTalentSlot( MXmlElement* pElement);
	void			ParseLookBalloonTextBox( MXmlElement* pElement);
	void			ParseLookMiniMap( MXmlElement* pElement);
	void			ParseLookFieldMap( MXmlElement* pElement);
	void			ParseLookBuffViewer( MXmlElement* pElement);
	void			ParseLookGauge( MXmlElement* pElement);

	// »ý¼º
	virtual MFont*	CreateNewFont( const char* szName, const char* szFontName, const char* szFileName, int nHeight, bool bBold =false, bool bItalic =false, bool bAntiAlias =false, float fOutline =0.0f, float fScale =1.0f);


public:
	XMintLoader();
	virtual ~XMintLoader();
};





#endif