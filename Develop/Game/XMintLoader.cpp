#include "stdafx.h"
#include "XMintLoader.h"
#include "MLookManager.h"
#include "XUIMapViewer.h"
#include "XUIControlKey.h"
#include "XUIKeyMonitor.h"
#include "XUIHDRMonitor.h"
#include "XMintScriptDeclarer.h"
#include "XWeaponBindingMenu.h"
#include "XTalentInfoMgr.h"
#include "XColorPallete.h"
// New UI widgets
#include "XUICustomLook.h"
#include "XUIEquippedItemSlot.h"
#include "XUIPaletteSlot.h"
#include "XUITalentSlot.h"
#include "XUIChattingBox.h"
#include "XUIJournalIndicator.h"
#include "XUIFadeBox.h"
#include "XUIBalloonTextBox.h"
#include "XUIMiniMap.h"
#include "XUIFieldMap.h"
#include "XUIBuffViewer.h"
#include "XUIPresentationBox.h"
#include "XUIGauge.h"
#include "XUITray.h"
#include "XUIVideo.h"
#include "MProgressCtrl.h"




XMintLoader::XMintLoader() : MintLoader()
{
}

XMintLoader::~XMintLoader()
{
}

#define MINT_XML_TAG_BUTTON				"Button"
#define MINT_XML_TAG_FRAME				"Frame"

#define MINT_XML_TAG_MAPVIEWER			"MapViewer"
#define MINT_XML_TAG_CONTROLKEY			"ControlKey"
#define MINT_XML_TAG_KEYMONITOR			"KeyMonitor"
#define MINT_XML_TAG_HDRMONITOR			"HDRMonitor"
#define MINT_XML_TAG_BUFFVIEWER			"BuffViewer"
#define MINT_XML_TAG_PALETTE			"Palette"
#define MINT_XML_TAG_TALENT_PANEL		"TalentIcon"
#define MINT_XML_TAG_WEAPONBINDING_MENU	"WeaponBindingMenu"
#define MINT_XML_TAG_COLOR_PALETTE		"ColorPallete"
#define MINT_XML_TAG_TEMPLATE			"Template"

#define MINT_XML_TAG_SHAPE				"Shape"
#define MINT_XML_TAG_FIELD				"Field"
#define MINT_XML_TAG_ITEMHEIGHT			"ItemHeight"
#define MINT_XML_TAG_REG_EQUIP			"RegEquip"
#define MINT_XML_TAG_LIST_ITEM_TYPE		"ItemType"



#define MINT_XML_ATTR_ANIMATION			"ani"
#define MINT_XML_ATTR_ANIENDATT			"endatt"

#define MINT_XML_ATTR_STAT_NORMAL		"Normal"
#define MINT_XML_ATTR_STAT_OVER			"Over"
#define MINT_XML_ATTR_STAT_PRESS		"Press"

#define MINT_XML_ATTR_NAME				"name"
#define MINT_XML_ATTR_Width				"width"

#define MINT_XML_TAG_CELL				"Cell"


MWidget* XMintLoader::ParseWidgets(MXmlElement* pElement, MWidget* pParent, const char* szWidgetName)
{
#define DECLWIDGET(_WIDGET,_NAME)	{																										\
										if ( _NAME != NULL  &&  _NAME[ 0] != 0) {															\
											decl.DeclWidget( pLua, _WIDGET->GetClassName(), _NAME, _WIDGET);								\
											_WIDGET->DeclToSctipt();																		\
									}	}


	MWidget* pNewWidget = NULL;
	pNewWidget = MintLoader::ParseWidgets(pElement, pParent, szWidgetName);
	if ( pNewWidget != NULL)		return pNewWidget;


	MWLua* pLua = (global.script) ? global.script->GetLua() : NULL;
	if (pLua == NULL)				return NULL;
	

	XMintScriptDeclarer decl;

	if (IsSameTag(pElement, MINT_XML_TAG_MAPVIEWER))
	{
		pNewWidget = NewWidget(MINT_MAPVIEWER, szWidgetName, pParent);
		DECLWIDGET( pNewWidget, szWidgetName);
		ParseMapViewer(pElement, pNewWidget);
	}
	else if (IsSameTag(pElement, MINT_XML_TAG_KEYMONITOR))
	{
		pNewWidget = NewWidget(MINT_KEYMONITOR, szWidgetName, pParent);
		DECLWIDGET( pNewWidget, szWidgetName);
		ParseKeyMonitor(pElement, pNewWidget);
	}
	else if (IsSameTag(pElement, MINT_XML_TAG_HDRMONITOR))
	{
		pNewWidget = NewWidget(MINT_HDRMONITOR, szWidgetName, pParent);
		DECLWIDGET( pNewWidget, szWidgetName);
		ParseKeyMonitor(pElement, pNewWidget);
	}
	else if (IsSameTag(pElement, MINT_XML_TAG_WEAPONBINDING_MENU))
	{
		pNewWidget = NewWidget(MINT_WEAPONBINDING_MENU, szWidgetName, pParent);
		DECLWIDGET( pNewWidget, szWidgetName);
		ParseWidget(pElement, pNewWidget);
	}
	else if (IsSameTag(pElement, MINT_XML_TAG_COLOR_PALETTE))
	{
		pNewWidget = NewWidget(MINT_COLOR_PALLETE, szWidgetName, pParent);
		DECLWIDGET( pNewWidget, szWidgetName);
		ParseColorPallete(pElement, pNewWidget);
	}

	// New UI widgets
