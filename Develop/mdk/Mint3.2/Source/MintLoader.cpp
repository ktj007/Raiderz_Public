#include "stdafx.h"
#include <cctype>
#include "MintLoader.h"
#include "MFileSystem.h"
#include "MFrame.h"
#include "MLabel.h"
#include "MLabelEx.h"
#include "MButton.h"
#include "MEdit.h"
#include "MEditCtrl.h"
#include "MListBox.h"
#include "MListCtrl.h"
#include "MComboBox.h"
#include "MScrollBar.h"
#include "MSpinCtrl.h"
#include "MSlider.h"
#include "MPicture.h"
#include "MGroup.h"
#include "MAnimation.h"
#include "MPanel.h"
#include "MPaint.h"
#include "MSheet.h"
#include "MTabCtrl.h"
#include "MLayer.h"
#include "MViewCtrl.h"
#include "MLinePrinter.h"
#include "MTextView.h"
#include "MTreeCtrl.h"
#include "MCategoryCtrl.h"
#include "MProgressCtrl.h"
#include "MSlideShow.h"
#include "MSelector.h"
#include "MTextFrame.h"
#include "MPopupMenu.h"
#include "MXml.h"
#include "Mint.h"
#include "MWLua.h"
#include "MScriptManager.h"
#include "MMainFrame.h"
#include "MLookManager.h"
#include "FileInfo.h"



namespace mint3 {

#define MINT_XML_TAG_UI				"UI"
#define MINT_XML_TAG_INCLUDE		"Include"
#define MINT_XML_TAG_FRAME			"Frame"
#define MINT_XML_TAG_SHEET			"Sheet"
#define MINT_XML_TAG_LABEL			"Label"
#define MINT_XML_TAG_BUTTON			"Button"
#define MINT_XML_TAG_EDIT			"Edit"
#define MINT_XML_TAG_LISTBOX		"ListBox"
#define MINT_XML_TAG_COMBOBOX		"ComboBox"
#define MINT_XML_TAG_SCROLLBAR		"ScrollBar"
#define MINT_XML_TAG_SPINCTRL		"SpinCtrl"
#define MINT_XML_TAG_SLIDER			"Slider"
#define MINT_XML_TAG_GROUP			"Group"
#define MINT_XML_TAG_PICTURE		"Picture"
#define MINT_XML_TAG_ANIMATION		"Animation"
#define MINT_XML_TAG_TIMER			"Timer"
#define MINT_XML_TAG_PANEL			"Panel"
#define MINT_XML_TAG_PAINT			"Paint"
#define MINT_XML_TAG_TABCTRL		"TabCtrl"
#define MINT_XML_TAG_VIEW			"View"
#define MINT_XML_TAG_LAYER			"Layer"
#define MINT_XML_TAG_TEXTVIEW		"TextView"
#define MINT_XML_TAG_TREECTRL		"TreeCtrl"

#define MINT_XML_TAG_INPUT			"Input"

#define MINT_XML_TAG_BITMAP			"Bitmap"
#define MINT_XML_TAG_FONT			"Font"
#define MINT_XML_TAG_ANIMATIONTEMPLATE	"AnimationTemplate"

// madduck [4/3/2006] #define
#define MINT_XML_TAG_ASSIGNMENT		"Assignment"
#define MINT_XML_TAG_LOOK			"Look"
#define MINT_XML_TAG_FRAMELOOK		"FrameLook"
#define MINT_XML_TAG_BUTTONLOOK		"ButtonLook"
#define MINT_XML_TAG_EDITLOOK		"EditLook"
#define MINT_XML_TAG_LISTBOXLOOK	"ListBoxLook"
#define MINT_XML_TAG_SCROLLBARLOOK	"ScrollBarLook"
#define MINT_XML_TAG_SPINCTRLLOOK	"SpinCtrlLook"
#define MINT_XML_TAG_GROUPLOOK		"GroupLook"
#define MINT_XML_TAG_TREECTRLLOOK	"TreeCtrlLook"
#define MINT_XML_TAG_TOOLTIPLOOK	"TooltipLook"

#define MINT_XML_TAG_LAYOUT			"Layout"
#define MINT_XML_TAG_SHAPE			"Shape"
#define MINT_XML_TAG_ACTION			"Action"
#define MINT_XML_TAG_SCRIPT			"Script"
#define MINT_XML_TAG_EVENT			"Event"
#define MINT_XML_TAG_CONTROLS		"Controls"
#define MINT_XML_TAG_RESOURCE		"Resource"
#define MINT_XML_TAG_IMAGE			"Image"
#define MINT_XML_TAG_TILED			"Tiled"

#define MINT_XML_TAG_SIZE			"Size"
#define MINT_XML_TAG_LOCATION		"Location"
#define MINT_XML_TAG_ANCHOR			"Anchor"
#define MINT_XML_TAG_TEXT			"Text"
#define MINT_XML_TAG_TOOLTIP		"ToolTip"
#define MINT_XML_TAG_HELPMSG		"HelpMsg"
#define MINT_XML_TAG_VISIBLE		"Visible"
#define MINT_XML_TAG_ENABLED		"Enabled"
#define MINT_XML_TAG_CURSOR			"Cursor"
#define MINT_XML_TAG_OPACITY		"Opacity"
#define MINT_XML_TAG_CLIPBYPARENT	"ClipByParent"
#define MINT_XML_TAG_INTERVAL		"Interval"
#define MINT_XML_TAG_TEXCOORD		"TexCoord"
#define MINT_XML_TAG_TITLEBAR		"TitleBar"
#define MINT_XML_TAG_EDITABLE		"Editable"
#define MINT_XML_TAG_SCROLLBARENABLE	"ScrollbarEnable"
#define MINT_XML_TAG_MOVEABLE		"Moveable"
#define MINT_XML_TAG_FONTCOLOR		"FontColor"
#define MINT_XML_TAG_BACKGROUNDCOLOR	"BackgroundColor"
#define MINT_XML_TAG_BORDERCOLOR	"BorderColor"
#define MINT_XML_TAG_TEXTOFFSET		"TextOffset"
#define MINT_XML_TAG_TEXTSHADOW		"TextShadow"
#define MINT_XML_TAG_TITLEOFFSET	"TitleOffset"
#define MINT_XML_TAG_TITLESHADOW	"TitleShadow"
#define MINT_XML_TAG_WINRECTOFFSET	"WinRectOffset"
#define MINT_XML_TAG_TITLEBARHEIGHT	"TitlebarHeight"
#define MINT_XML_TAG_RESIZE			"Resize"
#define MINT_XML_TAG_STYLE			"Style"
#define MINT_XML_TAG_GROUP			"Group"
#define MINT_XML_TAG_CHECKED		"Checked"
#define MINT_XML_TAG_CHILDTAB		"ChildTab"
#define MINT_XML_TAG_ALIGNMENT		"Alignment"
#define MINT_XML_TAG_NUMBER			"Number"
#define MINT_XML_TAG_LOWERCASE		"LowerCase"
#define MINT_XML_TAG_UPPERCASE		"UpperCase"
#define MINT_XML_TAG_PASSWORD		"Password"
#define MINT_XML_TAG_VISIBLEHEAD	"Visiblehead"
#define MINT_XML_TAG_FIELD			"Field"
#define MINT_XML_TAG_ITEM			"Item"
#define MINT_XML_TAG_ITEMHEIGHT		"ItemHeight"
#define MINT_XML_TAG_DROPSIZE		"DropSize"
#define MINT_XML_TAG_RANGE			"Range"
#define MINT_XML_TAG_VALUE			"Value"
#define MINT_XML_TAG_STRETCH		"Stretch"
#define MINT_XML_TAG_ROTATE			"Rotate"
#define MINT_XML_TAG_COLORIZE		"Colorize"
#define MINT_XML_TAG_MODE			"Mode"
#define MINT_XML_TAG_SCENE			"Scene"
#define MINT_XML_TAG_PLAY			"Play"
#define MINT_XML_TAG_DELAY			"Delay"
#define MINT_XML_TAG_TABPAGES		"TabPages"
#define MINT_XML_TAG_TABPAGE		"TabPage"
#define MINT_XML_TAG_TAB			"Tab"
#define MINT_XML_TAG_PAGE			"Page"
#define MINT_XML_TAG_ORIENTATION	"Orientation"
#define MINT_XML_TAG_BUDDY			"Buddy"
#define MINT_XML_TAG_WORDWRAP		"WordWrap"
#define MINT_XML_TAG_EDITABLE		"Editable"


#define MINT_XML_ATTR_SHEET			"sheet"
#define MINT_XML_ATTR_NAME			"name"
#define MINT_XML_ATTR_PARENT		"parent"
#define MINT_XML_ATTR_FILE			"file"
#define MINT_XML_ATTR_FONT			"font"

#define MINT_XML_ATTR_TYPE			"type"
#define MINT_XML_ATTR_SIDE			"side"
#define MINT_XML_ATTR_LEFT			"left"
#define MINT_XML_ATTR_RIGHT			"right"
#define MINT_XML_ATTR_TOP			"top"
#define MINT_XML_ATTR_BOTTOM		"bottom"

#define MINT_XML_ATTR_STRETCH		"stretch"
#define MINT_XML_ATTR_BUTTON		"button"
#define MINT_XML_ATTR_PANEL			"panel"

#define MINT_XML_ATTR_MIN			"min"
#define MINT_XML_ATTR_MAX			"max"

	// madduck [4/4/2006] #define
#define MINT_XML_ATTR_WIDTH			"width"
#define MINT_XML_ATTR_HEIGHT		"height"
#define MINT_XML_ATTR_OUTLINE		"outline"
#define MINT_XML_ATTR_OUTLINEOPACITY	"outlineopacity"
#define MINT_XML_ATTR_ANTIALIAS		"antialias"

#define MINT_XML_ATTR_X				"x"
#define MINT_XML_ATTR_Y				"y"
#define MINT_XML_ATTR_W				"w"
#define MINT_XML_ATTR_H				"h"

#define MINT_XML_ATTR_R				"r"
#define MINT_XML_ATTR_G				"g"
#define MINT_XML_ATTR_B				"b"
#define MINT_XML_ATTR_A				"a"

#define MINT_XML_LOOK_VALUE_NULL	"null"

// madduck [4/3/2006] #define
#define MINT_XML_ATTR_STAT_NORMAL	"Normal"
#define MINT_XML_ATTR_STAT_OVER		"Over"
#define MINT_XML_ATTR_STAT_FOCUS	"Focus"
#define MINT_XML_ATTR_STAT_PRESS	"Press"
#define MINT_XML_ATTR_STAT_DISABLE	"Disable"

#define MINT_XML_ATTR_STRING		"string"
#define MINT_XML_ATTR_ICON			"icon"

// 정렬
#define MINT_XML_ATTR_ALIGNMENT_LEFT		"left"
#define MINT_XML_ATTR_ALIGNMENT_RIGHT		"right"
#define MINT_XML_ATTR_ALIGNMENT_TOP			"top"
#define MINT_XML_ATTR_ALIGNMENT_BOTTOM		"bottom"
#define MINT_XML_ATTR_ALIGNMENT_HCENTER		"hcenter"
#define MINT_XML_ATTR_ALIGNMENT_VCENTER		"vcenter"

MintLoader::MintLoader()
{
	m_pMint = MGetMint();
	m_pXml = NULL;
}

MintLoader::~MintLoader()
{
	
}

MWidget* MintLoader::ParseWidgets(MXmlElement* pElement, MWidget* pParent, const char* szWidgetName)
{
#define DECLWIDGET(_WIDGET,_NAME)	{																										\
										if ( _NAME != NULL  &&  _NAME[ 0] != 0) {															\
											Mint::GetInstance()->GetScriptManager()->DeclWidget( _WIDGET->GetClassName(), _NAME, _WIDGET);	\
											_WIDGET->DeclToSctipt();																		\
									}	}


	MWidget* pNewWidget = NULL;

	if (IsSameTag(pElement, MINT_FRAME))
	{
		pNewWidget = NewWidget(MINT_FRAME, szWidgetName, pParent);
		DECLWIDGET( pNewWidget, szWidgetName);
		ParseFrame(pElement, pNewWidget);
	}
	else if (IsSameTag(pElement, MINT_XML_TAG_SHEET))
	{
		pNewWidget = FindSheet(string(szWidgetName));
		DECLWIDGET( pNewWidget, szWidgetName);
		ParseSheet(pElement, pNewWidget);
	}
	else if ( IsSameTag( pElement, MINT_LABEL))
	{
		pNewWidget = NewWidget( MINT_LABEL, szWidgetName, pParent);
		DECLWIDGET( pNewWidget, szWidgetName);
		ParseLabel( pElement, pNewWidget);
	}
	else if ( IsSameTag( pElement, MINT_LABELEX))
	{
		pNewWidget = NewWidget( MINT_LABELEX, szWidgetName, pParent);
		DECLWIDGET( pNewWidget, szWidgetName);
		ParseLabelEx( pElement, pNewWidget);
	}
	else if ( IsSameTag( pElement, MINT_BUTTON))
	{
		pNewWidget = NewWidget( MINT_BUTTON, szWidgetName, pParent);
		DECLWIDGET( pNewWidget, szWidgetName);
		ParseButton( pElement, pNewWidget);
	}
	else if (IsSameTag(pElement, MINT_EDIT))
	{
		pNewWidget = NewWidget( MINT_EDIT, szWidgetName, pParent);
		DECLWIDGET( pNewWidget, szWidgetName);
		ParseEdit( pElement, pNewWidget);
	}
	else if (IsSameTag(pElement, MINT_EDITCTRL))
	{
		pNewWidget = NewWidget( MINT_EDITCTRL, szWidgetName, pParent);
		DECLWIDGET( pNewWidget, szWidgetName);
		ParseEditCtrl( pElement, pNewWidget);
	}
	else if ( IsSameTag( pElement, MINT_LISTBOX))
	{
		pNewWidget = NewWidget( MINT_LISTBOX, szWidgetName, pParent);
		DECLWIDGET( pNewWidget, szWidgetName);
		ParseListBox( pElement, pNewWidget);
	}
	else if ( IsSameTag( pElement, MINT_LINEPRINTER))
	{
		pNewWidget = NewWidget( MINT_LINEPRINTER, szWidgetName, pParent);
		DECLWIDGET( pNewWidget, szWidgetName);
		ParseLinePrinter( pElement, pNewWidget);
	}
	else if ( IsSameTag( pElement, MINT_LISTCTRL))
	{
		pNewWidget = NewWidget( MINT_LISTCTRL, szWidgetName, pParent);
		DECLWIDGET( pNewWidget, szWidgetName);
		ParseListCtrl( pElement, pNewWidget);
	}
	else if ( IsSameTag( pElement, MINT_COMBOBOX))
	{
		pNewWidget = NewWidget( MINT_COMBOBOX, szWidgetName, pParent);
		DECLWIDGET( pNewWidget, szWidgetName);
		ParseComboBox( pElement, pNewWidget);
	}
	else if ( IsSameTag( pElement, MINT_SCROLLBAR))
	{
		pNewWidget = NewWidget( MINT_SCROLLBAR, szWidgetName, pParent);
		DECLWIDGET( pNewWidget, szWidgetName);
		ParseScrollBar( pElement, pNewWidget);
	}
	else if(IsSameTag(pElement, MINT_SPINCTRL))
	{
		pNewWidget = NewWidget(MINT_SPINCTRL, szWidgetName, pParent);
		DECLWIDGET( pNewWidget, szWidgetName);
		ParseSpinCtrl(pElement, pNewWidget);
	}
	else if(IsSameTag(pElement, MINT_SLIDER))
	{
		pNewWidget = NewWidget(MINT_SLIDER, szWidgetName, pParent);
		DECLWIDGET( pNewWidget, szWidgetName);
		ParseScrollBar(pElement, pNewWidget);
	}
	else if (IsSameTag(pElement, MINT_GROUP))
	{
		pNewWidget = NewWidget(MINT_GROUP, szWidgetName, pParent);
		DECLWIDGET( pNewWidget, szWidgetName);
		ParseGroup(pElement, pNewWidget);
	}
	else if (IsSameTag(pElement, MINT_PICTURE))
	{
		pNewWidget = NewWidget(MINT_PICTURE, szWidgetName, pParent);
		DECLWIDGET( pNewWidget, szWidgetName);
		ParsePicture(pElement, pNewWidget);
	}
	else if (IsSameTag(pElement, MINT_ANIMATION))
	{
		pNewWidget = NewWidget(MINT_ANIMATION, szWidgetName, pParent);
		DECLWIDGET( pNewWidget, szWidgetName);
		ParseAnimation(pElement, pNewWidget);
	}
	else if (IsSameTag(pElement, MINT_PANEL))
	{
		pNewWidget = NewWidget(MINT_PANEL, szWidgetName, pParent);
		DECLWIDGET( pNewWidget, szWidgetName);
		ParsePanel(pElement, pNewWidget);
	}
	else if ( IsSameTag( pElement, MINT_PAINT))
	{
		pNewWidget = NewWidget( MINT_PAINT, szWidgetName, pParent);
		DECLWIDGET( pNewWidget, szWidgetName);
		ParsePaint( pElement, pNewWidget);
	}
	else if ( IsSameTag( pElement, MINT_TABCTRL))
	{
		pNewWidget = NewWidget( MINT_TABCTRL, szWidgetName, pParent);
		DECLWIDGET( pNewWidget, szWidgetName);
		ParseTabCtrl( pElement, pNewWidget);
	}
	else if ( IsSameTag( pElement, MINT_LAYER))
	{
		pNewWidget = NewWidget( MINT_LAYER, szWidgetName, pParent);
		DECLWIDGET( pNewWidget, szWidgetName);
		ParseLayer( pElement, pNewWidget);
	}
	else if ( IsSameTag( pElement, MINT_TEXTVIEW))
	{
		pNewWidget = NewWidget( MINT_TEXTVIEW, szWidgetName, pParent);
		DECLWIDGET( pNewWidget, szWidgetName);
		ParseTextView( pElement, pNewWidget);
	}
	else if ( IsSameTag( pElement, MINT_TREECTRL))
	{
		pNewWidget = NewWidget( MINT_TREECTRL, szWidgetName, pParent);
		DECLWIDGET( pNewWidget, szWidgetName);
		ParseTreeCtrl( pElement, pNewWidget);
	}
	else if ( IsSameTag( pElement, MINT_CATEGORYCTRL))
	{
		pNewWidget = NewWidget( MINT_CATEGORYCTRL, szWidgetName, pParent);
		DECLWIDGET( pNewWidget, szWidgetName);
		ParseCategoryCtrl( pElement, pNewWidget);
	}
	else if ( IsSameTag( pElement, MINT_TEXTFRAME))
	{
		pNewWidget = NewWidget( MINT_TEXTFRAME, szWidgetName, pParent);
		DECLWIDGET( pNewWidget, szWidgetName);
		ParseTextFrame( pElement, pNewWidget);
	}
	else if ( IsSameTag( pElement, MINT_VIEWCTRL))
	{
		pNewWidget = NewWidget( MINT_VIEWCTRL, szWidgetName, pParent);
		DECLWIDGET( pNewWidget, szWidgetName);
		ParseViewCtrl( pElement, pNewWidget);
	}
	else if ( IsSameTag( pElement, MINT_SLIDESHOW))
	{
		pNewWidget = NewWidget( MINT_SLIDESHOW, szWidgetName, pParent);
		DECLWIDGET( pNewWidget, szWidgetName);
		ParseSlideShow( pElement, pNewWidget);
	}
	else if ( IsSameTag( pElement, MINT_PROGRESSCTRL))
	{
		pNewWidget = NewWidget( MINT_PROGRESSCTRL, szWidgetName, pParent);
		DECLWIDGET( pNewWidget, szWidgetName);
		ParseProgressCtrl( pElement, pNewWidget);
	}
	else if ( IsSameTag( pElement, MINT_SELECTOR))
	{
		pNewWidget = NewWidget( MINT_SELECTOR, szWidgetName, pParent);
		DECLWIDGET( pNewWidget, szWidgetName);
		ParseSelector( pElement, pNewWidget);
	}
	else if ( IsSameTag( pElement, MINT_POPUPMENU))
	{
		pNewWidget = NewWidget( MINT_POPUPMENU, szWidgetName, pParent);
		DECLWIDGET( pNewWidget, szWidgetName);
		ParsePopupMenu( pElement, pNewWidget);
	}


	// 아직 루아 스크립트 등록을 안했는데 연결된 이벤트 함수가 존재하면 루아에 등록시킨다
	if ( pNewWidget != NULL  &&  pNewWidget->IsDeclSctipt() == false  &&  pNewWidget->IsBindedScript())
		DECLWIDGET( pNewWidget, pNewWidget->GetName().c_str());

	return pNewWidget;
}


bool MintLoader::IsSameTag(MXmlElement* pElement, const char* szTagName)
{
	if (!_stricmp(pElement->Value(), szTagName)) return true;
	return false;
}

void MintLoader::PushXml(MXml* pXml)
{
	m_XmlStack.push_back(pXml);
	m_pXml = pXml;
}
void MintLoader::PopXml()
{
	if (!m_XmlStack.empty()) m_XmlStack.pop_back();
	if (m_pXml)
	{
		delete m_pXml; m_pXml = NULL;
	}
	if (!m_XmlStack.empty())
	{
		m_pXml = m_XmlStack.back();
	}
}

MWidget* MintLoader::FindSheet(string& strSheet)
{
	if (strSheet != "")
	{
		int nChild = m_pMint->GetMainFrame()->GetChildCount();
		for (int i = 0; i < nChild; i++)
		{
			MWidget* pChild = m_pMint->GetMainFrame()->GetChild(i);
			if (!_stricmp(pChild->GetName().c_str(), strSheet.c_str()))
			{
				if (!_stricmp(pChild->GetClassName(), MINT_SHEET)) return pChild;
			}
		}
	}
	return NewWidget(MINT_SHEET, strSheet.c_str(), m_pMint->GetMainFrame());
}

bool MintLoader::Parse()
{
	MXmlElement* pUIElement = m_pXml->Doc()->FirstChildElement(MINT_XML_TAG_UI);
	if (pUIElement == NULL) return false;

	string strSheet;
	_Attribute(strSheet, pUIElement, MINT_XML_ATTR_SHEET, "", m_pXml);
	
	MWidget* pSheetWidget = NULL;

	// sheet 이름이 없으면 sheet 를 만들지 않고 MainFrame 에 직접 붙인다
	if(strSheet.length())
	{
		m_SheetNameStack.push_back(strSheet);

		// sheet 생성
		pSheetWidget = FindSheet(strSheet);
	}

	MXmlElement* child;
	for( child = pUIElement->FirstChildElement(); child != NULL; child = child->NextSiblingElement() )
	{
		if (IsSameTag(child, MINT_XML_TAG_INCLUDE))
		{
			ParseInclude(child);
		}
		else if (IsSameTag(child, MINT_XML_TAG_SCRIPT))
		{
			ParseScript(child);
		}
		else if (IsSameTag(child, MINT_XML_TAG_RESOURCE))
		{
			ParseResource(child);
		}
		else if ( IsSameTag( child, "Reset"))
		{
			string strName;
			_Attribute( strName, child, "name", "", m_pXml);

			if ( strName.empty() == false)
			{
				MWidget* pWidget = m_pMint->FindWidget( strName);
				if ( pWidget)	ParseWidget( child, pWidget);
			}
		}
		else
		{
			char szName[256] = "";
			MWidget* pParentWidget = pSheetWidget ? pSheetWidget : m_pMint->GetMainFrame();
			MWidget* pRetParent = ParseWidgetName(szName, child, pParentWidget);
			if ( pRetParent != NULL)
				pParentWidget = pRetParent;

			MWidget* pNewWidget = ParseWidgets(child, pParentWidget, szName);
			if ( pNewWidget != NULL)	pNewWidget->EventLoaded();
		}
	}

	// 일단 모두 안보이도록 함
	if(pSheetWidget)
	{
		if (pSheetWidget->GetShow()) pSheetWidget->Show(false);
		m_SheetNameStack.pop_back();
	}

	return true;
}

bool MintLoader::Load(const char* szFileName, bool bInclude)
{
	MXml* pNewXml = new MXml();
	PushXml(pNewXml);

	if (m_pXml->LoadFile(szFileName) == false) 
	{
		PopXml();
		return false;
	}


	if (Parse() == false)
	{
		PopXml();
		return false;
	}

	PopXml();


	if ( bInclude == false)
		MGetMint()->GetMainFrame()->EventResizeLayer();

	return true;
}

bool MintLoader::LoadFromFile(const char* szFileName)
{
	char drive[_MAX_DRIVE], dir[_MAX_DIR], fname[_MAX_FNAME], ext[_MAX_EXT];
	_splitpath(szFileName, drive, dir, fname, ext);

	m_strBasePath = string(dir);

	return Load(szFileName, false);
}

bool MintLoader::LoadFromStream( const char* szStream )
{
	MXml* pNewXml = new MXml();
	PushXml(pNewXml);

	if (m_pXml->LoadStream(szStream) == false)
	{
		PopXml();
		return false;
	}

	if (Parse() == false)
	{
		PopXml();
		return false;
	}

	PopXml();
	return true;
}


void MintLoader::ParseInclude(MXmlElement* pElement)
{
	string strFileName;
	_Contents( strFileName, pElement, "", m_pXml);

	char szPathFileName[ _MAX_PATH];
	GetMergedPath( szPathFileName, m_strBasePath.c_str(), strFileName.c_str());

	if (CheckExistIncludedFile( szPathFileName) == true)
		return;

	Load( szPathFileName, true);
}

MWidget* MintLoader::NewWidget(const char* szClass, const char* szName, MWidget* pParent)
{
	MWidget* pNew = m_pMint->NewWidget(szClass, szName, pParent, pParent);
	return pNew;
}

MWidget* MintLoader::ParseWidgetName(char* szOutName, MXmlElement* pElement, MWidget* pParent)
{
	string strName;
	_Attribute(strName, pElement, MINT_XML_ATTR_NAME, "", m_pXml);

	string::size_type idx = strName.find("$parent");
	if (idx != string::npos)
	{
		strName.replace(idx, 7, pParent->GetName());
	}
	strcpy(szOutName, strName.c_str());


	string strParent;
	_Attribute(strParent, pElement, MINT_XML_ATTR_PARENT, "", m_pXml);
	if ( strParent.empty() == false)
	{
		MWidget* pWidget = m_pMint->FindWidget( strParent.c_str());
		if ( pWidget)
			return pWidget;
	}

	return NULL;
}



void MintLoader::ParseScript(MXmlElement* pElement)
{
	MWLua* pLua = m_pMint->GetScriptManager()->GetLua();
	string strFileName, strScript;

	_Attribute(strFileName, pElement, MINT_XML_ATTR_FILE, "", m_pXml);

	if (!strFileName.empty())
	{
		// 연결된 루아 스크립트 실행
		string strScriptFile = MGetPathFileNameFromRelativePath( m_strBasePath, strFileName);

		if (CheckExistIncludedFile(strScriptFile) == true)
			return;


		MFile file;
		if ( file.Open( strScriptFile.c_str()) == true)
		{
			unsigned int _size = file.GetLength();
			char* pBuffer = new char[ _size + 1];
			memset( pBuffer, 0, _size + 1);
			file.ReadAll( pBuffer, _size);
			file.Close();

			m_pMint->GetScriptManager()->RunScriptString( pBuffer);
			delete [] pBuffer;
		}

		return;
	}

	_Contents(strScript, pElement, "", m_pXml);
	if (!strScript.empty())
	{
		m_pMint->GetScriptManager()->RunScriptString(strScript.c_str());
	}
}