// 	else if ( IsSameTag( pElement, MINT_INVENLISTCTRL))
// 	{
// 		pNewWidget = NewWidget( MINT_INVENLISTCTRL, szWidgetName, pParent);
// 		DECLWIDGET( pNewWidget, szWidgetName);
// 		ParseInvenListCtrl( pElement, pNewWidget);
// 	}

	else if ( IsSameTag( pElement, MINT_EQUIPPEDITEMSLOT))
	{
		pNewWidget = NewWidget( MINT_EQUIPPEDITEMSLOT, szWidgetName, pParent);
		DECLWIDGET( pNewWidget, szWidgetName);
		ParseEquippedItemSlot( pElement, pNewWidget);
	}

	else if ( IsSameTag( pElement, MINT_PALETTESLOT))
	{
		pNewWidget = NewWidget( MINT_PALETTESLOT, szWidgetName, pParent);
		DECLWIDGET( pNewWidget, szWidgetName);
		ParsePaletteSlot( pElement, pNewWidget);
	}

	else if ( IsSameTag( pElement, MINT_TALENTSLOT))
	{
		pNewWidget = NewWidget( MINT_TALENTSLOT, szWidgetName, pParent);
		DECLWIDGET( pNewWidget, szWidgetName);
		ParseTalentSlot( pElement, pNewWidget);
	}

	else if ( IsSameTag( pElement, MINT_CHATTINGBOX))
	{
		pNewWidget = NewWidget( MINT_CHATTINGBOX, szWidgetName, pParent);
		DECLWIDGET( pNewWidget, szWidgetName);
		ParseChattingBox( pElement, pNewWidget);
	}

	else if ( IsSameTag( pElement, MINT_JOURNALINDICATOR))
	{
		pNewWidget = NewWidget( MINT_JOURNALINDICATOR, szWidgetName, pParent);
		DECLWIDGET( pNewWidget, szWidgetName);
		ParseJournalIndicator( pElement, pNewWidget);
	}

	else if ( IsSameTag( pElement, MINT_FADEBOX))
	{
		pNewWidget = NewWidget( MINT_FADEBOX, szWidgetName, pParent);
		DECLWIDGET( pNewWidget, szWidgetName);
		ParseFadeBox( pElement, pNewWidget);
	}

	else if ( IsSameTag( pElement, MINT_MINIMAP))
	{
		pNewWidget = NewWidget( MINT_MINIMAP, szWidgetName, pParent);
		DECLWIDGET( pNewWidget, szWidgetName);
		ParseMiniMap( pElement, pNewWidget);
	}

	else if ( IsSameTag( pElement, MINT_FIELDMAP))
	{
		pNewWidget = NewWidget( MINT_FIELDMAP, szWidgetName, pParent);
		DECLWIDGET( pNewWidget, szWidgetName);
		ParseFieldMap( pElement, pNewWidget);
	}

	else if (IsSameTag( pElement, MINT_BUFFVIEWER))
	{
		pNewWidget = NewWidget( MINT_BUFFVIEWER, szWidgetName, pParent);
		DECLWIDGET( pNewWidget, szWidgetName);
		ParseBuffViewer( pElement, pNewWidget);
	}

	else if ( IsSameTag( pElement, MINT_PRESENTATIONBOX))
	{
		pNewWidget = NewWidget( MINT_PRESENTATIONBOX, szWidgetName, pParent);
		DECLWIDGET( pNewWidget, szWidgetName);
		ParsePresentationBox( pElement, pNewWidget);
	}

	else if ( IsSameTag( pElement, MINT_GAUGE))
	{
		pNewWidget = NewWidget( MINT_GAUGE, szWidgetName, pParent);
		DECLWIDGET( pNewWidget, szWidgetName);
		ParseGauge(pElement, pNewWidget);
	}

	else if ( IsSameTag( pElement, MINT_CONTROLKEY))
	{
		pNewWidget = NewWidget( MINT_CONTROLKEY, szWidgetName, pParent);
		DECLWIDGET( pNewWidget, szWidgetName);
		ParseControlKey( pElement, pNewWidget);
	}

	else if ( IsSameTag( pElement, MINT_TRAY))
	{
		pNewWidget = NewWidget( MINT_TRAY, szWidgetName, pParent);
		DECLWIDGET( pNewWidget, szWidgetName);
		ParseTray( pElement, pNewWidget);
	}

	else if ( IsSameTag( pElement, MINT_VIDEO))
	{
		pNewWidget = NewWidget( MINT_VIDEO, szWidgetName, pParent);
		DECLWIDGET( pNewWidget, szWidgetName);
		ParseWidget(pElement, pNewWidget);
	}

	// 아직 루아 스크립트 등록을 안했는데 연결된 이벤트 함수가 존재하면 루아에 등록시킨다
	if ( pNewWidget != NULL  &&  pNewWidget->IsDeclSctipt() == false  &&  pNewWidget->IsBindedScript())
		DECLWIDGET( pNewWidget, pNewWidget->GetName().c_str());


	return pNewWidget;
}


// ParseLook
void XMintLoader::ParseLook(MXmlElement* pElement)
{
	MintLoader::ParseLook( pElement);

	if ( IsSameTag( pElement, "ListCtrlCustomLook"))				ParseLookListCtrlCustom( pElement);
	else if ( IsSameTag( pElement, "CategoryCtrlCustomLook"))		ParseLookCategoryCtrlCustom( pElement);
	else if ( IsSameTag( pElement, "ResizableFrameCustomLook"))		ParseLookResizableFrameCustom( pElement);
	else if ( IsSameTag( pElement, "EquippedItemSlotLook"))			ParseLookEquippedItemSlot( pElement);
	else if ( IsSameTag( pElement, "PaletteSlotLook"))				ParseLookPaletteSlot( pElement);
	else if ( IsSameTag( pElement, "TalentSlotLook"))				ParseLookTalentSlot( pElement);
	else if ( IsSameTag( pElement, "BalloonTextBoxLook"))			ParseLookBalloonTextBox( pElement);
	else if ( IsSameTag( pElement, "MiniMapLook"))					ParseLookMiniMap( pElement);
	else if ( IsSameTag( pElement, "FieldMapLook"))					ParseLookFieldMap( pElement);
	else if ( IsSameTag( pElement, "BuffViewerLook"))				ParseLookBuffViewer( pElement);
	else if ( IsSameTag( pElement, "GaugeLook"))					ParseLookGauge( pElement);
}