void MintLoader::ParseEvent(MXmlElement* pElement, MWidget* pWidget)
{
	MXmlElement* child;
	for( child = pElement->FirstChildElement(); child != NULL; child = child->NextSiblingElement() )
	{
		if ((child->Value()) && (child->GetText()))
		{
			string strEventName;
			string strScript;

			_Contents(strScript, child, "", m_pXml);
			strEventName = child->Value();

			pWidget->BindScriptEvent(strEventName, strScript);
		}			
	}
}

void MintLoader::ParseWidget(MXmlElement* pElement, MWidget* pWidget)
{
	MXmlElement* child;
	for( child = pElement->FirstChildElement(); child != NULL; child = child->NextSiblingElement() )
	{
		if (IsSameTag(child, MINT_XML_TAG_LAYOUT)  &&  pWidget->IsLayered() == false)
		{
			ParseLayout(child, pWidget);
		}
		else if (IsSameTag(child, MINT_XML_TAG_ACTION))
		{
			ParseAction(child, pWidget);
		}
		else if (IsSameTag(child, MINT_XML_TAG_SHAPE))
		{
			ParseShape(child, pWidget);
		}
		else if (IsSameTag(child, MINT_XML_TAG_SCRIPT))
		{
			ParseScript(child);
		}
		else if (IsSameTag(child, MINT_XML_TAG_EVENT))
		{
			ParseEvent(child, pWidget);
		}
	}
}

void MintLoader::ParseLayout(MXmlElement* pElement, MWidget* pWidget)
{
	int x=0,y=0,w=0,h=0;
	string strBase;

	MWidget* pParent = pWidget->GetParent();
	
	MXmlElement* child;
	for( child = pElement->FirstChildElement(); child != NULL; child = child->NextSiblingElement() )
	{
		if (IsSameTag(child, MINT_XML_TAG_SIZE))
		{
			string strW, strH;
			_Attribute( strW, child, "w");
			_Attribute( strH, child, "h");

			if ( pParent)
			{
				MRECT pr = pParent->GetWorkRect();
				if ( pParent && (strW.at( 0) == '%'))	w = (int)((float)pr.w * (float)atof(strW.c_str() + 1) / 100.0f);
				else									w = atoi( strW.c_str());
				if ( pParent && (strH.at( 0) == '%'))	h = (int)((float)pr.h * (float)atof(strH.c_str() + 1) / 100.0f);
				else									h = atoi( strH.c_str());
			}
			else
				_ASSERT( 0);
		}
		else if (IsSameTag(child, MINT_XML_TAG_LOCATION))
		{
			string strX, strY;
			_Attribute( strX, child, "x");
			_Attribute( strY, child, "y");

			if ( pParent)
			{
				MRECT pr = pParent->GetWorkRect();
				if ( pParent && (strX.at( 0) == '%'))	x = (int)((float)pr.w * (float)atof(strX.c_str() + 1) / 100.0f);
				else									x = atoi( strX.c_str());
				if ( pParent && (strY.at( 0) == '%'))	y = (int)((float)pr.h * (float)atof(strY.c_str() + 1) / 100.0f);
				else									y = atoi( strY.c_str());
			}
			else
				_ASSERT( 0);

			// base 속성
			_Attribute(strBase, child, "base", "TOPLEFT");
			transform(strBase.begin(), strBase.end(), strBase.begin(), tolower);
		}
		else if (IsSameTag(child, MINT_XML_TAG_ANCHOR))
		{
			MAnchorStyle anchor;
			_Attribute(&anchor.bLeft,	child, MINT_XML_ATTR_LEFT,	true);
			_Attribute(&anchor.bTop,	child, MINT_XML_ATTR_TOP,	true);
			_Attribute(&anchor.bRight,	child, MINT_XML_ATTR_RIGHT, false);
			_Attribute(&anchor.bBottom, child, MINT_XML_ATTR_BOTTOM,false);
			pWidget->SetAnchor( anchor);
		}
	}


	// base 위치에 따른 위치값 세팅
	if ( strBase == string("bottomright"))
	{
		x -= w;
		y -= h;
	}
	else if (strBase == string("bottomleft"))
	{
		y -= h;
	}
	else if (strBase == string("topright"))
	{
		x -= w;
	}

	pWidget->SetSize(w, h);
	pWidget->SetPosition(x, y);
}


void MintLoader::ParseShape(MXmlElement* pElement, MWidget* pWidget)
{
	MXmlElement* child;
	for( child = pElement->FirstChildElement(); child != NULL; child = child->NextSiblingElement() )
	{
		if ( IsSameTag( child, "Visible"))
		{
			bool bVisible;
			_Contents( &bVisible, child, true);
			pWidget->Show( bVisible);
		}

		else if ( IsSameTag( child, "Text"))
		{
			string strText;
			_Contents( strText, child, "", m_pXml);
			pWidget->SetText( strText.c_str());
		}

		else if ( IsSameTag( child, "ToolTip"))
		{
			string strToolTip;
			_Contents( strToolTip, child, "", m_pXml);
			pWidget->SetToolTipText( strToolTip.c_str());

			bool bOffsetValueExist = false;
			int nOffsetX = 0, nOffsetY = 0;

			if (_Attribute( &nOffsetX, child, "offset_x", 0))
			{
				bOffsetValueExist = true;
			}
			if (_Attribute( &nOffsetY, child, "offset_y", 0))
			{
				bOffsetValueExist = true;
			}

			if (bOffsetValueExist)
			{
				pWidget->SetToolTipOffset(MPOINT(nOffsetX, nOffsetY));
			}
		}

		else if ( IsSameTag( child, "Opacity"))
		{
			float fOpacity;
			_Contents( &fOpacity, child, 1.0f);
			pWidget->SetOpacity( fOpacity);
		}

		else if ( IsSameTag( child, "Movable"))
		{
			bool bMovable;
			_Contents( &bMovable, child, false);
			pWidget->SetMovable( bMovable);
		}

		else if ( IsSameTag( child, "Resize"))
		{
			bool bResize;
			_Contents( &bResize, child, false);
			pWidget->SetResizable( bResize);
		}

		else if ( IsSameTag( child, "EnableFocus"))
		{
			bool bFocus;
			_Contents( &bFocus, child, true);
			pWidget->SetEnableFocus( bFocus);
		}

		else if ( IsSameTag( child, "Static"))
		{
			bool bStatic;
			_Contents( &bStatic, child, false);
			pWidget->SetStatic( bStatic);
		}

		else if ( IsSameTag( child, "ShowBorder"))
		{
			bool bShow;
			_Contents( &bShow, child, true);
			pWidget->ShowBorder( bShow);
		}

		else if ( IsSameTag( child, "ShowScrollBar"))
		{
			MScrollBar* pScrollBar = pWidget->GetScrollBar();
			if ( pScrollBar)
			{
				string strShow;
				_Contents( strShow, child, "auto", m_pXml);
				if ( strShow == "show")			pScrollBar->SetAlwaysShow( true);
				else if ( strShow == "true")	pScrollBar->SetAlwaysShow( true);
				else if ( strShow == "hide")	pScrollBar->SetAlwaysHide( true);
				else if ( strShow == "false")	pScrollBar->SetAlwaysHide( true);
 				else if ( strShow == "auto")
				{
					pScrollBar->SetAlwaysShow( false);
					pScrollBar->SetAlwaysHide( false);
				}
			}
		}

		else if ( IsSameTag( child, "ThumbLength"))
		{
			MScrollBar* pScrollBar = pWidget->GetScrollBar();
			if ( pScrollBar)
			{
				int length;
				_Contents( &length, child, 0);
				pScrollBar->SetThumbLength( length);
			}
		}

		else if ( IsSameTag( child, "DragAndDrop"))
		{
			bool bEnable;
			_Contents( &bEnable, child, false);
			pWidget->EnableDragAndDrop( bEnable);
		}

#ifndef _PUBLISH
		else if ( IsSameTag( child, "ShowRect"))
		{
			bool bShow;
			_Contents( &bShow, child, false);
			pWidget->ShowRect( bShow);
		}
#endif
	}
}

void MintLoader::ParseAction(MXmlElement* pElement, MWidget* pWidget)
{
	bool enabled = true;

	MXmlElement* child;
	for( child = pElement->FirstChildElement(); child != NULL; child = child->NextSiblingElement() )
	{
		if (IsSameTag(child, MINT_XML_TAG_ENABLED))
		{
			_Contents(&enabled, child, true);
			pWidget->Enable(enabled);
		}
	}
}


// ParseLabel
void MintLoader::ParseLabel( MXmlElement* pElement, MWidget* pWidget)
{
	ParseWidget( pElement, pWidget);

	MLabel* pLabel = (MLabel*)pWidget;
	for ( MXmlElement* child = pElement->FirstChildElement();  child != NULL;  child = child->NextSiblingElement())
	{
		if ( IsSameTag( child, "Look"))
		{
			string strName;
			_Contents( strName, child, "", m_pXml);
			MLookBase* pLook = m_pMint->m_pLookManager->Get( strName);
			if ( pLook)		pLabel->SetLookAndFill( pLook);
		}
	}
}


// ParseLabelEx
void MintLoader::ParseLabelEx( MXmlElement* pElement, MWidget* pWidget)
{
	ParseWidget( pElement, pWidget);

	MLabelEx* pLabel = (MLabelEx*)pWidget;
	for ( MXmlElement* child = pElement->FirstChildElement();  child != NULL;  child = child->NextSiblingElement())
	{
		if ( IsSameTag( child, "Look"))
		{
			string strName;
			_Contents( strName, child, "", m_pXml);
			MLookBase* pLook = m_pMint->m_pLookManager->Get( strName);
			if ( pLook)		pLabel->SetLookAndFill( pLook);
		}

		else if ( IsSameTag( child, "Shape"))
		{
			for( MXmlElement* pShape = child->FirstChildElement();  pShape != NULL;  pShape = pShape->NextSiblingElement())
			{
				if ( IsSameTag( pShape, "Font"))
				{
					string strFont;
					_Contents( strFont, pShape, "", m_pXml);
					pLabel->SetFont( MFontManager::Get( strFont.c_str()) );
				}

				else if ( IsSameTag( pShape, "FontColor"))
				{
					int r = 255,  g = 255,  b = 255,  a = 255;
					_Attribute( &r, pShape, "r", 255);
					_Attribute( &g, pShape, "g", 255);
					_Attribute( &b, pShape, "b", 255);
					_Attribute( &a, pShape, "a", 255);
					pLabel->SetColor( MCOLOR( r, g, b, a));
				}

				else if ( IsSameTag( pShape, "Align"))
				{
					string strAlign;
					_Contents( strAlign, pShape, "left,vcenter", m_pXml);
					transform( strAlign.begin(), strAlign.end(), strAlign.begin(), tolower);
					int _align = 0;
					if ( strAlign.find( "left") != string::npos)			_align |= MAM_LEFT;
					if ( strAlign.find( "top") != string::npos)				_align |= MAM_TOP;
					if ( strAlign.find( "right") != string::npos)			_align |= MAM_RIGHT;
					if ( strAlign.find( "bottom") != string::npos)			_align |= MAM_BOTTOM;
					if ( strAlign.find( "hcenter") != string::npos)			_align |= MAM_HCENTER;
					if ( strAlign.find( "vcenter") != string::npos)			_align |= MAM_VCENTER;
					if ( strAlign.find( "justify") != string::npos)			_align |= MAM_JUSTIFY;

					pLabel->SetAlign( _align);
				}
			}
		}
	}
}


// ParseFrame
void MintLoader::ParseFrame( MXmlElement* pElement, MWidget* pWidget)
{
	ParseWidget( pElement, pWidget);

	MFrame* pFrame = (MFrame*)pWidget;
	for ( MXmlElement* child = pElement->FirstChildElement();  child != NULL;  child = child->NextSiblingElement())
	{
		if ( IsSameTag( child, "Look"))
		{
			string strName;
			_Contents( strName, child, "", m_pXml);
			MLookBase* pLook = m_pMint->m_pLookManager->Get( strName);
			if ( pLook)		pFrame->SetLookAndFill( pLook);
		}

		else if ( IsSameTag( child, "Shape"))
		{
			for( MXmlElement* pShape = child->FirstChildElement();  pShape != NULL;  pShape = pShape->NextSiblingElement())
			{
				if ( IsSameTag( pShape, "ShowTitleBar"))
				{
					bool bVisible = true;
					_Contents( &bVisible, pShape, true);
					pFrame->ShowTitlebar( bVisible);
				}

				else if ( IsSameTag( pShape, "Icon"))
				{
					string strIcon;
					_Contents( strIcon, pShape, "", m_pXml);
					MBitmap* pIcon = MBitmapManager::Get( strIcon.c_str());
					pFrame->SetIcon( pIcon);
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


// ParseButton
void MintLoader::ParseButton( MXmlElement* pElement, MWidget* pWidget)
{
	ParseWidget( pElement, pWidget);

	MButton* pButton = (MButton*)pWidget;
	for ( MXmlElement* child = pElement->FirstChildElement();  child != NULL;  child = child->NextSiblingElement())
	{
		if ( IsSameTag( child, "Look"))
		{
			string strName;
			_Contents( strName, child, "", m_pXml);
			MLookBase* pLook = m_pMint->m_pLookManager->Get( strName);
			if ( pLook)		pButton->SetLookAndFill( pLook);
		}

		else if ( IsSameTag( child, "Shape"))
		{
			for( MXmlElement* pShape = child->FirstChildElement();  pShape != NULL;  pShape = pShape->NextSiblingElement())
			{
				if ( IsSameTag( pShape, "Type"))
				{
					string strType;
					_Contents( strType, pShape, "", m_pXml);
					if ( strType == string( "checkbutton"))			pButton->SetType( MBT_CHECKBTN);
					else if ( strType == string( "pushbutton"))		pButton->SetType( MBT_PUSHBTN);
				}

				else if ( IsSameTag( pShape, "HotKey"))
				{
					string strHotKey;
					_Contents( strHotKey, pShape, "", m_pXml);
					if ( strHotKey.empty() == true)
					{
						pButton->SetHotKey( 0);
						continue;
					}

					transform( strHotKey.begin(), strHotKey.end(), strHotKey.begin(), toupper);
					if ( strHotKey == "ENTER")			pButton->SetHotKey( VK_RETURN);
					else if ( strHotKey == "ESC")		pButton->SetHotKey( VK_ESCAPE);
					else if ( strHotKey == "SPACE")		pButton->SetHotKey( VK_SPACE);
					else								pButton->SetHotKey( (unsigned char)strHotKey.at( 0));
				}

				else if ( IsSameTag( pShape, "Icon"))
				{
					int w, h;
					string strIcon;
					_Attribute( &w, pShape, "w", -1);
					_Attribute( &h, pShape, "h", -1);
					_Contents( strIcon, pShape, "", m_pXml);

					MBitmap* pIcon = MBitmapManager::Get( strIcon.c_str());
					if ( pIcon != NULL)
					{
						if ( w < 0)		w = pIcon->GetWidth();
						if ( h < 0)		h = pIcon->GetHeight();
					}
					pButton->SetIcon( pIcon, MSIZE( w, h));
				}

				else if ( IsSameTag( pShape, "Group"))
				{
					string strGroup;
					_Contents( strGroup, pShape, "", m_pXml);

					if ( strGroup.empty() == false)
					{
						MButtonGroupManager* pBGM = m_pMint->m_pButtonGroupManager;
						MButtonGroup* pButtonGroup = pBGM->Get( strGroup);
						if (pButtonGroup == NULL)
						{
							pButtonGroup = new MButtonGroup();
							pBGM->Add( strGroup, pButtonGroup);
						}
						pButton->SetButtonGroup( pButtonGroup);
					}
				}

				else if( IsSameTag( pShape, "Check"))
				{
					bool bCheck;
					_Contents( &bCheck, pShape, false);
					pButton->SetCheck( bCheck);
				}
			}
		}
	}
}


// ParseEdit
void MintLoader::ParseEdit( MXmlElement* pElement, MWidget* pWidget)
{
	ParseWidget( pElement, pWidget);

	MEdit* pEdit = (MEdit*)pWidget;
	for ( MXmlElement* child = pElement->FirstChildElement();  child != NULL;  child = child->NextSiblingElement())
	{
		if ( IsSameTag( child, "Look"))
		{
			string strName;
			_Contents( strName, child, "", m_pXml);
			MLookBase* pLook = m_pMint->m_pLookManager->Get( strName);
			if ( pLook)		pEdit->SetLookAndFill( pLook);
		}

		else if ( IsSameTag( child, "Shape"))
		{
			for ( MXmlElement*pShape = child->FirstChildElement();  pShape != NULL;  pShape = pShape->NextSiblingElement())
			{
				if ( IsSameTag( pShape, "Editable"))
				{
					bool bEditable;
					_Contents( &bEditable, pShape, true);
					pEdit->SetEditable( bEditable);
				}

				else if ( IsSameTag( pShape, "Number"))
				{
					bool bNumber;
					_Contents( &bNumber, pShape, false);
					pEdit->SetNumber( bNumber);
				}

				else if ( IsSameTag( pShape, "LowerCase"))
				{
					bool bLower;
					_Contents( &bLower, pShape, false);
					pEdit->SetLowerCase( bLower);
				}

				else if ( IsSameTag( pShape, "UpperCase"))
				{
					bool bUpper;
					_Contents( &bUpper, pShape, false);
					pEdit->SetUpperCase( bUpper);
				}

				else if ( IsSameTag( pShape, "Password"))
				{
					bool bPassword;
					_Contents( &bPassword, pShape, false);
					pEdit->SetPassword( bPassword);
				}

				else if ( IsSameTag( pShape, "LimitText"))
				{
					int nMax;
					_Contents( &nMax, pShape, 0);
					pEdit->SetLimitText( nMax);
				}

				else if ( IsSameTag( pShape, "History"))
				{
					bool bHistory;
					_Contents( &bHistory, pShape, false);
					pEdit->SetPassword( bHistory);
				}
			}
		}
	}
}


// ParseEditCtrl
void MintLoader::ParseEditCtrl( MXmlElement* pElement, MWidget* pWidget)
{
	ParseWidget( pElement, pWidget);

	MEditCtrl* pEdit = (MEditCtrl*)pWidget;
	for ( MXmlElement* child = pElement->FirstChildElement();  child != NULL;  child = child->NextSiblingElement())
	{
		if ( IsSameTag( child, "Look"))
		{
			string strType, strName;
			_Attribute( strType, child, "type", "");
			_Contents( strName, child, "", m_pXml);

			if ( strType.empty())
			{
				MLookBase* pLook = m_pMint->m_pLookManager->Get( strName);
				if ( pLook)		pEdit->SetLookAndFill( pLook);
			}

			else if ( strType == string( "scrollbar"))
			{
				MLookBase* pLook = m_pMint->m_pLookManager->Get( strName);
				if ( pLook)		pEdit->GetScrollBar()->SetLookAndFill( pLook);
			}
		}

		else if ( IsSameTag( child, "Shape"))
		{
			for ( MXmlElement*pShape = child->FirstChildElement();  pShape != NULL;  pShape = pShape->NextSiblingElement())
			{
				if ( IsSameTag( pShape, "Editable"))
				{
					bool bEditable;
					_Contents( &bEditable, pShape, true);
					pEdit->SetEditable( bEditable);
				}
			}
		}
	}
}


// ParseListBox
void MintLoader::ParseListBox( MXmlElement* pElement, MWidget* pWidget)
{
	ParseWidget( pElement, pWidget);

	MListBox* pListBox = (MListBox*)pWidget;
	for ( MXmlElement* child = pElement->FirstChildElement();  child != NULL;  child = child->NextSiblingElement())
	{
		if ( IsSameTag( child, "Look"))
		{
			string strType, strName;
			_Attribute( strType, child, "type", "");
			_Contents( strName, child, "", m_pXml);

			if ( strType.empty())
			{
				MLookBase* pLook = m_pMint->m_pLookManager->Get( strName);
				if ( pLook)		pListBox->SetLookAndFill( pLook);
			}

			else if ( strType == string( "scrollbar"))
			{
				MLookBase* pLook = m_pMint->m_pLookManager->Get( strName);
				if ( pLook)		pListBox->GetScrollBar()->SetLookAndFill( pLook);
			}
		}

		else if ( IsSameTag( child, "Shape"))
		{
			for ( MXmlElement* pShape = child->FirstChildElement();  pShape != NULL;  pShape = pShape->NextSiblingElement() )
			{
				if ( IsSameTag( pShape, "AddString"))
				{
					string strString;
					_Contents( strString, pShape, "", m_pXml);
					pListBox->AddString( strString.c_str());
				}

				else if ( IsSameTag( pShape, "SetCurSel"))
				{
					int sel;
					_Contents( &sel, pShape, 0);
					pListBox->SetCurSel( sel);
				}

				else if ( IsSameTag( pShape, "MultipleSel"))
				{
					bool bMultiSel;
					_Contents( &bMultiSel, pShape, false);
					pListBox->EnableMultipleSel( bMultiSel);
				}
			}
		}
	}
}


// ParseLinePrinter
void MintLoader::ParseLinePrinter( MXmlElement* pElement, MWidget* pWidget)
{
	ParseWidget( pElement, pWidget);

	MLinePrinter* pLinePrinter = (MLinePrinter*)pWidget;
	for ( MXmlElement* child = pElement->FirstChildElement();  child != NULL;  child = child->NextSiblingElement())
	{
		if ( IsSameTag( child, "Look"))
		{
			string strType, strName;
			_Attribute( strType, child, "type", "");
			_Contents( strName, child, "", m_pXml);

			if ( strType.empty())
			{
				MLookBase* pLook = m_pMint->m_pLookManager->Get( strName);
				if ( pLook)		pLinePrinter->SetLookAndFill( pLook);
			}

			else if ( strType == string( "scrollbar"))
			{
				MLookBase* pLook = m_pMint->m_pLookManager->Get( strName);
				if ( pLook)		pLinePrinter->GetScrollBar()->SetLookAndFill( pLook);
			}
		}

		else if ( IsSameTag( child, "Shape"))
		{
			for ( MXmlElement* pShape = child->FirstChildElement();  pShape != NULL;  pShape = pShape->NextSiblingElement() )
			{
				if ( IsSameTag( pShape, "Capacity"))
				{
					int capacity;
					_Contents( &capacity, pShape, 256);
					pLinePrinter->SetCapacity( capacity);
				}
			}
		}
	}
}


// ParseListCtrl
void MintLoader::ParseListCtrl( MXmlElement* pElement, MWidget* pWidget)
{
	ParseWidget( pElement, pWidget);

	MListCtrl* pListCtrl = (MListCtrl*)pWidget;
	for ( MXmlElement* child = pElement->FirstChildElement();  child != NULL;  child = child->NextSiblingElement())
	{
		if ( IsSameTag( child, "Look"))
		{
			string strType, strName;
			_Attribute( strType, child, "type", "");
			_Contents( strName, child, "", m_pXml);

			if ( strType.empty())
			{
				MLookBase* pLook = m_pMint->m_pLookManager->Get( strName);
				if ( pLook)		pListCtrl->SetLookAndFill( pLook);
			}

			else if ( strType == string( "scrollbar"))
			{
				MLookBase* pLook = m_pMint->m_pLookManager->Get( strName);
				if ( pLook)		pListCtrl->GetScrollBar()->SetLookAndFill( pLook);
			}
		}

		else if ( IsSameTag( child, "Shape"))
		{
			for ( MXmlElement* pShape = child->FirstChildElement();  pShape != NULL;  pShape = pShape->NextSiblingElement() )
			{
				if ( IsSameTag( pShape, "ViewStyle"))
				{
					string strViewStyle;
					_Contents( strViewStyle, pShape, "list", m_pXml);
					if ( strViewStyle == string( "icon"))			pListCtrl->SetViewStyle( MLCVS_ICON);
					else if ( strViewStyle == string( "list"))		pListCtrl->SetViewStyle( MLCVS_LIST);
					else if ( strViewStyle == string( "report"))	pListCtrl->SetViewStyle( MLCVS_REPORT);
				}

				else if ( IsSameTag( pShape, "ShowColumnHeader"))
				{
					bool bShow;
					_Contents( &bShow, pShape, true);
					pListCtrl->ShowColumnHeader( bShow);
				}

				else if ( IsSameTag( pShape, "AddColumn"))
				{
					string strAlign;
					_Attribute( strAlign, pShape, "align", "left,vcenter");
					transform( strAlign.begin(), strAlign.end(), strAlign.begin(), tolower);
					int _align = 0;
					if ( strAlign.find( "left") != string::npos)			_align |= MAM_LEFT;
					if ( strAlign.find( "top") != string::npos)				_align |= MAM_TOP;
					if ( strAlign.find( "right") != string::npos)			_align |= MAM_RIGHT;
					if ( strAlign.find( "bottom") != string::npos)			_align |= MAM_BOTTOM;
					if ( strAlign.find( "hcenter") != string::npos)			_align |= MAM_HCENTER;
					if ( strAlign.find( "vcenter") != string::npos)			_align |= MAM_VCENTER;
					if ( strAlign.find( "justify") != string::npos)			_align |= MAM_JUSTIFY;

					int width;
					_Attribute( &width, pShape, "width", 10);

					string strColumn;
					_Contents( strColumn, pShape, "", m_pXml);
					pListCtrl->AddColumn( strColumn.c_str(), _align, width);
				}

				else if ( IsSameTag( pShape, "ItemMargin"))
				{
					int nHorzSpacing, nVertSpacing;
					_Attribute( &nHorzSpacing, pShape, "horz", 2);
					_Attribute( &nVertSpacing, pShape, "vert", 2);
					pListCtrl->SetItemMargin( nHorzSpacing, nVertSpacing);
				}

				else if ( IsSameTag( pShape, "SetCurSel"))
				{
					int sel;
					_Contents( &sel, pShape, 0);
					pListCtrl->SetCurSel( sel);
				}

				else if ( IsSameTag( pShape, "MultipleSel"))
				{
					bool bMultiSel;
					_Contents( &bMultiSel, pShape, false);
					pListCtrl->EnableMultipleSel( bMultiSel);
				}

				else if ( IsSameTag( pShape, "AlwaysSel"))
				{
					bool bSel;
					_Contents( &bSel, pShape, false);
					pListCtrl->SetAlwaysSel( bSel);
				}
			}
		}
	}
}


// ParseComboBox
void MintLoader::ParseComboBox( MXmlElement* pElement, MWidget* pWidget)
{
	ParseWidget( pElement, pWidget);

	MComboBox* pCombo = (MComboBox*)pWidget;
	for ( MXmlElement* child = pElement->FirstChildElement();  child != NULL;  child = child->NextSiblingElement())
	{
		if ( IsSameTag( child, "Look"))
		{
			string strType, strName;
			_Attribute( strType, child, "type", "");
			_Contents( strName, child, "", m_pXml);

			if ( strType.empty())
			{
				MLookBase* pLook = m_pMint->m_pLookManager->Get( strName);
				if ( pLook)		pCombo->SetLookAndFill( pLook);
			}

			else if ( strType == string( "listbox"))
			{
				MLookBase* pLook = m_pMint->m_pLookManager->Get( strName);
				if ( pLook)		pCombo->GetListBox()->SetLookAndFill( pLook);
			}

			else if ( strType == string( "scrollbar"))
			{
				MLookBase* pLook = m_pMint->m_pLookManager->Get( strName);
				if ( pLook)		pCombo->GetListBox()->GetScrollBar()->SetLookAndFill( pLook);
			}
		}

		else if ( IsSameTag( child, MINT_XML_TAG_SHAPE))
		{
			for ( MXmlElement* pShape = child->FirstChildElement();  pShape != NULL;  pShape = pShape->NextSiblingElement())
			{
				if ( IsSameTag( pShape, "AddString"))
				{
					string strString;
					_Contents( strString, pShape, "", m_pXml);
					pCombo->AddString( strString.c_str());
				}

				else if ( IsSameTag( pShape, "SetCurSel"))
				{
					int sel;
					_Contents( &sel, pShape, -1);
					pCombo->SetCurSel( sel);
				}

				else if ( IsSameTag( pShape, "DroppedWidth"))
				{
					int width;
					_Contents( &width, pShape, 50);
					pCombo->SetDroppedWidth( width);
				}
			}
		}
	}
}


// ParseScrollBar
void MintLoader::ParseScrollBar( MXmlElement* pElement, MWidget* pWidget)
{
	ParseWidget( pElement, pWidget);

	MScrollBar* pScrollBar = (MScrollBar*)pWidget;
	for( MXmlElement* child = pElement->FirstChildElement();  child != NULL;  child = child->NextSiblingElement())
	{
		if ( IsSameTag( child, "Look"))
		{
			string strName;
			_Contents( strName, child, "", m_pXml);
			MLookBase* pLook = m_pMint->m_pLookManager->Get( strName);
			if ( pLook)		pScrollBar->SetLookAndFill( pLook);
		}

		else if ( IsSameTag( child, "Shape"))
		{
			for ( MXmlElement* pShape = child->FirstChildElement();  pShape != NULL;  pShape = pShape->NextSiblingElement())
			{
				if ( IsSameTag( pShape, "Style"))
				{
					string strStyle;
					_Contents( strStyle, pShape, "", m_pXml);
					if ( strStyle == string("horizontal"))			pScrollBar->SetStyle( MSBS_HORZ);
					else if ( strStyle == string("vertical"))		pScrollBar->SetStyle( MSBS_VERT);
				}

				else if ( IsSameTag( pShape, "ScrollRange"))
				{
					int min, max;
					_Attribute( &min, pShape, "min", 0);
					_Attribute( &max, pShape, "max", 100);
					pScrollBar->SetScrollRange( min, max);
				}

				else if ( IsSameTag(pShape, "ScrollValue"))
				{
					int value;
					_Contents( &value, pShape, 0);
					pScrollBar->SetScrollValue( value);
				}

				else if (IsSameTag( pShape, "ThumbLength"))
				{
					int length;
					_Contents( &length, pShape, 0);
					pScrollBar->SetThumbLength( length);
				}
			}
		}
	}
}


// ParseSpinCtrl
void MintLoader::ParseSpinCtrl( MXmlElement* pElement, MWidget* pWidget)
{
	ParseWidget( pElement, pWidget);

	MSpinCtrl* pSpin = (MSpinCtrl*)pWidget;
	for ( MXmlElement* child = pElement->FirstChildElement();  child != NULL;  child = child->NextSiblingElement())
	{
		if ( IsSameTag( child, "Look"))
		{
			string strName;
			_Contents( strName, child, "", m_pXml);
			MLookBase* pLook = m_pMint->m_pLookManager->Get( strName);
			if ( pLook)		pSpin->SetLookAndFill( pLook);
		}

		else if ( IsSameTag( child, "Shape"))
		{
			MXmlElement* pShape;
			for ( pShape = child->FirstChildElement();  pShape != NULL;  pShape = pShape->NextSiblingElement())
			{
				if ( IsSameTag( pShape, "Orientation"))
				{
					string strOrientation;
					_Contents( strOrientation, pShape, "", m_pXml);

					if ( strOrientation == string("vertical"))			pSpin->SetType( MSPT_VERTICAL);
					else if ( strOrientation == string("horizontal"))	pSpin->SetType( MSPT_HORIZONTAL);
				}

				else if ( IsSameTag(pShape, "Buddy"))
				{
					string strBuddy;
					_Contents( strBuddy, pShape, "", m_pXml);
					MWidget* pBuddy = m_pMint->FindWidget( strBuddy);
					if ( pBuddy)	pSpin->SetBuddy( pBuddy);
				}
			}
		}
	}
}


// ParseGroup
void MintLoader::ParseGroup( MXmlElement* pElement, MWidget* pWidget)
{
	ParseWidget( pElement, pWidget);

	MGroup* pGroup = (MGroup*)pWidget;
	for ( MXmlElement* child = pElement->FirstChildElement();  child != NULL;  child = child->NextSiblingElement())
	{
		if ( IsSameTag(child, "Look"))
		{
			string strName;
			_Contents( strName, child, "", m_pXml);
			MLookBase* pLook = m_pMint->m_pLookManager->Get( strName);
			if ( pLook)		pGroup->SetLookAndFill( pLook);
		}
	}
}


void MintLoader::ParsePicture(MXmlElement* pElement, MWidget* pWidget)
{
	ParseWidget(pElement, pWidget);

	MPicture* pPicture = (MPicture*)pWidget;

	MXmlElement* child;
	for( child = pElement->FirstChildElement(); child != NULL; child = child->NextSiblingElement() )
	{
		// Image 태그
		if (IsSameTag(child, MINT_XML_TAG_IMAGE))
		{
			string strImageName;
			if (_Contents(strImageName, child, "", m_pXml))
			{
				MBitmap* pBitmap = MBitmapManager::Get(strImageName.c_str());
				if (pBitmap)
					pPicture->SetBitmap(pBitmap);
			}

		}

		else if (IsSameTag(child, MINT_XML_TAG_SHAPE))
		{
			MXmlElement* pShape;
			for( pShape = child->FirstChildElement(); pShape != NULL; pShape = pShape->NextSiblingElement() )
			{
				if ( IsSameTag( pShape, MINT_XML_TAG_STRETCH))
				{
					bool bStretch;
					if ( _Contents( &bStretch, pShape, false))
						pPicture->SetStretch( bStretch);
				}

				else if ( IsSameTag( pShape, "Effect"))
				{
					string effect;
					if ( _Contents( effect, pShape, ""))
					{
						if ( effect == string( "normal"))				pPicture->SetEffect( MDE_NORMAL);
						else if ( effect == string( "add"))				pPicture->SetEffect( MDE_ADD);
						else if ( effect == string( "multiply"))		pPicture->SetEffect( MDE_MULTIPLY);
					}
				}

				else if ( IsSameTag( pShape, MINT_XML_TAG_ROTATE))
				{
					string rotate;
					if ( _Contents( rotate, pShape, ""))
					{
						if ( rotate == string( "flip horizontal"))		pPicture->SetFlipHorizontal();
						else if ( rotate == string( "flip vertical"))	pPicture->SetFlipVertical();
						else if ( rotate == string( "rotate 90CW"))		pPicture->SetRotate90CW();
						else if ( rotate == string( "rotate 90CCW"))	pPicture->SetRotate90CCW();
						else if ( rotate == string( "rotate 180"))		pPicture->SetRotate180();
					}
				}

				else if ( IsSameTag( pShape, MINT_XML_TAG_COLORIZE))
				{
					int r = 255,  g = 255,  b = 255;

					_Attribute( &r, pShape, MINT_XML_ATTR_R, 255);
					_Attribute( &g, pShape, MINT_XML_ATTR_G, 255);
					_Attribute( &b, pShape, MINT_XML_ATTR_B, 255);


					pPicture->SetColorize( MCOLOR( r, g, b));
				}
			}
		}
	}
}

void MintLoader::ParseAnimation(MXmlElement* pElement, MWidget* pWidget)
{
	ParseWidget( pElement, pWidget);

	ParsePicture( pElement, pWidget);

	MAnimation* pAnimation = (MAnimation*)pWidget;

	MXmlElement* child;
	for( child = pElement->FirstChildElement(); child != NULL; child = child->NextSiblingElement() )
	{
		if (IsSameTag(child, MINT_XML_TAG_SHAPE))
		{
			MXmlElement* pShape;
			for( pShape = child->FirstChildElement(); pShape != NULL; pShape = pShape->NextSiblingElement() )
			{
				if ( IsSameTag( pShape, MINT_XML_TAG_ANIMATIONTEMPLATE))
				{
					string strAnimationTemplate;
					if ( _Contents( strAnimationTemplate, pShape ))
					{
						MAnimationTemplate* pAnimationTemplate = MGetMint()->GetAnimationTemplateManager()->Get( strAnimationTemplate);
						pAnimation->SetAnimationTemplate( pAnimationTemplate );
					}
				}
				else if ( IsSameTag( pShape, MINT_XML_TAG_SCENE))
				{
					int nScene;
					if ( _Contents( &nScene, pShape, 0))
						pAnimation->SetCurrScene( nScene);
				}
				else if ( IsSameTag( pShape, MINT_XML_TAG_MODE))
				{
					string mode;
					if ( _Contents( mode, pShape, "forward"))
					{
						if ( mode == string( "forward"))				pAnimation->SetPlayMode( MPM_FORWARD);
						else if ( mode == string( "backward"))			pAnimation->SetPlayMode( MPM_BACKWARD);
						else if ( mode == string( "forwardonce"))		pAnimation->SetPlayMode( MPM_FORWARDONCE);
						else if ( mode == string( "backwardonce"))		pAnimation->SetPlayMode( MPM_BACKWARDONCE);
						else if ( mode == string( "forwardbackward"))	pAnimation->SetPlayMode( MPM_FORWARDBACKWARD);
					}
				}
				else if ( IsSameTag( pShape, MINT_XML_TAG_PLAY))
				{
					bool bPlay = true;
					if ( _Contents( &bPlay, pShape, true))
						pAnimation->SetPlay( bPlay);
				}
			}
		}
	}
}

void MintLoader::ParseResource(MXmlElement* pElement)
{
	MXmlElement* pChildElement = pElement->FirstChildElement();
	for( pChildElement; pChildElement != NULL; pChildElement=pChildElement->NextSiblingElement() )
	{
		if (IsSameTag(pChildElement, MINT_XML_TAG_BITMAP))
		{
			ParseBitmap(pChildElement);
		}
		else if (IsSameTag(pChildElement, MINT_XML_TAG_FONT))
		{
			ParseFont(pChildElement);
		}
		else if (IsSameTag(pChildElement, MINT_XML_TAG_CURSOR))
		{
			ParseCursor(pChildElement);
		}
		else if (IsSameTag(pChildElement, MINT_XML_TAG_ANIMATIONTEMPLATE))
		{
			ParseAnimationTemplate(pChildElement);
		}		
		// madduck [4/3/2006]
		else// if (IsSameTag(pChildElement, MINT_XML_TAG_BUTTONLOOK))
		{
			ParseLook(pChildElement);
		}
	}
}











// ParseLook
void MintLoader::ParseLook( MXmlElement* pElement)
{
	if ( IsSameTag( pElement, "Look"))						ParseLookCommon( pElement);
	else if ( IsSameTag( pElement, "LabelLook"))			ParseLookLabel( pElement);
	else if ( IsSameTag( pElement, "FrameLook"))			ParseLookFrame( pElement);
	else if ( IsSameTag( pElement, "ButtonLook"))			ParseLookButton( pElement);
	else if ( IsSameTag( pElement, "EditLook"))				ParseLookEdit( pElement);
	else if ( IsSameTag( pElement, "EditCtrlLook"))			ParseLookEditCtrl( pElement);
	else if ( IsSameTag( pElement, "ListBoxLook"))			ParseLookListBox( pElement);
	else if ( IsSameTag( pElement, "ListCtrlLook"))			ParseLookListCtrl( pElement);
	else if ( IsSameTag( pElement, "ScrollBarLook"))		ParseLookScrollBar( pElement);
	else if ( IsSameTag( pElement, "SliderLook"))			ParseLookScrollBar( pElement);
	else if ( IsSameTag( pElement, "SpinCtrlLook"))			ParseLookSpinCtrl( pElement);
	else if ( IsSameTag( pElement, "GroupLook"))			ParseLookGroup( pElement);
	else if ( IsSameTag( pElement, "TextViewLook"))			ParseLookTextView( pElement);
	else if ( IsSameTag( pElement, "ProgressCtrlLook"))		ParseLookProgressCtrl( pElement);
	else if ( IsSameTag( pElement, "TreeCtrlLook"))			ParseLookTreeCtrl( pElement);
	else if ( IsSameTag( pElement, "CategoryCtrlLook"))		ParseLookCategoryCtrl( pElement);
	else if ( IsSameTag( pElement, "SelectorLook"))			ParseLookSelector( pElement);
	else if ( IsSameTag( pElement, "ToolTipLook"))			ParseLookToolTip( pElement);
	else if ( IsSameTag( pElement, "PopupMenuLook"))		ParseLookPopupMenu( pElement);
}


// ParseLookDefault
bool MintLoader::ParseLookDefault( MXmlElement* pElement, MLookBase* pLook, const char* szFixedName)
{
	string strName;
	_Attribute ( strName, pElement, "name", "");
	if ( strName.empty())
	{
		_ASSERT( 0);
		return false;
	}

	if ( szFixedName != NULL)		strName = szFixedName;

	pLook->SetName( strName.c_str());

	if ( m_pMint->m_pLookManager->Add( strName.c_str(), pLook) == false)
		return false;


	for ( MXmlElement* child = pElement->FirstChildElement();  child != NULL;  child = child->NextSiblingElement())
	{
		if ( IsSameTag( child, "image"))
		{
			string strImage;
			_Contents( strImage, child, "", m_pXml);
			pLook->SetBitmap( strImage);
		}

		else if ( IsSameTag( child, "Font"))
		{
			string strFont;
			_Contents( strFont, child, "", m_pXml);
			MFont* pFont = MFontManager::Get( strFont.c_str());
			pLook->SetFont( pFont);
		}

		else if ( IsSameTag( child, "TextAlign"))
		{
			string strAlign;
			_Contents( strAlign, child, "", m_pXml);
			transform( strAlign.begin(), strAlign.end(), strAlign.begin(), tolower);

			int _align = 0;
			if ( strAlign.find( "left") != string::npos)			_align |= MAM_LEFT;
			if ( strAlign.find( "top") != string::npos)				_align |= MAM_TOP;
			if ( strAlign.find( "right") != string::npos)			_align |= MAM_RIGHT;
			if ( strAlign.find( "bottom") != string::npos)			_align |= MAM_BOTTOM;
			if ( strAlign.find( "hcenter") != string::npos)			_align |= MAM_HCENTER;
			if ( strAlign.find( "vcenter") != string::npos)			_align |= MAM_VCENTER;
			if ( strAlign.find( "justify") != string::npos)			_align |= MAM_JUSTIFY;
			pLook->SetTextAlign( _align);
		}

		else if ( IsSameTag( child, "Parts"))
		{
			ParseLookParts( child, pLook);
		}

		else if ( IsSameTag( child, "WorkRectOffset"))
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
			pLook->SetWorkRectOffset( offset);
		}

		else if ( IsSameTag( child, "BkgrndRectOffset"))
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
			pLook->SetBkgrndRectOffset( offset);
		}

		else if ( IsSameTag( child, "Event"))
		{
			for ( MXmlElement* pEvent = child->FirstChildElement();  pEvent != NULL;  pEvent = pEvent->NextSiblingElement())
			{
				string strEvent;
				strEvent = pEvent->Value();

				string strScript;
				_Contents( strScript, pEvent, "", m_pXml);


				if ( strEvent.empty() == false  &&  strScript.empty() == false)
					pLook->BindScriptEvent( strEvent, strScript);
			}
		}
	}

	pLook->SetStretchBlt( true);

	return true;
}


// ParseLookParts
void MintLoader::ParseLookParts( MXmlElement* pElement, MLookBase* pLook)
{
	string part;
	_Attribute( part, pElement, "part", "");
	if ( part.empty())		return;

	for ( MXmlElement* child = pElement->FirstChildElement();  child != NULL;  child = child->NextSiblingElement())
	{
		if ( IsSameTag( child, "States"))
			ParseLookStates( child, pLook, part);
	}
}


// ParseLookStates
void MintLoader::ParseLookStates( MXmlElement* pElement, MLookBase* pLook, const string& part)
{
	string state;
	_Attribute( state, pElement, "state", "");
	if ( state.empty())		return;

	for ( MXmlElement* child = pElement->FirstChildElement();  child != NULL;  child = child->NextSiblingElement())
	{
		if ( IsSameTag( child, "FontColor"))
		{
			int r, g, b, a;
			_Attribute( &r, child, "r", 255);
			_Attribute( &g, child, "g", 255);
			_Attribute( &b, child, "b", 255);
			_Attribute( &a, child, "a", 255);
			pLook->SetFontColor( part, state, MCOLOR( r, g, b, a));
		}

		else if ( IsSameTag( child, "Assign"))
		{
			int type;
			int x, y, w, h;
			bool tiled;
			_Attribute( &type, child, "type", -1);
			_Attribute( &x, child, "x", 0);
			_Attribute( &y, child, "y", 0);
			_Attribute( &w, child, "w", 0);
			_Attribute( &h, child, "h", 0);
			_Attribute( &tiled, child, "tiled", false);

			if ( type >= 0  &&  type <= 9)
				pLook->SetAssignRect( part, state, type, MRECT( x, y, w, h), tiled);
		}
	}
}


// ParseLookCommon
void MintLoader::ParseLookCommon( MXmlElement* pElement)
{
	MLookBase* pLook = new MLookBase();
	if ( ParseLookDefault( pElement, pLook) == false)		delete pLook;
}


// ParseLookLabel
void MintLoader::ParseLookLabel( MXmlElement* pElement)
{
	MLabelLook* pLook = new MLabelLook();
	if ( ParseLookDefault( pElement, pLook) == false)		delete pLook;
}


// ParseLookFrame
void MintLoader::ParseLookFrame( MXmlElement* pElement)
{
	MFrameLook* pLook = new MFrameLook();
	if ( ParseLookDefault( pElement, pLook) == false)
	{
		delete pLook;
		return;
	}

	for ( MXmlElement* child = pElement->FirstChildElement();  child != NULL;  child = child->NextSiblingElement())
	{
		if (IsSameTag(child, "TitlebarHeight"))
		{
			int h;
			_Contents( &h, child, 18);
			pLook->SetTitlebarHeight( h);
		}

		else if ( IsSameTag( child, "TitleOffset"))
		{
			int x, y;
			_Attribute( &x, child, "x", 0);
			_Attribute( &y, child, "y", 0);
			pLook->SetTitleOffset( x, y);
		}

		else if ( IsSameTag( child, "TitleShadow"))
		{
			bool bShadow = false;
			_Contents( &bShadow, child, false);
			pLook->SetTitleShadow( bShadow);
		}

		else if ( IsSameTag( child, "IconRect"))
		{
			int x, y, w, h;
			_Attribute( &x, child, "x", 0);
			_Attribute( &y, child, "y", 0);
			_Attribute( &w, child, "w", 0);
			_Attribute( &h, child, "h", 0);
			pLook->SetIconRect( x, y, w, h);
		}

		else if ( IsSameTag( child, "MagneticRectOffset"))
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


// ParseLookButton
void MintLoader::ParseLookButton( MXmlElement* pElement)
{
	MButtonLook* pLook = new MButtonLook();
	if ( ParseLookDefault( pElement, pLook) == false)
	{
		delete pLook;
		return;
	}

	for ( MXmlElement* child = pElement->FirstChildElement();  child != NULL;  child = child->NextSiblingElement())
	{
		if (IsSameTag( child, "TextOffset"))
		{
			int x, y;
			_Attribute( &x, child, "x");
			_Attribute( &y, child, "y");
			pLook->SetTextOffset( x, y);
		}

		else if ( IsSameTag( child, "ShowHotKey"))
		{
			bool bShow = true;
			_Contents( &bShow, child, true);
			pLook->ShowHotKey( bShow);
		}
	}
}


// ParseLookEdit
void MintLoader::ParseLookEdit( MXmlElement* pElement)
{
	MEditLook* pLook = new MEditLook();
	if ( ParseLookDefault( pElement, pLook) == false)		delete pLook;
}


// ParseLookEditCtrl
void MintLoader::ParseLookEditCtrl( MXmlElement* pElement)
{
	MEditCtrlLook* pLook = new MEditCtrlLook();
	if ( ParseLookDefault( pElement, pLook) == false)		delete pLook;
}


// ParseLookListBox
void MintLoader::ParseLookListBox( MXmlElement* pElement)
{
	MListBoxLook* pLook = new MListBoxLook();
	if ( ParseLookDefault( pElement, pLook) == false)
	{
		delete pLook;
		return;
	}

	for ( MXmlElement* child = pElement->FirstChildElement();  child != NULL;  child = child->NextSiblingElement())
	{
		if (IsSameTag( child, "ItemHeight"))
		{
			int height;
			_Contents( &height, child, 0);
			pLook->SetItemHeight( height);
		}
	}
}


// ParseLookListCtrl
void MintLoader::ParseLookListCtrl( MXmlElement* pElement)
{
	MListCtrlLook* pLook = new MListCtrlLook();
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
	}
}


// ParseLookScrollBar
void MintLoader::ParseLookScrollBar( MXmlElement* pElement)
{
	MScrollBarLook* pLook = new MScrollBarLook();
	if ( ParseLookDefault( pElement, pLook) == false)
	{
		delete pLook;
		return;
	}

	for ( MXmlElement* child = pElement->FirstChildElement();  child != NULL;  child = child->NextSiblingElement())
	{
		if (IsSameTag( child, "Width"))
		{
			int width;
			_Contents( &width, child, 15);
			pLook->SetWidth( width);
		}
	}
}


// ParseLookSpinCtrl
void MintLoader::ParseLookSpinCtrl( MXmlElement* pElement)
{
	MSpinCtrlLook* pLook = new MSpinCtrlLook();
	if ( ParseLookDefault( pElement, pLook) == false)
	{
		delete pLook;
		return;
	}

	for ( MXmlElement* child = pElement->FirstChildElement();  child != NULL;  child = child->NextSiblingElement())
	{
		if (IsSameTag( child, "Width"))
		{
			int width;
			_Contents( &width, child, 15);
			pLook->SetWidth( width);
		}
	}
}


// ParseLookGroup
void MintLoader::ParseLookGroup( MXmlElement* pElement)
{
	MGroupLook* pLook = new MGroupLook();
	if ( ParseLookDefault( pElement, pLook) == false)		delete pLook;
}


// ParseLookTextView
void MintLoader::ParseLookTextView( MXmlElement* pElement)
{
	MTextViewLook* pLook = new MTextViewLook();
	if ( ParseLookDefault( pElement, pLook) == false)
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


// ParseLookProgressCtrl
void MintLoader::ParseLookProgressCtrl( MXmlElement* pElement)
{
	MProgressCtrlLook* pLook = new MProgressCtrlLook();
	if ( ParseLookDefault( pElement, pLook) == false)
	{
		delete pLook;
		return;
	}

	for ( MXmlElement* child = pElement->FirstChildElement();  child != NULL;  child = child->NextSiblingElement())
	{
		if ( IsSameTag( child, "TextViewStyle"))
		{
			string strTextViewStyle;
			_Contents( strTextViewStyle, child, "", m_pXml);
			if ( strTextViewStyle == string( "percent"))		pLook->SetTextViewStyle( MPCTS_PERCENT);
			else if ( strTextViewStyle == string( "progress"))	pLook->SetTextViewStyle( MPCTS_PROGRESS);
			else												pLook->SetTextViewStyle( MPCTS_NONE);
		}
	}
}


// ParseLookTreeCtrl
void MintLoader::ParseLookTreeCtrl( MXmlElement* pElement)
{
	MTreeCtrlLook* pLook = new MTreeCtrlLook();
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
			_Contents( &indent, child, 20);
			pLook->SetIndent( indent);
		}

		else if ( IsSameTag( child, "ItemHeight"))
		{
			int height;
			_Contents( &height, child, 0);
			pLook->SetItemHeight( height);
		}

		else if ( IsSameTag( child, "IconSize"))
		{
			int w, h;
			_Attribute( &w, child, "w", 16);
			_Attribute( &h, child, "h", 16);
			pLook->SetImageSize( w, h);
		}
	}
}


// ParseLookCategoryCtrl
void MintLoader::ParseLookCategoryCtrl( MXmlElement* pElement)
{
	MCategoryCtrlLook* pLook = new MCategoryCtrlLook();
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
	}
}


// ParseLookSelector
void MintLoader::ParseLookSelector( MXmlElement* pElement)
{
	MSelectorLook* pLook = new MSelectorLook();
	if ( ParseLookDefault( pElement, pLook) == false)
	{
		delete pLook;
		return;
	}

	for ( MXmlElement* child = pElement->FirstChildElement();  child != NULL;  child = child->NextSiblingElement())
	{
	}
}


// ParseLookToolTip
void MintLoader::ParseLookToolTip( MXmlElement* pElement)
{
	MTextViewLook* pLook = new MTextViewLook();
	if ( ParseLookDefault( pElement, pLook, MDEFAULT_TOOLTIPLOOK) == false)		delete pLook;
}


// ParseLookPopupMenu
void MintLoader::ParseLookPopupMenu( MXmlElement* pElement)
{
	MPopupMenuLook* pLook = new MPopupMenuLook();
	if ( ParseLookDefault( pElement, pLook) == false)		delete pLook;
}










void MintLoader::ParseBitmap(MXmlElement* pElement)
{
	string strName, strFile;
	bool bCaching = true;

	_Attribute(strName, pElement, MINT_XML_ATTR_NAME, "");
	if (strName.empty()) return;

	_Attribute(strFile, pElement, MINT_XML_ATTR_FILE, "");
	if (strFile.empty()) return;

	if (!MGetMint()->GetDC()) return;

	// Bitmap 생성
	MBitmap* pNewBitmap = MGetMint()->NewBitmap(strName.c_str(), strFile.c_str());
	if (pNewBitmap == NULL) return;

	//중요도 높은 거는 미리 CreateTexture() 시켜도 될듯
	_Attribute(&bCaching, pElement, "caching", true);
	if (bCaching == false)
	{
		pNewBitmap->SetCaching(bCaching);
		pNewBitmap->CreateTexture();
	}

	MXmlElement* child;
	for( child = pElement->FirstChildElement(); child != NULL; child = child->NextSiblingElement() )
	{
		if (IsSameTag(child, MINT_XML_TAG_TEXCOORD))
		{
			float left = 0.0f;
			float right = 1.0f;
			float top = 0.0f;
			float bottom = 1.0f;

			_Attribute(&left, child, MINT_XML_ATTR_LEFT, 0.0f);
			_Attribute(&right, child, MINT_XML_ATTR_RIGHT, 1.0f);
			_Attribute(&top, child, MINT_XML_ATTR_TOP, 0.0f);
			_Attribute(&bottom, child, MINT_XML_ATTR_BOTTOM, 1.0f);

			pNewBitmap->SetTexCoord(left, top, right, bottom);
		}
	}

	MBitmapManager::Add(pNewBitmap);
}

void MintLoader::ParseFont(MXmlElement* pElement)
{
	// Get name
	string strAliasName;
	_Attribute(strAliasName, pElement, "name", m_pXml);
	if ( strAliasName.empty())		return;

	// Get font name
	string strFontName;
	_Attribute(strFontName, pElement, "font", m_pXml);

	// Get file name
	string strFile;
	_Attribute( strFile, pElement, "file", m_pXml);

	// Get height
	int nHeight = 10;
	_Attribute( &nHeight, pElement, "height");

	// Get bold
	bool bBold = false;
	_Attribute( &bBold, pElement, "bold");

	// Get italic
	bool bItalic = false;
	_Attribute( &bItalic, pElement, "italic");

	// Get antialias
	bool bAntiAlias = false;
	_Attribute( &bAntiAlias, pElement, "antialias");

	// Get outline
	bool bOutline = false;
	_Attribute( &bOutline, pElement, "outline");

	// Get outline opacity
	float fOutlineOpacity = 0.0f;
	_Attribute( &fOutlineOpacity, pElement, "outlineopacity", 1.0f);

	// Get outline opacity
	float fScale = 1.0f;
	_Attribute( &fScale, pElement, "scale", 1.0f);


	float fOutline = ( bOutline == true)  ?  fOutlineOpacity : 0.0f;

	// Create font
	MFont* pFont = CreateNewFont( strAliasName.c_str(), strFontName.c_str(), strFile.c_str(), nHeight,  bBold, bItalic, bAntiAlias, fOutline, fScale);
	if ( pFont != NULL)		MFontManager::Add( pFont);
}

void MintLoader::ParseCursor(MXmlElement* pElement)
{
	// Get name
	string strAliasName;
	_Attribute(strAliasName, pElement, MINT_XML_ATTR_NAME, m_pXml);
	if ( strAliasName.empty())
		return;

	MCursor* pCursor = MGetMint()->NewCursor( strAliasName.c_str() );
	if ( pCursor == NULL)		return;
	MCursorSystem::Add(pCursor);

	MCURSORTYPE cursorType = MCT_NONE;

	MXmlElement* child;
	for( child = pElement->FirstChildElement(); child != NULL; child = child->NextSiblingElement() )
	{
		if (IsSameTag(child, MINT_XML_TAG_BITMAP))
		{
			string strBitmapName;
			_Contents( strBitmapName, child);
			cursorType = MCT_BITMAP;
			MBitmap* pBitmap = pBitmap = MBitmapManager::Get(strBitmapName.c_str());
			if(pBitmap)
				pCursor->SetBitmap(pBitmap);
		}
		else if (IsSameTag(child, MINT_XML_TAG_ANIMATIONTEMPLATE))
		{
			string strAnimationTemplate;
			_Contents( strAnimationTemplate, child);
			cursorType = MCT_ANIBITMAP;
			MAnimationTemplate* pAnimationTemplate = MGetMint()->GetAnimationTemplateManager()->Get( strAnimationTemplate );
			if(pAnimationTemplate)
				pCursor->SetAnimationTemplate(pAnimationTemplate);
		}
		else if (IsSameTag(child, "HotSpot"))
		{
			MPOINT pt( 0, 0);
			_Attribute( &pt.x, child, "x", 0);
			_Attribute( &pt.y, child, "y", 0);
			pCursor->SetHotSpot( pt);
		}
	}
}

void MintLoader::ParseAnimationTemplate(MXmlElement* pElement)
{
	// Get name
	string strAliasName;
	_Attribute(strAliasName, pElement, MINT_XML_ATTR_NAME, m_pXml);
	if ( strAliasName.empty())
		return;

	if ( MGetMint()->GetAnimationTemplateManager()->Get( strAliasName) != NULL)
		return;

	MAnimationTemplate* pNewAnimationTemplate = new MAnimationTemplate;
	pNewAnimationTemplate->SetName( strAliasName );

	MXmlElement* child;
	for( child = pElement->FirstChildElement(); child != NULL; child = child->NextSiblingElement() )
	{
		// Image 태그
		if (IsSameTag(child, MINT_XML_TAG_IMAGE))
		{
			string strImageName;
			if (_Contents(strImageName, child, "", m_pXml))
			{
				MBitmap* pBitmap = MBitmapManager::Get(strImageName.c_str());
				if (pBitmap)
					pNewAnimationTemplate->SetBitmap(pBitmap);
			}
		}
		else if ( IsSameTag( child, MINT_XML_TAG_DELAY))
		{
			string delay;
			if ( _Contents( delay, child, ""))
				pNewAnimationTemplate->SetDelay( delay);
		}
	}

	MGetMint()->GetAnimationTemplateManager()->Add(pNewAnimationTemplate);
}


void MintLoader::ParsePanel(MXmlElement* pElement, MWidget* pWidget)
{
	ParseWidget(pElement, pWidget);

	MPanel* pPanel = (MPanel*)pWidget;

	MXmlElement* child;
	for( child = pElement->FirstChildElement(); child != NULL; child = child->NextSiblingElement() )
	{
		if (IsSameTag(child, MINT_XML_TAG_CONTROLS))
		{
			MXmlElement* pWidgetElement = child->FirstChildElement();
			for( pWidgetElement; pWidgetElement != NULL; pWidgetElement=pWidgetElement->NextSiblingElement() )
			{
				char szWidgetName[256];
				ParseWidgetName(szWidgetName, pWidgetElement, pWidget);

				MWidget* pNewWidget = ParseWidgets(pWidgetElement, pWidget, szWidgetName);
				if ( pNewWidget != NULL)		pNewWidget->EventLoaded();
			}
		}
	}
}

void MintLoader::ParsePaint(MXmlElement* pElement, MWidget* pWidget)
{
	ParseWidget( pElement, pWidget);

	MPaint* pPaint = (MPaint*)pWidget;
	for ( MXmlElement* child = pElement->FirstChildElement();  child != NULL;  child = child->NextSiblingElement())
	{
		if ( IsSameTag( child, "Look"))
		{
			string strName;
			_Contents( strName, child, "", m_pXml);
			MLookBase* pLook = m_pMint->m_pLookManager->Get( strName);
			if ( pLook)		pPaint->SetLookAndFill( pLook);
		}

		else if ( IsSameTag( child, "Shape"))
		{
			MXmlElement* pShape;
			for ( pShape = child->FirstChildElement();  pShape != NULL;  pShape = pShape->NextSiblingElement())
			{
				if ( IsSameTag( pShape, "Color"))
				{
					int r, g, b, a;
					_Attribute( &r, pShape, "r", 255);
					_Attribute( &g, pShape, "g", 255);
					_Attribute( &b, pShape, "b", 255);
					_Attribute( &a, pShape, "a", 255);
					pPaint->SetColor( r, g, b, a);
				}
			}
		}
	}
}

void MintLoader::ParseTabCtrl(MXmlElement* pElement, MWidget* pWidget)
{
	ParseWidget(pElement, pWidget);

	MXmlElement* child = pElement->FirstChildElement();
	for( child; child != NULL; child = child->NextSiblingElement() )
	{
		if(IsSameTag(child, MINT_XML_TAG_TABPAGES))
		{
			ParseTabPages(child, pWidget);
		}
		else if(IsSameTag(child, MINT_XML_TAG_CONTROLS))
		{
			MXmlElement* pWidgetElement = child->FirstChildElement();
			for( pWidgetElement = child->FirstChildElement(); pWidgetElement != NULL; pWidgetElement = pWidgetElement->NextSiblingElement() )
			{
				char szWidgetName[256] = "";
				ParseWidgetName(szWidgetName, pWidgetElement, pWidget);

				MWidget* pNewWidget = ParseWidgets(pWidgetElement, pWidget, szWidgetName);
				if ( pNewWidget != NULL)		pNewWidget->EventLoaded();
			}
		}
	}
}

void MintLoader::ParseTabPages( MXmlElement* pElement, MWidget* pWidget )
{
	MXmlElement* child;
	for( child = pElement->FirstChildElement(); child != NULL; child = child->NextSiblingElement() )
	{
		if(IsSameTag(child, MINT_XML_TAG_TABPAGE))
			ParseTabPage(child, pWidget);
	}
}

void MintLoader::ParseTabPage( MXmlElement* pElement, MWidget* pWidget)
{
	MWidget* pTabWidget		= NULL;
	MWidget* pPageWidget	= NULL;

	MXmlElement* child;
	for( child = pElement->FirstChildElement(); child != NULL; child = child->NextSiblingElement() )
	{

		if(IsSameTag(child, MINT_XML_TAG_TAB))
		{
			pTabWidget = NewWidget(MINT_BUTTON, NULL, pWidget);
			ParseButton(child, pTabWidget);
		}
		else if(IsSameTag(child, MINT_XML_TAG_PAGE))
		{
			char szName[ 256] = "";
			ParseWidgetName( szName, child, pWidget);
			pPageWidget = NewWidget(MINT_PANEL, szName, pWidget);
			ParsePanel(child, pPageWidget);
		}
	}

	MTabCtrl* pTabCtrl	= (MTabCtrl*)pWidget;

	if(pTabWidget && pPageWidget)
	{
		pTabCtrl->Add((MButton*)pTabWidget, (MPanel*)pPageWidget);
	}
}


void MintLoader::ParseLayer(MXmlElement* pElement, MWidget* pWidget)
{
	ParseWidget(pElement, pWidget);

	for ( MXmlElement* child = pElement->FirstChildElement();  child != NULL;  child = child->NextSiblingElement())
	{
		if ( IsSameTag( child, MINT_XML_TAG_CONTROLS))
		{
			MXmlElement* pWidgetElement;
			for ( pWidgetElement = child->FirstChildElement();  pWidgetElement != NULL;  pWidgetElement = pWidgetElement->NextSiblingElement())
			{
				char szWidgetName[256] = "";
				ParseWidgetName( szWidgetName, pWidgetElement, pWidget);
				
				MWidget* pNewWidget = ParseWidgets( pWidgetElement, pWidget, szWidgetName);
				if ( pNewWidget != NULL)		pNewWidget->EventLoaded();
			}
		}
	}
}


// ParseViewCtrl
void MintLoader::ParseViewCtrl(MXmlElement* pElement, MWidget* pWidget)
{
	ParseWidget(pElement, pWidget);

	MViewCtrl* pViewCtrl = (MViewCtrl*)pWidget;
	for ( MXmlElement* child = pElement->FirstChildElement();  child != NULL;  child = child->NextSiblingElement())
	{
		if ( IsSameTag( child, "Look"))
		{
			string strType, strName;
			_Attribute( strType, child, "type", "");
			_Contents( strName, child, "", m_pXml);

			if ( strType.empty())
			{
				MLookBase* pLook = m_pMint->m_pLookManager->Get( strName);
				if ( pLook)		pViewCtrl->SetLookAndFill( pLook);
			}

			else if ( strType == "scrollbar")
			{
				MLookBase* pLook = m_pMint->m_pLookManager->Get( strName);
				if ( pLook)		pViewCtrl->GetScrollBar()->SetLookAndFill( (MScrollBarLook*)pLook);
			}
		}

		else if ( IsSameTag( child, "Shape"))
		{
			MXmlElement* pShape;
			for ( pShape = child->FirstChildElement();  pShape != NULL;  pShape = pShape->NextSiblingElement())
			{
				if ( IsSameTag( pShape, "ScrollRange"))
				{
					int min, max;
					_Attribute( &min, pShape, "min", 0);
					_Attribute( &max, pShape, "max", 100);
					pViewCtrl->SetScrollRange( min, max);
				}

				else if ( IsSameTag( pShape, "SetScrollValue"))
				{
					int value;
					_Contents( &value, pShape, 0);
					pViewCtrl->SetScrollValue( value);
				}
			}
		}

		else if ( IsSameTag( child, "Controls"))
		{
			MXmlElement* pWidgetElement;
			for ( pWidgetElement = child->FirstChildElement();  pWidgetElement != NULL;  pWidgetElement = pWidgetElement->NextSiblingElement())
			{
				char szWidgetName[256] = "";
				ParseWidgetName( szWidgetName, pWidgetElement, pViewCtrl->GetPanel());

				MWidget* pNewWidget = ParseWidgets( pWidgetElement, pViewCtrl->GetPanel(), szWidgetName);
				if ( pNewWidget != NULL)		pNewWidget->EventLoaded();
			}
		}
	}
}


// ParseSlideShow
void MintLoader::ParseSlideShow( MXmlElement* pElement, MWidget* pWidget)
{
	ParseWidget( pElement, pWidget);

	MSlideShow* pSlideShow = (MSlideShow*)pWidget;
	for ( MXmlElement* child = pElement->FirstChildElement();  child != NULL;  child = child->NextSiblingElement())
	{
		if ( IsSameTag( child, "Look"))
		{
			string strName;
			_Contents( strName, child, "", m_pXml);

			MLookBase* pLook = m_pMint->m_pLookManager->Get( strName);
			if ( pLook)		pSlideShow->SetLookAndFill( pLook);
		}

		else if ( IsSameTag( child, "Shape"))
		{
			MXmlElement* pShape;
			for ( pShape = child->FirstChildElement();  pShape != NULL;  pShape = pShape->NextSiblingElement())
			{
				if ( IsSameTag( pShape, "Image"))
				{
					string strImage;
					_Contents( strImage, pShape, "");
					pSlideShow->SetBitmap( MBitmapManager::Get( strImage.c_str()));
				}

				else if ( IsSameTag( pShape, "SliceScene"))
				{
					int nRow, nCol;
					_Attribute( &nRow, pShape, "row", 0);
					_Attribute( &nCol, pShape, "col", 0);
					pSlideShow->SliceScene( nRow, nCol);
				}

				else if ( IsSameTag( pShape, "SceneCount"))
				{
					int nCount;
					_Contents( &nCount, pShape, 1);
					pSlideShow->SetSceneCount( nCount);
				}

				else if ( IsSameTag( pShape, "CurScene"))
				{
					int nCurScene;
					_Contents( &nCurScene, pShape, 0);
					pSlideShow->SetCurScene( nCurScene);
				}

				else if ( IsSameTag( pShape, "SlideShow"))
				{
					bool bSlideShow;
					_Contents( &bSlideShow, pShape, false);
					pSlideShow->SlideShow( bSlideShow);
				}

				else if ( IsSameTag( pShape, "DelayTime"))
				{
					int nDelayTime;
					_Contents( &nDelayTime, pShape, 3000);
					pSlideShow->SetDelayTime( nDelayTime);
				}

				else if ( IsSameTag( pShape, "FadeTime"))
				{
					int nFadeTime;
					_Contents( &nFadeTime, pShape, 1000);
					pSlideShow->SetFadeTime( nFadeTime);
				}

				else if ( IsSameTag( pShape, "PlayOnce"))
				{
					bool bPlayOnce;
					_Contents( &bPlayOnce, pShape, false);
					pSlideShow->SetPlayOnce( bPlayOnce);
				}

				else if ( IsSameTag( pShape, "Effect"))
				{
					string effect;
					if ( _Contents( effect, pShape, ""))
					{
						if ( effect == string( "normal"))				pSlideShow->SetEffect( MDE_NORMAL);
						else if ( effect == string( "add"))				pSlideShow->SetEffect( MDE_ADD);
						else if ( effect == string( "multiply"))		pSlideShow->SetEffect( MDE_MULTIPLY);
					}
				}

				else if ( IsSameTag( pShape, MINT_XML_TAG_ROTATE))
				{
					string rotate;
					if ( _Contents( rotate, pShape, ""))
					{
						if ( rotate == string( "flip horizontal"))		pSlideShow->SetFlipHorizontal();
						else if ( rotate == string( "flip vertical"))	pSlideShow->SetFlipVertical();
						else if ( rotate == string( "rotate 90CW"))		pSlideShow->SetRotate90CW();
						else if ( rotate == string( "rotate 90CCW"))	pSlideShow->SetRotate90CCW();
						else if ( rotate == string( "rotate 180"))		pSlideShow->SetRotate180();
					}
				}

				else if ( IsSameTag( pShape, MINT_XML_TAG_COLORIZE))
				{
					int r = 255,  g = 255,  b = 255;

					_Attribute( &r, pShape, MINT_XML_ATTR_R, 255);
					_Attribute( &g, pShape, MINT_XML_ATTR_G, 255);
					_Attribute( &b, pShape, MINT_XML_ATTR_B, 255);

					pSlideShow->SetColorize( MCOLOR( r, g, b));
				}
			}
		}
	}
}


// ParseProgressCtrl
void MintLoader::ParseProgressCtrl( MXmlElement* pElement, MWidget* pWidget)
{
	ParseWidget( pElement, pWidget);

	MProgressCtrl* pProgressCtrl = (MProgressCtrl*)pWidget;
	for ( MXmlElement* child = pElement->FirstChildElement();  child != NULL;  child = child->NextSiblingElement())
	{
		if ( IsSameTag( child, "Look"))
		{
			string strName;
			_Contents( strName, child, "", m_pXml);
			MLookBase* pLook = m_pMint->m_pLookManager->Get( strName);
			if ( pLook)		pProgressCtrl->SetLookAndFill( pLook);
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
					pProgressCtrl->SetRange( nLower, nUpper);
				}

				else if ( IsSameTag( pShape, "Pos"))
				{
					int nPos;
					_Contents( &nPos, pShape, 0);
					pProgressCtrl->SetPos( nPos);
				}

				else if ( IsSameTag( pShape, "Sliding"))
				{
					bool bEnable;
					_Contents( &bEnable, pShape, true);
					pProgressCtrl->EnableSliding( bEnable);
				}
			}
		}
	}
}