// ParseLookListCtrlCustom
void XMintLoader::ParseLookListCtrlCustom( MXmlElement* pElement)
{
	XListCtrlCustomLook* pLook = new XListCtrlCustomLook();
	if ( ParseLookDefault( pElement, pLook) == false)
	{
		delete pLook;
		return;
	}

	for ( MXmlElement* child = pElement->FirstChildElement();  child != NULL;  child = child->NextSiblingElement())
	{
		if ( IsSameTag( child, "ItemHeight"))
		{
			int height;
			_Contents( &height, child, 0);
			pLook->SetItemHeight( height);
		}

		else if ( IsSameTag( child, "ColumnHeaderHeight"))
		{
			int height;
			_Contents( &height, child, 0);
			pLook->SetColumnHeaderHeight( height);
		}

		else if ( IsSameTag( child, "LargeIconSize"))
		{
			int w, h;
			_Attribute( &w, child, "w");
			_Attribute( &h, child, "h");
			pLook->SetLargeImageSize( w, h);
		}

		else if ( IsSameTag( child, "SmallIconSize"))
		{
			int w, h;
			_Attribute( &w, child, "w");
			_Attribute( &h, child, "h");
			pLook->SetSmallImageSize( w, h);
		}

		else if ( IsSameTag( child, "SmallFont"))
		{
			string strFont;
			_Contents( strFont, child, "", m_pXml);
			pLook->SetSmallFont( strFont.c_str());
		}

		else if ( IsSameTag( child, "ItemBkgrndExpand"))
		{
			int expand;
			_Contents( &expand, child, 1);
			pLook->SetItemBkgrndRectExpand( expand);
		}
	}
}


// ParseLookCategoryCtrlCustom
void XMintLoader::ParseLookCategoryCtrlCustom( MXmlElement* pElement)
{
	XCategoryCtrlCustomLook* pLook = new XCategoryCtrlCustomLook();
	if ( ParseLookDefault( pElement, pLook) == false)
	{
		delete pLook;
		return;
	}

	for ( MXmlElement* child = pElement->FirstChildElement();  child != NULL;  child = child->NextSiblingElement())
	{
		if ( IsSameTag( child, "Indent"))
		{
			int indent;
			_Contents( &indent, child, 10);
			pLook->SetIndent( indent);
		}

		else if ( IsSameTag( child, "ItemHeight"))
		{
			int height;
			_Contents( &height, child, 0);
			pLook->SetItemHeight( height);
		}

		else if ( IsSameTag( child, "CategoryHeight"))
		{
			int height;
			_Contents( &height, child, 0);
			pLook->SetCategoryHeight( height);
		}

		else if ( IsSameTag( child, "ExpandIconSize"))
		{
			int w, h;
			_Attribute( &w, child, "w", 16);
			_Attribute( &h, child, "h", 16);
			pLook->SetExpandIconSize( w, h);
		}

		else if ( IsSameTag( child, "ImageSize"))
		{
			int w, h;
			_Attribute( &w, child, "w", 16);
			_Attribute( &h, child, "h", 16);
			pLook->SetImageSize( w, h);
		}

		else if ( IsSameTag( child, "SmallFont"))
		{
			string strFont;
			_Contents( strFont, child, "", m_pXml);
			pLook->SetSmallFont( strFont.c_str());
		}

		else if ( IsSameTag( child, "ItemBkgrndExpand"))
		{
			int expand;
			_Contents( &expand, child, 1);
			pLook->SetItemBkgrndRectExpand( expand);
		}
	}
}


// ParseLookResizableFrameCustom
void XMintLoader::ParseLookResizableFrameCustom( MXmlElement* pElement)
{
	XResizableFrameCustomLook* pLook = new XResizableFrameCustomLook();
	if ( ParseLookDefault( pElement, pLook) == false)
	{
		delete pLook;
		return;
	}

	for ( MXmlElement* child = pElement->FirstChildElement();  child != NULL;  child = child->NextSiblingElement())
	{
		if ( IsSameTag( child, "MagneticRectOffset"))
		{
			MRECT offset( 0, 0, 0, 0);
			int val;
			if ( _Attribute( &val, child, "x", 0))			offset.x = val;
			if ( _Attribute( &val, child, "y", 0))			offset.y = val;
			if ( _Attribute( &val, child, "w", 0))			offset.w = val;
			if ( _Attribute( &val, child, "h", 0))			offset.h = val;
			if ( _Attribute( &val, child, "left", 0))		{ offset.x -= val;	offset.w += val;	}
			if ( _Attribute( &val, child, "top", 0))		{ offset.y -= val;	offset.h += val;	}
			if ( _Attribute( &val, child, "right", 0))		offset.w += val;
			if ( _Attribute( &val, child, "bottom", 0))		offset.h += val;
			pLook->SetMagneticRectOffset( offset.x, offset.y, offset.w, offset.h);
		}
	}
}


// ParseLookEquippedItemSlot
void XMintLoader::ParseLookEquippedItemSlot( MXmlElement* pElement)
{
	XEquippedItemSlotLook* pLook = new XEquippedItemSlotLook();
	if ( ParseLookDefault( pElement, pLook) == false)
	{
		delete pLook;
		return;
	}

	for ( MXmlElement* child = pElement->FirstChildElement();  child != NULL;  child = child->NextSiblingElement())
	{
		if ( IsSameTag( child, "SmallFont"))
		{
			string strFont;
			_Contents( strFont, child, "", m_pXml);
			pLook->SetSmallFont( strFont.c_str());
		}
	}
}