// ParseSelector
void MintLoader::ParseSelector( MXmlElement* pElement, MWidget* pWidget)
{
	ParseWidget( pElement, pWidget);

	MSelector* pSelector = (MSelector*)pWidget;
	for ( MXmlElement* child = pElement->FirstChildElement();  child != NULL;  child = child->NextSiblingElement())
	{
		if ( IsSameTag( child, "Look"))
		{
			string strName;
			_Contents( strName, child, "", m_pXml);
			MLookBase* pLook = m_pMint->m_pLookManager->Get( strName);
			if ( pLook)		pSelector->SetLookAndFill( pLook);
		}

		else if ( IsSameTag( child, "Shape"))
		{
			MXmlElement* pShape;
			for ( pShape = child->FirstChildElement();  pShape != NULL;  pShape = pShape->NextSiblingElement())
			{
				if ( IsSameTag( pShape, "AddItem"))
				{
					string strString;
					float fValue;
					_Attribute( strString, pShape, "string", m_pXml);
					_Attribute( &fValue, pShape, "value", 0.0f);
					pSelector->AddItem( strString.c_str(), fValue);
				}

				else if ( IsSameTag( pShape, "Style"))
				{
					string strStyle;
					_Contents( strStyle, pShape, "", m_pXml);
					if ( strStyle == string("horizontal"))			pSelector->SetStyle( MSES_HORZ);
					else if ( strStyle == string("vertical"))		pSelector->SetStyle( MSES_VERT);
				}

				else if ( IsSameTag( pShape, "SetCurSel"))
				{
					int sel;
					_Contents( &sel, pShape, 0);
					pSelector->SetCurSel( sel);
				}
			}
		}
	}
}