// ParseLookPaletteSlot
void XMintLoader::ParseLookPaletteSlot( MXmlElement* pElement)
{
	XPaletteSlotLook* pLook = new XPaletteSlotLook();
	if ( ParseLookDefault( pElement, pLook) == false)
	{
		delete pLook;
		return;
	}

	for ( MXmlElement* child = pElement->FirstChildElement();  child != NULL;  child = child->NextSiblingElement())
	{
		if ( IsSameTag( child, "LargeFont"))
		{
			string strFont;
			_Contents( strFont, child, "", m_pXml);
			pLook->SetLargeFont( strFont.c_str());
		}

		else if ( IsSameTag( child, "SmallFont"))
		{
			string strFont;
			_Contents( strFont, child, "", m_pXml);
			pLook->SetSmallFont( strFont.c_str());
		}

		else if ( IsSameTag( child, "CooltimeImage"))
		{
			string image;
			_Contents( image, child, "", m_pXml);
			pLook->SetCooltimeImage( image.c_str());
		}

		else if ( IsSameTag( child, "SliceCellCooltimeImg"))
		{
			int row, col;
			_Attribute( &row, child, "row", 1);
			_Attribute( &col, child, "col", 1);
			pLook->SliceCellCooltimeImg( row, col);
		}

		else if ( IsSameTag( child, "CooltimeImgCellCount"))
		{
			int count;
			_Contents( &count, child, 1);
			pLook->SetCooltimeImgCellCount( count);
		}
	}
}


// ParseLookTalentSlot
void XMintLoader::ParseLookTalentSlot( MXmlElement* pElement)
{
	XTalentSlotLook* pLook = new XTalentSlotLook();
	if ( ParseLookDefault( pElement, pLook) == false)
	{
		delete pLook;
		return;
	}

	for ( MXmlElement* child = pElement->FirstChildElement();  child != NULL;  child = child->NextSiblingElement())
	{
		if ( IsSameTag( child, "SmallFont"))
		{
			string strFont;
			_Contents( strFont, child, "", m_pXml);
			pLook->SetSmallFont( strFont.c_str());
		}

		else if ( IsSameTag( child, "GlowImage"))
		{
			string strImage;
			_Contents( strImage, child, "", m_pXml);
			pLook->SetGlowImage( strImage.c_str());
		}
	}
}


// ParseLookBalloonTextBox
void XMintLoader::ParseLookBalloonTextBox( MXmlElement* pElement)
{
	MTextViewLook* pLook = new MTextViewLook();
	if ( ParseLookDefault( pElement, pLook, MDEFAULT_BALLOONTEXTBOXLOOK) == false)
	{
		delete pLook;
		return;
	}

	for ( MXmlElement* child = pElement->FirstChildElement();  child != NULL;  child = child->NextSiblingElement())
	{
		if ( IsSameTag( child, "ColumnHeight"))
		{
			int nHeight;
			_Contents( &nHeight, child, 0);
			pLook->SetColumnHeight( nHeight);
		}

		else if ( IsSameTag( child, "ParagraphSpacing"))
		{
			int nSpacing;
			_Contents( &nSpacing, child, -1);
			pLook->SetParagraphSpacing( nSpacing);
		}

		else if ( IsSameTag( child, "LineSpacing"))
		{
			int nSpacing;
			_Contents( &nSpacing, child, 5);
			pLook->SetLineSpacing( nSpacing);
		}

		else if ( IsSameTag( child, "Indent"))
		{
			int nIndent;
			_Contents( &nIndent, child, 0);
			pLook->SetIndent( nIndent);
		}

		else if ( IsSameTag( child, "Secondent"))
		{
			int nSecondent;
			_Contents( &nSecondent, child, 0);
			pLook->SetSecondent( nSecondent);
		}
	}
}


// ParseLookMiniMap
void XMintLoader::ParseLookMiniMap( MXmlElement* pElement)
{
	XMiniMapLook* pLook = new XMiniMapLook();
	if ( ParseLookDefault( pElement, pLook) == false)
	{
		delete pLook;
		return;
	}

	for ( MXmlElement* child = pElement->FirstChildElement();  child != NULL;  child = child->NextSiblingElement())
	{
		if ( IsSameTag( child, "SmallFont"))
		{
			string strFont;
			_Contents( strFont, child, "", m_pXml);
			pLook->SetSmallFont( strFont.c_str());
		}

		else if ( IsSameTag( child, "MaskImage"))
		{
			string strImage;
			_Contents( strImage, child, "", m_pXml);
			pLook->SetMaskImage( strImage.c_str());
		}
	}
}


// ParseLookFieldMap
void XMintLoader::ParseLookFieldMap( MXmlElement* pElement)
{
	XFieldMapLook* pLook = new XFieldMapLook();
	if ( ParseLookDefault( pElement, pLook) == false)
	{
		delete pLook;
		return;
	}

	for ( MXmlElement* child = pElement->FirstChildElement();  child != NULL;  child = child->NextSiblingElement())
	{
		if ( IsSameTag( child, "LargeFont"))
		{
			string strFont;
			_Contents( strFont, child, "", m_pXml);
			pLook->SetLargeFont( strFont.c_str());
		}
	}
}


// ParseLookBuffViewer
void XMintLoader::ParseLookBuffViewer( MXmlElement* pElement)
{
	XBuffViewerLook* pLook = new XBuffViewerLook();
	if ( ParseLookDefault( pElement, pLook) == false)
	{
		delete pLook;
		return;
	}

	for ( MXmlElement* child = pElement->FirstChildElement();  child != NULL;  child = child->NextSiblingElement())
	{
		if ( IsSameTag( child, "SmallFont"))
		{
			string strFont;
			_Contents( strFont, child, "", m_pXml);
			pLook->SetSmallFont( strFont.c_str());
		}

		else if ( IsSameTag( child, "CooltimeImage"))
		{
			string image;
			_Contents( image, child, "", m_pXml);
			pLook->SetCooltimeImage( image.c_str());
		}

		else if ( IsSameTag( child, "SliceCellCooltimeImg"))
		{
			int row, col;
			_Attribute( &row, child, "row", 1);
			_Attribute( &col, child, "col", 1);
			pLook->SliceCellCooltimeImg( row, col);
		}

		else if ( IsSameTag( child, "CooltimeImgCellCount"))
		{
			int count;
			_Contents( &count, child, 1);
			pLook->SetCooltimeImgCellCount( count);
		}
	}
}


void XMintLoader::ParseLookGauge( MXmlElement* pElement)
{
	XGaugeLook* pLook = new XGaugeLook();
	if ( ParseLookDefault( pElement, pLook) == false)
	{
		delete pLook;
		return;
	}

	for ( MXmlElement* child = pElement->FirstChildElement();  child != NULL;  child = child->NextSiblingElement())
	{
		if ( IsSameTag( child, "SmallFont"))
		{
			string strFont;
			_Contents( strFont, child, "", m_pXml);
			pLook->SetSmallFont( strFont.c_str());
		}

		else if ( IsSameTag( child, "TextViewStyle"))
		{
			string strTextViewStyle;
			_Contents( strTextViewStyle, child, "", m_pXml);
			if ( strTextViewStyle == "percent")		pLook->SetTextViewStyle( MPCTS_PERCENT);
			else if ( strTextViewStyle == "progress")	pLook->SetTextViewStyle( MPCTS_PROGRESS);
			else												pLook->SetTextViewStyle( MPCTS_NONE);
		}
	}
}



void XMintLoader::ParseMapViewer(MXmlElement* pElement, MWidget* pWidget)
{
	ParseWidget(pElement, pWidget);
}

void XMintLoader::ParseKeyMonitor(MXmlElement* pElement, MWidget* pWidget)
{
	ParseWidget(pElement, pWidget);
}

void XMintLoader::ParseInventoryListBox(MXmlElement* pElement, MWidget* pWidget)
{
	ParseListBox( pElement, pWidget);
}

void XMintLoader::ParseMinimap(MXmlElement* pElement, MWidget* pWidget)
{
	ParseFrame(pElement, pWidget);
}

void XMintLoader::ParseColorPallete( MXmlElement* pElement, MWidget* pWidget )
{
	ParseWidget(pElement, pWidget);

	XColorPallete* pColorPallete = static_cast<XColorPallete*>(pWidget);
	
	MXmlElement* child;
	for( child = pElement->FirstChildElement(); child != NULL; child = child->NextSiblingElement() )
	{
		if (IsSameTag(child, MINT_XML_TAG_SHAPE))
		{
			MXmlElement* pShapeChildElement;
			for( pShapeChildElement = child->FirstChildElement(); pShapeChildElement != NULL; pShapeChildElement = pShapeChildElement->NextSiblingElement() )
			{
				if (IsSameTag(pShapeChildElement, MINT_XML_TAG_CELL))
				{
					int nCellWidth = 8, nCellHeight = 8, nCellMargin = 4;
					
					string strColor;

					_Attribute( &nCellWidth, pShapeChildElement, "width");
					_Attribute( &nCellHeight, pShapeChildElement, "height");
					_Attribute( &nCellMargin, pShapeChildElement, "margin");

					if (_Attribute( strColor, pShapeChildElement, "normal_border"))
					{
						uint32 nColor = strtol(strColor.c_str(), NULL, 16) | 0xFF000000;
						pColorPallete->SetNormalBorderColor(MCOLOR(nColor));
					}
					if (_Attribute( strColor, pShapeChildElement, "sel_border"))
					{
						uint32 nColor = strtol(strColor.c_str(), NULL, 16) | 0xFF000000;
						pColorPallete->SetSelBorderColor(MCOLOR(nColor));
					}
					if (_Attribute( strColor, pShapeChildElement, "focus_border"))
					{
						uint32 nColor = strtol(strColor.c_str(), NULL, 16) | 0xFF000000;
						pColorPallete->SetFocusBorderColor(MCOLOR(nColor));
					}
					

					pColorPallete->SetCellShape(nCellWidth, nCellHeight, nCellMargin);
				}
				else if (IsSameTag(pShapeChildElement, MINT_XML_TAG_TEMPLATE))
				{
					string strValue;
					if (_Contents(strValue, pShapeChildElement, MINT_XML_TAG_TEMPLATE))
					{
						pColorPallete->Assign(MLocale::ConvAnsiToUTF16(strValue.c_str()).c_str());
					}
				}

			}
		}
	}
}

// New UI widgets
// void XMintLoader::ParseInvenListCtrl( MXmlElement* pElement, MWidget* pWidget )
// {
// 	ParseListCtrl( pElement, pWidget);
// }

void XMintLoader::ParseEquippedItemSlot( MXmlElement* pElement, MWidget* pWidget )
{
	ParseWidget( pElement, pWidget);

	XUIEquippedItemSlot* pEquippedItemSlot = static_cast<XUIEquippedItemSlot*>(pWidget);
	for ( MXmlElement* child = pElement->FirstChildElement();  child != NULL;  child = child->NextSiblingElement())
	{
		if ( IsSameTag( child, "Look"))
		{
			string strName;
			_Contents( strName, child, "", m_pXml);
			MLookBase* pLook = m_pMint->GetLookManager()->Get( strName.c_str());
			if ( pLook)		pEquippedItemSlot->SetLookAndFill( pLook);
		}

		else if ( IsSameTag( child, "SlotType"))
		{
			string strSlotType;
			_Contents( strSlotType, child, "", m_pXml);
			pEquippedItemSlot->SetSlotType( MLocale::ConvAnsiToUTF16(strSlotType.c_str()).c_str());
		}

		else if ( IsSameTag( child, "DefaultImage"))
		{
			string strEmptyImage;
			_Contents( strEmptyImage, child, "", m_pXml);
			pEquippedItemSlot->SetDefaultImage( MLocale::ConvAnsiToUTF16(strEmptyImage.c_str()).c_str());
		}
	}
}

void XMintLoader::ParsePaletteSlot( MXmlElement* pElement, MWidget* pWidget )
{
	ParseWidget( pElement, pWidget);

	XUIPaletteSlot* pPaletteSlot = static_cast<XUIPaletteSlot*>(pWidget);
	for ( MXmlElement* child = pElement->FirstChildElement();  child != NULL;  child = child->NextSiblingElement())
	{
		if ( IsSameTag( child, "Look"))
		{
			string strName;
			_Contents( strName, child, "", m_pXml);
			MLookBase* pLook = m_pMint->GetLookManager()->Get( strName.c_str());
			if ( pLook)		pPaletteSlot->SetLookAndFill( pLook);
		}

		if ( IsSameTag( child, "SlotIndex"))
		{
			int nSlotIndex;
			_Contents( &nSlotIndex, child, -1);
			pPaletteSlot->SetSlotIndex( nSlotIndex);
		}
	}
}