// ParseTextFrame
void MintLoader::ParseTextFrame( MXmlElement* pElement, MWidget* pWidget)
{
	ParseWidget( pElement, pWidget);

	MTextFrame* pTextFrame = (MTextFrame*)pWidget;
	for ( MXmlElement* child = pElement->FirstChildElement();  child != NULL;  child = child->NextSiblingElement())
	{
	}
}


// ParseTextView
void MintLoader::ParseTextView(MXmlElement* pElement, MWidget* pWidget)
{
	ParseWidget( pElement, pWidget);

	MTextView* pTextView = (MTextView*)pWidget;
	for ( MXmlElement* child = pElement->FirstChildElement();  child != NULL;  child = child->NextSiblingElement())
	{
		if ( IsSameTag( child, "Look"))
		{
			string strType, strName;
			_Attribute( strType, child, "type", "");
			_Contents( strName, child, "", m_pXml);

			if ( strType.empty())
			{
				MLookBase* pLook = m_pMint->m_pLookManager->Get( strName);
				if ( pLook)		pTextView->SetLookAndFill( pLook);
			}

			else if ( strType == "scrollbar")
			{
				MLookBase* pLook = m_pMint->m_pLookManager->Get( strName);
				if ( pLook)		pTextView->GetScrollBar()->SetLookAndFill( (MScrollBarLook*)pLook);
			}
		}

		else if ( IsSameTag( child, "Shape"))
		{
			for ( MXmlElement* pShape = child->FirstChildElement();  pShape != NULL;  pShape = pShape->NextSiblingElement())
			{
				if ( IsSameTag( pShape, "Wrap"))
				{
					bool bWrap = true;
					_Contents( &bWrap, pShape, true);

					pTextView->SetWrapping( bWrap);
				}
			}
		}
	}
}