void XMintLoader::ParseTalentSlot( MXmlElement* pElement, MWidget* pWidget )
{
	ParseWidget( pElement, pWidget);

	XUITalentSlot* pTalentSlot = static_cast<XUITalentSlot*>(pWidget);
	for ( MXmlElement* child = pElement->FirstChildElement();  child != NULL;  child = child->NextSiblingElement())
	{
		if ( IsSameTag( child, "Look"))
		{
			string strName;
			_Contents( strName, child, "", m_pXml);
			MLookBase* pLook = m_pMint->GetLookManager()->Get( strName.c_str());
			if ( pLook)		pTalentSlot->SetLookAndFill( pLook);
		}

		else if ( IsSameTag( child, "Talents"))
		{
			for ( MXmlElement* pTalents = child->FirstChildElement();  pTalents != NULL;  pTalents = pTalents->NextSiblingElement())
			{
				if ( IsSameTag( pTalents, "ID"))
				{
					int nRank, nID;
					_Attribute( &nRank, pTalents, "rank", 0);
					_Contents( &nID, pTalents, 0);
					pTalentSlot->SetTalent( nRank, nID);
				}
			}
		}

		else if ( IsSameTag( child, "Shape"))
		{
			for ( MXmlElement* pShape = child->FirstChildElement();  pShape != NULL;  pShape = pShape->NextSiblingElement())
			{
			}
		}
	}

	pTalentSlot->UpdateInfo();
}


void XMintLoader::ParseChattingBox( MXmlElement* pElement, MWidget* pWidget )
{
	ParseWidget( pElement, pWidget);

	XUIChattingBox* pChat = static_cast<XUIChattingBox*>(pWidget);
	for ( MXmlElement* child = pElement->FirstChildElement();  child != NULL;  child = child->NextSiblingElement())
	{
		if ( IsSameTag( child, "Look"))
		{
			string strType, strName;
			_Attribute( strType, child, "type", "");
			_Contents( strName, child, "", m_pXml);

			if ( strType.empty())
			{
				MLookBase* pLook = m_pMint->GetLookManager()->Get( strName.c_str());
				if ( pLook)		pChat->SetLookAndFill( pLook);
			}

			else if ( strType == "edit")
			{
				MLookBase* pLook = m_pMint->GetLookManager()->Get( strName.c_str());
				if ( pLook)		pChat->SetEditLook( pLook);
			}

			else if ( strType == "small_text")
			{
				MLookBase* pLook = m_pMint->GetLookManager()->Get( strName.c_str());
				if ( pLook)		pChat->SetTextLook( CTS_SMALL, pLook);
			}

			else if ( strType == "normal_text")
			{
				MLookBase* pLook = m_pMint->GetLookManager()->Get( strName.c_str());
				if ( pLook)		pChat->SetTextLook( CTS_NORMAL, pLook);
			}

			else if ( strType == "large_text")
			{
				MLookBase* pLook = m_pMint->GetLookManager()->Get( strName.c_str());
				if ( pLook)		pChat->SetTextLook( CTS_LARGE, pLook);
			}

			else if ( strType == "tab")
			{
				MLookBase* pLook = m_pMint->GetLookManager()->Get( strName.c_str());
				if ( pLook)		pChat->SetTabBtnLook( pLook);
			}

			else if ( strType == "adder")
			{
				MLookBase* pLook = m_pMint->GetLookManager()->Get( strName.c_str());
				if ( pLook)		pChat->SetAddBtnLook( pLook);
			}

			else if ( strType == "remover")
			{
				MLookBase* pLook = m_pMint->GetLookManager()->Get( strName.c_str());
				if ( pLook)		pChat->SetRemoveBtnLook( pLook);
			}

			else if ( strType == "property")
			{
				MLookBase* pLook = m_pMint->GetLookManager()->Get( strName.c_str());
				if ( pLook)		pChat->SetPropBtnLook( pLook);
			}

			else if ( strType == "scrollbar")
			{
				MLookBase* pLook = m_pMint->GetLookManager()->Get( strName.c_str());
				if ( pLook)		pChat->SetScrollBarLook( pLook);
			}

			else if ( strType == "endlinebtn")
			{
				MLookBase* pLook = m_pMint->GetLookManager()->Get( strName.c_str());
				if ( pLook)		pChat->SetEndLineBtnLook( pLook);
			}
		}

		else if ( IsSameTag( child, "Shape"))
		{
			for ( MXmlElement* pShape = child->FirstChildElement();  pShape != NULL;  pShape = pShape->NextSiblingElement())
			{
				if ( IsSameTag( pShape, "HeaderHeight"))
				{
					int nHeight;
					_Contents( &nHeight, pShape, 20);
					pChat->SetHeaderHeight( nHeight);
				}

				else if ( IsSameTag( pShape, "ThumbLength"))
				{
					int nLength;
					_Contents( &nLength, pShape, 16);
					pChat->SetThumbLength( nLength);
				}

				else if ( IsSameTag( pShape, "AdderSize"))
				{
					int w, h;
					_Attribute( &w, pShape, "w", 16);
					_Attribute( &h, pShape, "h", 16);
					pChat->SetAdderSize( MSize( w, h));
				}

				else if ( IsSameTag( pShape, "RemoverSize"))
				{
					int w, h;
					_Attribute( &w, pShape, "w", 16);
					_Attribute( &h, pShape, "h", 16);
					pChat->SetRemoverSize( MSize( w, h));
				}

				else if ( IsSameTag( pShape, "PropertySize"))
				{
					int w, h;
					_Attribute( &w, pShape, "w", 16);
					_Attribute( &h, pShape, "h", 16);
					pChat->SetPropertySize( MSize( w, h));
				}

				else if ( IsSameTag( pShape, "PrefixSelector"))
				{
					string strPreFix;
					_Contents( strPreFix, pShape, "", m_pXml);
					pChat->SetPrefixSelector( strPreFix.c_str());
				}

				else if ( IsSameTag( pShape, "SetIcon"))
				{
					int x, y, w, h;
					_Attribute( &x, pShape, "x", 0);
					_Attribute( &y, pShape, "y", 0);
					_Attribute( &w, pShape, "w", 32);
					_Attribute( &h, pShape, "h", 32);

					string icon;
					_Contents( icon, pShape, "", m_pXml);

					pChat->SetIcon( MBitmapManager::Get( icon.c_str()), MRECT( x, y, w, h));
				}
			}
		}

		else if ( IsSameTag( child, "Controls"))
		{
			for ( MXmlElement* pControls = child->FirstChildElement();  pControls != NULL;  pControls = pControls->NextSiblingElement())
			{
				char szName[ 256] = "";
				ParseWidgetName( szName, pControls, pWidget);

				MWidget* pNewWidget = ParseWidgets( pControls, pWidget, szName);
				if ( pNewWidget != NULL)		pNewWidget->EventLoaded();
			}
		}
	}
}


void XMintLoader::ParseJournalIndicator( MXmlElement* pElement, MWidget* pWidget )
{
	ParseWidget( pElement, pWidget);

	XUIJournalIndicator* pJournalIndicator = static_cast<XUIJournalIndicator*>(pWidget);
	for ( MXmlElement* child = pElement->FirstChildElement();  child != NULL;  child = child->NextSiblingElement())
	{
		if ( IsSameTag( child, "Look"))
		{
			string strType, strName;
			_Attribute( strType, child, "type", "");
			_Contents( strName, child, "", m_pXml);

			if ( strType.empty())
			{
				MLookBase* pLook = m_pMint->GetLookManager()->Get( strName.c_str());
				if ( pLook)		pJournalIndicator->SetLookAndFill( pLook);
			}

			else if ( strType ==  "scrollbar")
			{
				MLookBase* pLook = m_pMint->GetLookManager()->Get( strName.c_str());
				if ( pLook)		pJournalIndicator->GetScrollBar()->SetLookAndFill( pLook);
			}

			else if ( strType == "text")
			{
				MLookBase* pLook = m_pMint->GetLookManager()->Get( strName.c_str());
				if ( pLook)		pJournalIndicator->SetTextLook( pLook);
			}

			else if ( strType == "button")
			{
				MLookBase* pLook = m_pMint->GetLookManager()->Get( strName.c_str());
				if ( pLook)		pJournalIndicator->SetButtonLook( pLook);
			}
		}
	}
}


void XMintLoader::ParseFadeBox( MXmlElement* pElement, MWidget* pWidget)
{
	ParseWidget( pElement, pWidget);

	XUIFadeBox* pFadeBox = static_cast<XUIFadeBox*>(pWidget);
	for ( MXmlElement* child = pElement->FirstChildElement();  child != NULL;  child = child->NextSiblingElement())
	{
		if ( IsSameTag( child, "Look"))
		{
			string strName;
			_Contents( strName, child, "", m_pXml);
			MLookBase* pLook = m_pMint->GetLookManager()->Get( strName.c_str());
			if ( pLook)		pFadeBox->SetLookAndFill( pLook);
		}

		else if ( IsSameTag( child, "Controls"))
		{
			for ( MXmlElement* pControls = child->FirstChildElement();  pControls != NULL;  pControls = pControls->NextSiblingElement())
			{
				char szName[ 256] = "";
				ParseWidgetName( szName, pControls, pWidget);
				
				MWidget* pNewWidget = ParseWidgets( pControls, pWidget, szName);
				if ( pNewWidget != NULL)		pNewWidget->EventLoaded();
			}
		}
	}
}


void XMintLoader::ParseMiniMap( MXmlElement* pElement, MWidget* pWidget)
{
	ParseWidget( pElement, pWidget);

	XUIMiniMap* pMiniMap = static_cast<XUIMiniMap*>(pWidget);
	for ( MXmlElement* child = pElement->FirstChildElement();  child != NULL;  child = child->NextSiblingElement())
	{
		if ( IsSameTag( child, "Look"))
		{
			string strName;
			_Contents( strName, child, "", m_pXml);
			MLookBase* pLook = m_pMint->GetLookManager()->Get( strName.c_str());
			if ( pLook)		pMiniMap->SetLookAndFill( pLook);
		}
	}
}


void XMintLoader::ParseFieldMap( MXmlElement* pElement, MWidget* pWidget)
{
	ParseWidget( pElement, pWidget);

	XUIFieldMap* pFieldMap = static_cast<XUIFieldMap*>(pWidget);
	for ( MXmlElement* child = pElement->FirstChildElement();  child != NULL;  child = child->NextSiblingElement())
	{
		if ( IsSameTag( child, "Look"))
		{
			string strName;
			_Contents( strName, child, "", m_pXml);
			MLookBase* pLook = m_pMint->GetLookManager()->Get( strName.c_str());
			if ( pLook)		pFieldMap->SetLookAndFill( pLook);
		}
	}
}


void XMintLoader::ParseBuffViewer(MXmlElement* pElement, MWidget* pWidget)
{
	ParseWidget(pElement, pWidget);

	XUIBuffViewer* pBuffViewer = static_cast<XUIBuffViewer*>(pWidget);
	for ( MXmlElement* child = pElement->FirstChildElement();  child != NULL;  child = child->NextSiblingElement())
	{
		if ( IsSameTag( child, "Look"))
		{
			string strName;
			_Contents( strName, child, "", m_pXml);
			MLookBase* pLook = m_pMint->GetLookManager()->Get( strName.c_str());
			if ( pLook)		pBuffViewer->SetLookAndFill( pLook);
		}
	}
}