// ParsePopupMenu
void MintLoader::ParsePopupMenu(MXmlElement* pElement, MWidget* pWidget)
{
	ParseWidget( pElement, pWidget);
	pWidget->Show( false);

	MPopupMenu* pPopupMenu = (MPopupMenu*)pWidget;
	for ( MXmlElement* child = pElement->FirstChildElement();  child != NULL;  child = child->NextSiblingElement())
	{
		if ( IsSameTag( child, "Look"))
		{
			string strName;
			_Contents( strName, child, "", m_pXml);
			MLookBase* pLook = m_pMint->m_pLookManager->Get( strName);
			if ( pLook)		pPopupMenu->SetLookAndFill( pLook);
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


// ParseTreeCtrl
void MintLoader::ParseTreeCtrl( MXmlElement* pElement, MWidget* pWidget)
{
	ParseWidget( pElement, pWidget);

	MTreeCtrl* pTreeCtrl = (MTreeCtrl*)pWidget;
	for ( MXmlElement* child = pElement->FirstChildElement();  child != NULL;  child = child->NextSiblingElement())
	{
		if ( IsSameTag( child, "Look"))
		{
			string strType, strName;
			_Attribute( strType, child, "type", "");
			_Contents( strName, child, "", m_pXml);

			if ( strType.empty())
			{
				MLookBase* pLook = m_pMint->m_pLookManager->Get( strName);
				if ( pLook)		pTreeCtrl->SetLookAndFill( pLook);
			}

			else if ( strType == string( "scrollbar"))
			{
				MLookBase* pLook = m_pMint->m_pLookManager->Get( strName);
				if ( pLook)		pTreeCtrl->GetScrollBar()->SetLookAndFill( (MScrollBarLook*)pLook);
			}
		}

		else if ( IsSameTag( child, "Shape"))
		{
			for ( MXmlElement* pShape = child->FirstChildElement();  pShape != NULL;  pShape = pShape->NextSiblingElement())
			{
				if ( IsSameTag( pShape, "ShowRootItem"))
				{
					bool bShow = false;
					_Contents( &bShow, pShape, true);
					pTreeCtrl->ShowRootItem( bShow);
				}

				else if ( IsSameTag( pShape, "MultipleSel"))
				{
					bool bMultiSel = false;
					_Contents( &bMultiSel, pShape, true);
					pTreeCtrl->EnableMultipleSel( bMultiSel);
				}
			}
		}
	}
}


// ParseCategoryCtrl
void MintLoader::ParseCategoryCtrl( MXmlElement* pElement, MWidget* pWidget)
{
	ParseWidget( pElement, pWidget);

	MCategoryCtrl* pCategory = (MCategoryCtrl*)pWidget;
	for ( MXmlElement* child = pElement->FirstChildElement();  child != NULL;  child = child->NextSiblingElement())
	{
		if ( IsSameTag( child, "Look"))
		{
			string strType, strName;
			_Attribute( strType, child, "type", "");
			_Contents( strName, child, "", m_pXml);

			if ( strType.empty())
			{
				MLookBase* pLook = m_pMint->m_pLookManager->Get( strName);
				if ( pLook)		pCategory->SetLookAndFill( pLook);
			}

			else if ( strType == string( "scrollbar"))
			{
				MLookBase* pLook = m_pMint->m_pLookManager->Get( strName);
				if ( pLook)		pCategory->GetScrollBar()->SetLookAndFill( (MScrollBarLook*)pLook);
			}
		}

		else if ( IsSameTag( child, "Shape"))
		{
			for ( MXmlElement* pShape = child->FirstChildElement();  pShape != NULL;  pShape = pShape->NextSiblingElement())
			{
				if ( IsSameTag( pShape, "AddColumn"))
				{
					string strAlign;
					_Attribute( strAlign, pShape, "align", "left,vcenter");
					transform( strAlign.begin(), strAlign.end(), strAlign.begin(), tolower);
					int _align = 0;
					if ( strAlign.find( "left") != string::npos)			_align |= MAM_LEFT;
					if ( strAlign.find( "top") != string::npos)				_align |= MAM_TOP;
					if ( strAlign.find( "right") != string::npos)			_align |= MAM_RIGHT;
					if ( strAlign.find( "bottom") != string::npos)			_align |= MAM_BOTTOM;
					if ( strAlign.find( "hcenter") != string::npos)			_align |= MAM_HCENTER;
					if ( strAlign.find( "vcenter") != string::npos)			_align |= MAM_VCENTER;
					if ( strAlign.find( "justify") != string::npos)			_align |= MAM_JUSTIFY;

					int width;
					_Attribute( &width, pShape, "width", 10);

					string strColumn;
					_Contents( strColumn, pShape, "", m_pXml);
					pCategory->AddColumn( strColumn.c_str(), _align, width);
				}

				else if ( IsSameTag( pShape, "MultipleSel"))
				{
					bool bMultiSel;
					_Contents( &bMultiSel, pShape, false);
					pCategory->EnableMultipleSel( bMultiSel);
				}
			}
		}
	}
}


bool MintLoader::CheckExistIncludedFile( string strFileName )
{
	std::transform(strFileName.begin(), strFileName.end(), strFileName.begin(), std::tolower);

	if (m_IncludedFileNames.find(strFileName) != m_IncludedFileNames.end()) return true;
	m_IncludedFileNames.insert(strFileName);

	return false;
}

void MintLoader::ClearIncludedFileNames()
{
	m_IncludedFileNames.clear();
}

void MintLoader::ParseSheet( MXmlElement* pElement, MWidget* pWidget )
{
	if (pWidget == NULL) return;

	MSheet* pSheet = dynamic_cast<MSheet*>(pWidget);
	if (pSheet == NULL) return;

	MXmlElement* child;
	for( child = pElement->FirstChildElement(); child != NULL; child = child->NextSiblingElement() )
	{
		if (IsSameTag(child, MINT_XML_TAG_SCRIPT))
		{
			ParseScript(child);
		}
		else if (IsSameTag(child, MINT_XML_TAG_EVENT))
		{
			ParseEvent(child, pWidget);
		}
	}

}

MFont* MintLoader::CreateNewFont( const char* szName, const char* szFontName, const char* szFileName, int nHeight, bool bBold, bool bItalic, bool bAntiAlias, float fOutline, float fScale)
{
	return MGetMint()->NewFont( szName, szFontName, szFileName, nHeight, bBold, bItalic, bAntiAlias, fOutline, fScale);
}


} // namespace mint3