void XMintLoader::ParsePresentationBox(MXmlElement* pElement, MWidget* pWidget)
{
	ParseWidget(pElement, pWidget);

	XUIPresentationBox* pPresentationBox = static_cast<XUIPresentationBox*>(pWidget);
	for ( MXmlElement* child = pElement->FirstChildElement();  child != NULL;  child = child->NextSiblingElement())
	{
		if ( IsSameTag( child, "Look"))
		{
			string strType, strName;
			_Attribute( strType, child, "type", "");
			_Contents( strName, child, "", m_pXml);

			if ( strType.empty())
			{
				MLookBase* pLook = m_pMint->GetLookManager()->Get( strName.c_str());
				if ( pLook)		pPresentationBox->SetLookAndFill( pLook);
			}

			else if ( strType == "text")
			{
				MLookBase* pLook = m_pMint->GetLookManager()->Get( strName.c_str());
				if ( pLook)		pPresentationBox->SetTextLook( pLook);
			}
		}

		else if ( IsSameTag( child, "Shape"))
		{
			MXmlElement* pShape;
			for ( pShape = child->FirstChildElement();  pShape != NULL;  pShape = pShape->NextSiblingElement())
			{
				if ( IsSameTag( pShape, "FadeSecond"))
				{
					float fSecond;
					_Contents( &fSecond, pShape, 1.0f);
					pPresentationBox->SetFadeSecond( fSecond);
				}
			}
		}
	}
}


void XMintLoader::ParseGauge( MXmlElement* pElement, MWidget* pWidget)
{
	ParseWidget( pElement, pWidget);

	XUIGauge* pGauge = (XUIGauge*)pWidget;
	for ( MXmlElement* child = pElement->FirstChildElement();  child != NULL;  child = child->NextSiblingElement())
	{
		if ( IsSameTag( child, "Look"))
		{
			string strName;
			_Contents( strName, child, "", m_pXml);
			MLookBase* pLook = m_pMint->GetLookManager()->Get( strName);
			if ( pLook)		pGauge->SetLookAndFill( pLook);
		}

		else if ( IsSameTag( child, "Shape"))
		{
			MXmlElement* pShape;
			for ( pShape = child->FirstChildElement();  pShape != NULL;  pShape = pShape->NextSiblingElement())
			{
				if ( IsSameTag( pShape, "Range"))
				{
					int nLower, nUpper;
					_Attribute( &nLower, pShape, "lower", 0);
					_Attribute( &nUpper, pShape, "upper", 100);
					pGauge->SetRange( nLower, nUpper);
				}

				else if ( IsSameTag( pShape, "Pos"))
				{
					int nPos;
					_Contents( &nPos, pShape, 0);
					pGauge->SetPos( nPos);
				}

				else if ( IsSameTag( pShape, "Sliding"))
				{
					bool bEnable;
					_Contents( &bEnable, pShape, true);
					pGauge->EnableSliding( bEnable);
				}
			}
		}
	}
}


void XMintLoader::ParseControlKey(MXmlElement* pElement, MWidget* pWidget)
{
	ParseWidget(pElement, pWidget);

	XUIControlKey* pCtrlKey = (XUIControlKey*)pWidget;
	for ( MXmlElement* child = pElement->FirstChildElement();  child != NULL;  child = child->NextSiblingElement())
	{
		if ( IsSameTag( child, "Look"))
		{
			string strName;
			_Contents( strName, child, "", m_pXml);
			MLookBase* pLook = m_pMint->GetLookManager()->Get( strName);
			if ( pLook)		pCtrlKey->SetLookAndFill( pLook);
		}
	}
}


void XMintLoader::ParseTray(MXmlElement* pElement, MWidget* pWidget)
{
	ParseWidget(pElement, pWidget);

	XUITray* pTray = (XUITray*)pWidget;
	for ( MXmlElement* child = pElement->FirstChildElement();  child != NULL;  child = child->NextSiblingElement())
	{
		if ( IsSameTag( child, "Look"))
		{
			string strName;
			_Contents( strName, child, "", m_pXml);
			MLookBase* pLook = m_pMint->GetLookManager()->Get( strName);
			if ( pLook)		pTray->SetLookAndFill( pLook);
		}

		else if ( IsSameTag( child, "Shape"))
		{
			MXmlElement* pShape;
			for ( pShape = child->FirstChildElement();  pShape != NULL;  pShape = pShape->NextSiblingElement())
			{
				if ( IsSameTag( pShape, "Bind"))
				{
					string _instance;
					_Contents( _instance, pShape, "", m_pXml);
					pTray->BindScriptInstance( _instance.c_str());
				}

				else if ( IsSameTag( pShape, "HotKey"))
				{
					string strHotKey;
					_Contents( strHotKey, pShape, "", m_pXml);
					if ( strHotKey.empty() == true)
					{
						pTray->SetHotKey( 0);
						continue;
					}

					transform( strHotKey.begin(), strHotKey.end(), strHotKey.begin(), toupper);
					if ( strHotKey == "ENTER")			pTray->SetHotKey( VK_RETURN);
					else if ( strHotKey == "ESC")		pTray->SetHotKey( VK_ESCAPE);
					else if ( strHotKey == "SPACE")		pTray->SetHotKey( VK_SPACE);
					else								pTray->SetHotKey( (uint8)strHotKey.at( 0));
				}
			}
		}
	}
}


MFont* XMintLoader::CreateNewFont( const char* szName, const char* szFontName, const char* szFileName, int nHeight, bool bBold, bool bItalic, bool bAntiAlias, float fOutline, float fScale)
{
	MFont* pFont = NULL;
	if ( global.filesys->GetAccessMode() == MFILEACCESS_ENCRYPTED)
		pFont = MGetMint()->NewFontMemory( szName, szFontName, szFileName, nHeight, bBold, bItalic, bAntiAlias, fOutline, fScale);
	else
		pFont = MGetMint()->NewFont( szName, szFontName, szFileName, nHeight, bBold, bItalic, bAntiAlias, fOutline, fScale);
	return pFont;
}
