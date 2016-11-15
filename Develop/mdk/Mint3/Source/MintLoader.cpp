#include "stdafx.h"
#include <cctype>
#include "MintLoader.h"
#include "MFileSystem.h"
#include "MFrame.h"
#include "MLabel.h"
#include "MButton.h"
#include "MEdit.h"
#include "MListBox.h"
#include "MComboBox.h"
#include "MScrollBar.h"
#include "MSpinCtrl.h"
#include "MSlider.h"
#include "MTextArea.h"
#include "MPicture.h"
#include "MAnimation.h"
#include "MPanel.h"
#include "MPaint.h"
#include "MMTimer.h"
#include "MSheet.h"
#include "MTabCtrl.h"
#include "MView.h"
#include "MLayer.h"
#include "MTextView.h"
#include "MTreeView.h"
#include "MXml.h"
#include "Mint.h"
#include "MWLua.h"
#include "MScriptManager.h"
#include "MMainFrame.h"

// madduck [4/4/2006]
#include "MLookManager.h"
#include "MBFrameLook.h"
#include "MBButtonLook.h"
#include "MBEditLook.h"
#include "MBListBoxLook.h"
#include "MBScrollBarLook.h"
#include "MBSpinCtrlLook.h"
#include "MBTextAreaLook.h"
#include "MBGroupLook.h"
#include "MBTreeViewLook.h"
#include "MBPopupBoxLook.h"


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
#define MINT_XML_TAG_TEXTAREA		"TextArea"
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
#define MINT_XML_TAG_TREEVIEW		"TreeView"

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
#define MINT_XML_TAG_TEXTAREALOOK	"TextAreaLook"
#define MINT_XML_TAG_GROUPLOOK		"GroupLook"
#define MINT_XML_TAG_TREEVIEWLOOK	"TreeViewLook"
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
#define MINT_XML_TAG_FADEROLLOVER	"FadeRollover"			// Button look only

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
#define MINT_XML_TAG_DROPABLE		"ItemDragDrop"	
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
#define MINT_XML_TAG_INDICATOR		"Indicator"
#define MINT_XML_TAG_PASSWORD		"Password"
#define MINT_XML_TAG_NUMBER			"Number"
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

void MintLoader::ParseWidgets(MXmlElement* pElement, MWidget* pParent, const char* szWidgetName)
{
	MWidget* pNewWidget = NULL;

	if (IsSameTag(pElement, MINT_XML_TAG_FRAME))
	{
		pNewWidget = NewWidget(MINT_FRAME, szWidgetName, pParent);
		ParseFrame(pElement, pNewWidget);
	}
	else if (IsSameTag(pElement, MINT_XML_TAG_SHEET))
	{
		pNewWidget = FindSheet(string(szWidgetName));
		ParseSheet(pElement, pNewWidget);
	}
	else if (IsSameTag(pElement, MINT_XML_TAG_LABEL))
	{
		pNewWidget = NewWidget(MINT_LABEL, szWidgetName, pParent);
		ParseLabel(pElement, pNewWidget);
	}
	else if (IsSameTag(pElement, MINT_XML_TAG_BUTTON))
	{
		pNewWidget = NewWidget(MINT_BUTTON, szWidgetName, pParent);
		ParseButton(pElement, pNewWidget);
	}
	else if (IsSameTag(pElement, MINT_XML_TAG_EDIT))
	{
		pNewWidget = NewWidget(MINT_EDIT, szWidgetName, pParent);
		ParseEdit(pElement, pNewWidget);
	}
	else if (IsSameTag(pElement, MINT_XML_TAG_LISTBOX))
	{
		pNewWidget = NewWidget(MINT_LISTBOX, szWidgetName, pParent);
		ParseListBox(pElement, pNewWidget);
	}
	else if(IsSameTag(pElement, MINT_XML_TAG_COMBOBOX))
	{
		pNewWidget = NewWidget(MINT_COMBOBOX, szWidgetName, pParent);
		ParseComboBox(pElement, pNewWidget);
	}
	else if(IsSameTag(pElement, MINT_XML_TAG_SCROLLBAR))
	{
		pNewWidget = NewWidget(MINT_SCROLLBAR, szWidgetName, pParent);
		ParseScrollBar(pElement, pNewWidget);
	}
	else if(IsSameTag(pElement, MINT_XML_TAG_SPINCTRL))
	{
		pNewWidget = NewWidget(MINT_SPINCTRL, szWidgetName, pParent);
		ParseSpinCtrl(pElement, pNewWidget);
	}
	else if(IsSameTag(pElement, MINT_XML_TAG_SLIDER))
	{
		pNewWidget = NewWidget(MINT_SLIDER, szWidgetName, pParent);
		ParseScrollBar(pElement, pNewWidget);
	}
	else if (IsSameTag(pElement, MINT_XML_TAG_TEXTAREA))
	{
		pNewWidget = NewWidget(MINT_TEXTAREA, szWidgetName, pParent);
		ParseTextArea(pElement, pNewWidget);
	}
	else if (IsSameTag(pElement, MINT_XML_TAG_GROUP))
	{
		pNewWidget = NewWidget(MINT_GROUP, szWidgetName, pParent);
		ParseGroup(pElement, pNewWidget);
	}
	else if (IsSameTag(pElement, MINT_XML_TAG_PICTURE))
	{
		pNewWidget = NewWidget(MINT_PICTURE, szWidgetName, pParent);
		ParsePicture(pElement, pNewWidget);
	}
	else if (IsSameTag(pElement, MINT_XML_TAG_ANIMATION))
	{
		pNewWidget = NewWidget(MINT_ANIMATION, szWidgetName, pParent);
		ParseAnimation(pElement, pNewWidget);
	}
	else if (IsSameTag(pElement, MINT_XML_TAG_TIMER))
	{
		pNewWidget = NewWidget(MINT_TIMER, szWidgetName, pParent);
		ParseTimer(pElement, pNewWidget);
	}
	else if (IsSameTag(pElement, MINT_XML_TAG_PANEL))
	{
		pNewWidget = NewWidget(MINT_PANEL, szWidgetName, pParent);
		ParsePanel(pElement, pNewWidget);
	}
	else if (IsSameTag(pElement, MINT_XML_TAG_PAINT))
	{
		pNewWidget = NewWidget(MINT_PAINT, szWidgetName, pParent);
		ParsePaint(pElement, pNewWidget);
	}
	else if(IsSameTag(pElement, MINT_XML_TAG_TABCTRL))
	{
		pNewWidget = NewWidget(MINT_TABCTRL, szWidgetName, pParent);
		ParseTabCtrl(pElement, pNewWidget);
	}
	else if(IsSameTag(pElement, MINT_XML_TAG_VIEW))
	{
		pNewWidget = NewWidget(MINT_VIEW, szWidgetName, pParent);
		ParseView(pElement, pNewWidget);
	}
	else if(IsSameTag(pElement, MINT_XML_TAG_LAYER))
	{
		pNewWidget = NewWidget(MINT_LAYER, szWidgetName, pParent);
		ParseLayer(pElement, pNewWidget);
	}
	else if(IsSameTag(pElement, MINT_XML_TAG_TEXTVIEW))
	{
		pNewWidget = NewWidget(MINT_TEXTVIEW, szWidgetName, pParent);
		ParseTextView(pElement, pNewWidget);
	}
	else if(IsSameTag(pElement, MINT_XML_TAG_TREEVIEW))
	{
		pNewWidget = NewWidget(MINT_TREEVIEW, szWidgetName, pParent);
		ParseTreeView(pElement, pNewWidget);
	}
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
	if (m_pMint->GetMainFrame() == NULL) return NULL;

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
		else
		{
			char szName[256] = "";
			MWidget* pParentWidget = pSheetWidget ? pSheetWidget : m_pMint->GetMainFrame();
			MWidget* pRetParent = ParseWidgetName(szName, child, pParentWidget);
			if ( pRetParent != NULL)
				pParentWidget = pRetParent;

			ParseWidgets(child, pParentWidget, szName);
		}
	}

	// 일단 모두 안보이도록 함
	if(pSheetWidget)
	{
		if (pSheetWidget->GetVisible()) pSheetWidget->Show(false);
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
	char szContents[256];
	_Contents(szContents, pElement, "", m_pXml);

	string strInclFileName = m_strBasePath + string(szContents);

	if (CheckExistIncludedFile(strInclFileName) == true) return;

	Load(strInclFileName.c_str(), true);
}

MWidget* MintLoader::NewWidget(const char* szClass, const char* szName, MWidget* pParent)
{
	MWidget* pNew = m_pMint->NewWidget(szClass, szName, pParent, NULL);
	if (strlen(szName) > 0)
		pNew->DeclToSctipt();	// 루아 오브젝트 생성

	//m_pMint->GetScriptManager()->DeclWidget(szClass, szName, pNew);	
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
	string strAttr, strScript;

	_Attribute(strAttr, pElement, MINT_XML_ATTR_FILE, "", m_pXml);

	if (!strAttr.empty())
	{
		// 연결된 루아 스크립트 실행
		string strScriptFile = m_strBasePath + strAttr;

		if (CheckExistIncludedFile(strScriptFile) == false)
		{
			m_pMint->GetScriptManager()->RunScriptFile(strScriptFile.c_str());
		}
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

			pWidget->LinkScriptEvent(strEventName, strScript);
		}			
	}
}

void MintLoader::ParseWidget(MXmlElement* pElement, MWidget* pWidget)
{
	MXmlElement* child;
	for( child = pElement->FirstChildElement(); child != NULL; child = child->NextSiblingElement() )
	{
		if (IsSameTag(child, MINT_XML_TAG_LAYOUT))
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

void MintLoader::ParseButton(MXmlElement* pElement, MWidget* pWidget)
{
	ParseWidget(pElement, pWidget);

	MButton* pButton = (MButton*)pWidget;

	// madduck [4/4/2006]
	MXmlElement* child;
	for( child = pElement->FirstChildElement(); child != NULL; child = child->NextSiblingElement() )
	{
		if (IsSameTag(child, MINT_XML_TAG_LOOK))
		{
			string strSkinName;
			if (_Contents(strSkinName, child, "", m_pXml))
			{
				map<string, MButtonLook*>::iterator itor = m_pMint->m_pLookManager->m_ButtonLookMap.find(strSkinName.c_str());
				if (itor != m_pMint->m_pLookManager->m_ButtonLookMap.end())
				{
					if((*itor).second->GetUILookType() == MWIDGET_MBBUTTONLOOK)
					{
						((MBButtonLook*)(*itor).second)->SetCustomLook(true);
					}
					pButton->ChangeCustomLook((*itor).second);
				}
			}
		}
		// Push 기능 입력
		else if(IsSameTag(child, MINT_XML_TAG_SHAPE))
		{
			MXmlElement* pShapeElement;
			for( pShapeElement = child->FirstChildElement(); pShapeElement != NULL; pShapeElement=pShapeElement->NextSiblingElement() )
			{
				if(IsSameTag(pShapeElement, MINT_XML_TAG_STYLE))
				{
					string strType;
					_Attribute(strType, pShapeElement, MINT_XML_ATTR_TYPE, "button");
					if(strType == string("checkbutton"))
					{
						pButton->SetType(MBT_CHECKBTN);
					}
					else if(strType == string("pushbutton"))
					{
						pButton->SetType(MBT_PUSHBTN);
					}
				}
				else if(IsSameTag(pShapeElement, MINT_XML_TAG_GROUP))
				{
					string strGroupName;
					_Attribute(strGroupName, pShapeElement, MINT_XML_ATTR_NAME);

					if (!strGroupName.empty())
					{
						MButtonGroupManager* pBGM = m_pMint->m_pButtonGroupManager;
						MButtonGroup* pButtonGroup = pBGM->Get(strGroupName);
						if (pButtonGroup == NULL)
						{
							pButtonGroup = new MButtonGroup();
							pBGM->Add(strGroupName, pButtonGroup);
						}

						pButton->SetButtonGroup(pButtonGroup);
					}
				}
				else if(IsSameTag(pShapeElement, MINT_XML_TAG_CHECKED))
				{
					pButton->SetCheck(true);
				}
				else if(IsSameTag(pShapeElement, "Confirm"))
				{
					string strConfirm;
					_Contents( strConfirm, pShapeElement, "", m_pXml);

					pButton->SetConfirmMessageBox( strConfirm.c_str());
				}

			}
		}
		else if (IsSameTag(child, MINT_XML_TAG_CONTROLS))
		{
			MXmlElement* pWidgetElement;
			for( pWidgetElement = child->FirstChildElement(); pWidgetElement != NULL; pWidgetElement = pWidgetElement->NextSiblingElement() )
			{
				char szWidgetName[256] = "";
				ParseWidgetName(szWidgetName, pWidgetElement, pWidget);

				// 무한 재귀호출이 될 수 있음. 조심.
				ParseWidgets(pWidgetElement, pWidget, szWidgetName);
			}
		}
	}
}

void MintLoader::ParseFrame(MXmlElement* pElement, MWidget* pWidget)
{
	ParseWidget(pElement, pWidget);

	MFrame* pFrame = (MFrame*)pWidget;

	MXmlElement* child;
	for( child = pElement->FirstChildElement(); child != NULL; child = child->NextSiblingElement() )
	{
		if (IsSameTag(child, MINT_XML_TAG_CONTROLS))
		{
			MXmlElement* pWidgetElement;
			for( pWidgetElement = child->FirstChildElement(); pWidgetElement != NULL; pWidgetElement = pWidgetElement->NextSiblingElement() )
			{
				char szWidgetName[256] = "";
				ParseWidgetName(szWidgetName, pWidgetElement, pWidget);

				// 무한 재귀호출이 될 수 있음. 조심.
				ParseWidgets(pWidgetElement, pWidget, szWidgetName);
			}
		}
		else if (IsSameTag(child, MINT_XML_TAG_SHAPE))
		{
			MXmlElement* pShapeElement;
			for( pShapeElement = child->FirstChildElement(); pShapeElement != NULL; pShapeElement=pShapeElement->NextSiblingElement() )
			{
				if (IsSameTag(pShapeElement, MINT_XML_TAG_TITLEBAR))
				{
					bool bVisible = true;
					_Contents(&bVisible, pShapeElement, true);

					if (!bVisible)
					{
						pFrame->m_bTitleBar = bVisible;
					}
				}

				else if (IsSameTag(pShapeElement, MINT_XML_TAG_MOVEABLE))
				{
					bool bMoveable = true;
					_Contents(&bMoveable, pShapeElement, true);

					pFrame->SetMovable( bMoveable);
				}
			}
		}
		// madduck [4/5/2006]
		else if (IsSameTag(child, MINT_XML_TAG_LOOK))
		{
			string strSkinName;
			if (_Contents(strSkinName, child, "", m_pXml))
			{
				MFrame* pFrame = (MFrame*)pWidget;
				map<string, MBFrameLook*>::iterator itor = m_pMint->m_pLookManager->m_FrameLookMap.find(strSkinName.c_str());
				if (itor != m_pMint->m_pLookManager->m_FrameLookMap.end())
				{
					((MBFrameLook*)(*itor).second)->SetCustomLook(0);
					pFrame->ChangeCustomLook((MBFrameLook*)(*itor).second);					
				}
			}
		}
	}
}

void MintLoader::ParseLayout(MXmlElement* pElement, MWidget* pWidget)
{
	int x=0,y=0,w=0,h=0;
	MAnchorStyle size_base;		// size 기준점
	string strLocBase;			// 나중에 지울꺼...
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
				if ( pParent && (strW.at( 0) == '%'))	w = (int)((float)pParent->m_Rect.w * (float)atof(strW.c_str() + 1) / 100.0f);
				else									w = atoi( strW.c_str());
				if ( pParent && (strH.at( 0) == '%'))	h = (int)((float)pParent->m_Rect.h * (float)atof(strH.c_str() + 1) / 100.0f);
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
				if ( pParent && (strX.at( 0) == '%'))	x = (int)((float)pParent->m_Rect.w * (float)atof(strX.c_str() + 1) / 100.0f);
				else									x = atoi( strX.c_str());
				if ( pParent && (strY.at( 0) == '%'))	y = (int)((float)pParent->m_Rect.h * (float)atof(strY.c_str() + 1) / 100.0f);
				else									y = atoi( strY.c_str());
			}

			// base 속성
			_Attribute(strLocBase, child, "base", "TOPLEFT");
			transform(strLocBase.begin(), strLocBase.end(), strLocBase.begin(), tolower);	// 소문자로 변환

			// base 속성
			_Attribute(strBase, child, "_base", "TOPLEFT");
			transform(strBase.begin(), strBase.end(), strBase.begin(), tolower);
		}
		else if (IsSameTag(child, MINT_XML_TAG_ANCHOR))
		{
			MAnchorStyle anchor;
			_Attribute(&anchor.bLeft,	child, MINT_XML_ATTR_LEFT,	true);
			_Attribute(&anchor.bRight,	child, MINT_XML_ATTR_RIGHT, false);
			_Attribute(&anchor.bTop,	child, MINT_XML_ATTR_TOP,	true);
			_Attribute(&anchor.bBottom, child, MINT_XML_ATTR_BOTTOM,false);

			pWidget->SetAnchor(anchor);
		}
	}



	///////////////////////////////////////////////////////// 나중에 지워버릴꺼...
	// loc base 속성에 따른 위치값 세팅
	if (strLocBase == string("bottomright"))
	{
		if (pParent)
		{
			x = (pParent->m_Rect.w-1) - x - w;
			y = (pParent->m_Rect.h-1) - y - h;
		}
		else { x=0;y=0; }
	}
	else if (strLocBase == string("bottomleft"))
	{
		if (pParent)
		{
			y = (pParent->m_Rect.h-1) - y - h;
		}
		else { y=0; }
	}
	else if (strLocBase == string("topright"))
	{
		if (pParent)
		{
			x = (pParent->m_Rect.w-1) - x - w;
		}
		else { y=0; }
	}
	/////////////////////////////////////////////////////////



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
	bool visible = true;
	bool bResize = false;
	bool bClipByParent = false;
	string font, text, tooltip, helpmsg, textAlignment;
	float opacity = 1.0f;
	int r = 255,g = 255, b = 255;
	MAlignmentMode am = 0;

	MXmlElement* child;
	for( child = pElement->FirstChildElement(); child != NULL; child = child->NextSiblingElement() )
	{
		if (IsSameTag(child, MINT_XML_TAG_VISIBLE))
		{
			_Contents(&visible, child, true);
		}
		else if (IsSameTag(child, MINT_XML_TAG_TEXT))
		{
			_Contents(text, child, "", m_pXml);
		}
		else if (IsSameTag(child, MINT_XML_TAG_TOOLTIP))
		{
			_Contents(tooltip, child, "", m_pXml);
		}
		else if (IsSameTag(child, MINT_XML_TAG_HELPMSG))
		{
			_Contents(helpmsg, child, "", m_pXml);
		}
		else if (IsSameTag(child, MINT_XML_TAG_CURSOR))
		{

		}
		else if (IsSameTag(child, MINT_XML_TAG_FONT))
		{
			_Contents(font, child, "", m_pXml);
		}
		else if (IsSameTag(child, MINT_XML_TAG_OPACITY))
		{
			_Contents(&opacity, child, 1.0f);
		}
		else if (IsSameTag(child, MINT_XML_TAG_CLIPBYPARENT))
		{
			_Contents(&bClipByParent, child, false);
		}
		else if(IsSameTag(child, MINT_XML_TAG_FONTCOLOR))
		{
			_Attribute(&r,	child, MINT_XML_ATTR_R);
			_Attribute(&g,	child, MINT_XML_ATTR_G);
			_Attribute(&b,	child, MINT_XML_ATTR_B);
		}
		else if(IsSameTag(child, MINT_XML_TAG_RESIZE))
		{
			_Contents(&bResize, child, false);
		}
		else if(IsSameTag(child, MINT_XML_TAG_ALIGNMENT))
		{
			_Contents(textAlignment, child, "", m_pXml);
			transform(textAlignment.begin(), textAlignment.end(), textAlignment.begin(), tolower);	// 소문자로 변환

			// 텍스트 정렬
			bool bSelect = false;
			_Attribute(&bSelect, child, MINT_XML_ATTR_ALIGNMENT_LEFT,	false);
			if(bSelect) am |= MAM_LEFT;
			_Attribute(&bSelect, child, MINT_XML_ATTR_ALIGNMENT_RIGHT,	false);
			if(bSelect) am |= MAM_RIGHT;
			_Attribute(&bSelect, child, MINT_XML_ATTR_ALIGNMENT_TOP,	false);
			if(bSelect) am |= MAM_TOP;
			_Attribute(&bSelect, child, MINT_XML_ATTR_ALIGNMENT_BOTTOM,	false);
			if(bSelect) am |= MAM_BOTTOM;
			_Attribute(&bSelect, child, MINT_XML_ATTR_ALIGNMENT_HCENTER,false);
			if(bSelect) am |= MAM_HCENTER;
			_Attribute(&bSelect, child, MINT_XML_ATTR_ALIGNMENT_VCENTER,false);
			if(bSelect) am |= MAM_VCENTER;

			// 정렬에 대한 값이 없으면 셋팅하지 않는다.
			if(am > 0)
			{
				pWidget->SetAlignment(am);
			}
		}
	}

	// 폰트 설정
	MFont* pFont = MFontManager::Get( font.c_str());
	pWidget->SetFont( pFont);

	// TODO: enabled은 아직 Widget에서 지원안함
	pWidget->SetText(text.c_str());

	// TODO: 툴팁 오브젝트 자체를 붙이는 건 나중에 지원함.
//	if (!tooltip.empty()) pWidget->AttachToolTip(tooltip.c_str());

	if (!tooltip.empty()) pWidget->SetTooltipMsg( tooltip.c_str());
	if (!helpmsg.empty()) pWidget->SetHelpMsg( helpmsg.c_str());

	if (opacity != 1.0f)
	{
		pWidget->SetOpacity(opacity);
	}

	pWidget->Show(visible);
	pWidget->SetTextColor(MCOLOR(r, g, b));
	pWidget->SetResizable(bResize);
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

void MintLoader::ParseTextArea(MXmlElement* pElement, MWidget* pWidget)
{
	ParseWidget(pElement, pWidget);

	MTextArea* pTextArea = (MTextArea*)pWidget;

	MXmlElement* child;
	for( child = pElement->FirstChildElement(); child != NULL; child = child->NextSiblingElement() )
	{
		if (IsSameTag(child, MINT_XML_TAG_LOOK))
		{
			string strType;
			_Attribute( strType, child, MINT_XML_ATTR_TYPE, "");

			string strSkinName;
			if (_Contents( strSkinName, child, "", m_pXml))
			{
				if ( strType == string( MINT_XML_TAG_TEXTAREA))
				{
					map<string, MBTextAreaLook*>::iterator itor = m_pMint->m_pLookManager->m_TextAreaLookMap.find(strSkinName.c_str());
					if (itor != m_pMint->m_pLookManager->m_TextAreaLookMap.end())
					{
						pTextArea->ChangeCustomLook((MBTextAreaLook*)(*itor).second);
					}
					else
					{
						if (!_stricmp(strSkinName.c_str(), MINT_XML_LOOK_VALUE_NULL))
						{
							pTextArea->ChangeLookToNull();
						}
					}
				}

				else if ( strType == string( MINT_XML_TAG_SCROLLBAR))
				{
					map<string, MBScrollBarLook*>::iterator itor = m_pMint->m_pLookManager->m_ScrollBarLookMap.find(strSkinName.c_str());
					if (itor != m_pMint->m_pLookManager->m_ScrollBarLookMap.end())
						pTextArea->SetScrollBarLook( (MBScrollBarLook*)(*itor).second);
				}
			}
		}

		else if (IsSameTag(child, MINT_XML_TAG_SHAPE))
		{
			MXmlElement* pShape;
			for( pShape = child->FirstChildElement(); pShape != NULL; pShape = pShape->NextSiblingElement() )
			{
				if (IsSameTag(pShape, MINT_XML_TAG_EDITABLE))
				{
					bool bEditable = true;
					_Contents(&bEditable, pShape, true);
					pTextArea->SetEditable(bEditable);
				}
				else if(IsSameTag(pShape, MINT_XML_TAG_SCROLLBARENABLE))
				{
					bool bEnable = true;
					_Contents(&bEnable, pShape, true);
					pTextArea->SetScrollBarEnable(bEnable);
				}

				else if(IsSameTag(pShape, MINT_XML_TAG_SCROLLBAR))
				{
					// 스크롤바 두께
					int width;
					_Attribute( &width, pShape, MINT_XML_ATTR_WIDTH, 15);
					pTextArea->SetScrollBarWidth( width);
				}
				else if (IsSameTag(pShape, MINT_XML_TAG_WORDWRAP))
				{
					bool bWordWrap = true;
					_Contents(&bWordWrap, pShape, true);
					pTextArea->SetWordWrap(bWordWrap);
				}
			}
		}
	}
}

void MintLoader::ParseGroup(MXmlElement* pElement, MWidget* pWidget)
{
	ParseWidget(pElement, pWidget);

	MGroup* pGroup = (MGroup*)pWidget;

	MXmlElement* child;
	for( child = pElement->FirstChildElement(); child != NULL; child = child->NextSiblingElement() )
	{
		if (IsSameTag(child, MINT_XML_TAG_LOOK))
		{
			string strSkinName;
			if (_Contents(strSkinName, child, "", m_pXml))
			{
				MGroup* pGroup = (MGroup*)pWidget;
				map<string, MBGroupLook*>::iterator itor = m_pMint->m_pLookManager->m_GroupLookMap.find(strSkinName.c_str());
				if (itor != m_pMint->m_pLookManager->m_GroupLookMap.end())
				{
					((MBGroupLook*)(*itor).second)->SetCustomLook(0);
					pGroup->ChangeCustomLook((MBGroupLook*)(*itor).second);					
				}
			}
		}
	}
}

void MintLoader::ParseTimer(MXmlElement* pElement, MWidget* pWidget)
{
	ParseWidget(pElement, pWidget);

	MXmlElement* child;
	for( child = pElement->FirstChildElement(); child != NULL; child = child->NextSiblingElement() )
	{
		// Action 태그
		if (IsSameTag(child, MINT_XML_TAG_ACTION))
		{
			MXmlElement* pChildAction;
			for( pChildAction = child->FirstChildElement(); pChildAction != NULL; pChildAction = pChildAction->NextSiblingElement() )
			{
				if (IsSameTag(pChildAction, MINT_XML_TAG_INTERVAL))
				{
					int interval = MINT_DEFAULT_TIMER_INTERVAL;
					_Contents(&interval, pChildAction, MINT_DEFAULT_TIMER_INTERVAL);

					MMTimer* pTimer = (MMTimer*)pWidget;
					pTimer->SetInterval(unsigned long(interval));
				}
			}
		}
	}
}

void MintLoader::ParseLabel(MXmlElement* pElement, MWidget* pWidget)
{
	ParseWidget(pElement, pWidget);
}

void MintLoader::ParseEdit(MXmlElement* pElement, MWidget* pWidget)
{
	ParseWidget(pElement, pWidget);

	MEdit* pEdit = (MEdit*)pWidget;

	MXmlElement* child;
	for( child = pElement->FirstChildElement(); child != NULL; child = child->NextSiblingElement() )
	{
		if (IsSameTag(child, MINT_XML_TAG_SHAPE))
		{
			MXmlElement* pShape;
			for( pShape = child->FirstChildElement(); pShape != NULL; pShape = pShape->NextSiblingElement() )
			{
				if (IsSameTag(pShape, MINT_XML_TAG_INDICATOR))
				{
					bool bVisible;
					_Contents(&bVisible, pShape, true);
					pEdit->SetVisibleIndicator(bVisible);
				}
				else if (IsSameTag(pShape, MINT_XML_TAG_PASSWORD))
				{
					bool bPassword;
					_Contents(&bPassword, pShape, true);
					pEdit->SetPasswordField(bPassword);
				}
				else if (IsSameTag(pShape, MINT_XML_TAG_NUMBER))
				{
					bool bNumber;
					_Contents(&bNumber, pShape, true);
					pEdit->SetNumber(bNumber);
				}
			}
		}

		else if (IsSameTag(child, MINT_XML_TAG_LOOK))
		{
			string strSkinName;
			if (_Contents(strSkinName, child, "", m_pXml))
			{
				map<string, MBEditLook*>::iterator itor = m_pMint->m_pLookManager->m_EditLookMap.find(strSkinName.c_str());
				if (itor != m_pMint->m_pLookManager->m_EditLookMap.end())
				{
					((MBEditLook*)(*itor).second)->SetCustomLook(0);
					pEdit->ChangeCustomLook((MBEditLook*)(*itor).second);					
				}
				else
				{
					if (!_stricmp(strSkinName.c_str(), MINT_XML_LOOK_VALUE_NULL))
					{
						pEdit->ChangeLookToNull();
					}
				}
			}
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

				else if ( IsSameTag( pShape, "Tiled"))
				{
					bool bTiled;
					if ( _Contents( &bTiled, pShape, false))
						pPicture->SetTiled( bTiled);
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

// Look Header
bool MintLoader::ParseLookHeader(MXmlElement* pElement, MLOOKBase& LookHeader)
{
	_Attribute(LookHeader.strName, pElement, MINT_XML_ATTR_NAME, "");
	if (LookHeader.strName.empty()) 
		return false;

	// 이미지를 고정/느리기
	_Attribute(&(LookHeader.bStretch), pElement, MINT_XML_ATTR_STRETCH, true);

	// 파일
	_Attribute(LookHeader.strFile, pElement, MINT_XML_ATTR_FILE, "");

	return true;
}

// madduck [4/3/2006]
//스킨을 콘트롤의 종류에 따라서 구분해주어야 되는디.
void MintLoader::ParseLook(MXmlElement* pElement)
{
	if (IsSameTag(pElement, MINT_XML_TAG_FRAMELOOK))
		ParseFrameLook(pElement);

	else if (IsSameTag(pElement, MINT_XML_TAG_BUTTONLOOK))
		ParseButtonLook(pElement);

	else if (IsSameTag(pElement, MINT_XML_TAG_EDITLOOK))
		ParseEditLook(pElement);

	else if (IsSameTag(pElement, MINT_XML_TAG_LISTBOXLOOK))
		ParseListBoxLook(pElement);

	else if (IsSameTag(pElement, MINT_XML_TAG_SCROLLBARLOOK))
		ParseScrollBarLook(pElement);

	else if (IsSameTag(pElement, MINT_XML_TAG_SPINCTRLLOOK))
		ParseSpinCtrlLook(pElement);

	else if (IsSameTag(pElement, MINT_XML_TAG_TEXTAREALOOK))
		ParseTextAreaLook(pElement);

	else if (IsSameTag(pElement, MINT_XML_TAG_GROUPLOOK))
		ParseGroupLook(pElement);

	else if (IsSameTag(pElement, MINT_XML_TAG_TREEVIEWLOOK))
		ParseTreeViewLook(pElement);

	else if (IsSameTag(pElement, MINT_XML_TAG_TOOLTIPLOOK))
		ParseTooltipLook(pElement);
}

// madduck [4/4/2006]
void MintLoader::ParseFrameLook(MXmlElement* pElement)
{
	MLOOKBase LookHeader;
	if(ParseLookHeader(pElement, LookHeader) == false)
		return;

	MBFrameLook* pFrameLook = new MBFrameLook();
	if(!m_pMint->m_pLookManager->m_FrameLookMap.insert(map<string, MBFrameLook*>::value_type(string(LookHeader.strName.c_str()), (MBFrameLook*)pFrameLook)).second)
		OutputDebugString("insert MBFrameLook failed.\n");

	pFrameLook->SetStretchBlt( LookHeader.bStretch);

	string strImageName;
	MXmlElement* child;
	for( child = pElement->FirstChildElement(); child != NULL; child = child->NextSiblingElement() )
	{
		if (IsSameTag(child, MINT_XML_TAG_IMAGE))
		{
			if (_Contents(strImageName, child, "", m_pXml))
			{
				pFrameLook->SetBitmap( strImageName);
			}
		}

		// 타일 형식의 비트맵을 사용하는지 여부
		else if ( IsSameTag( child, MINT_XML_TAG_TILED))
		{
			bool bTiled = false;
			_Contents( &bTiled, child, false);

			pFrameLook->SetTiled( bTiled);
		}

		else if (IsSameTag(child, MINT_XML_TAG_FONT))
		{
			string strFontName;
			if ( _Contents( strFontName, child, "", m_pXml))
			{
				MFont* pFont = MFontManager::Get( strFontName.c_str());
				pFrameLook->SetFont( pFont);
			}
		}

		else if (IsSameTag(child, MINT_XML_TAG_TITLEOFFSET))
		{
			int x = 0, y = 0;

			_Attribute( &x, child, MINT_XML_ATTR_X);
			_Attribute( &y, child, MINT_XML_ATTR_Y);

			pFrameLook->SetTitleOffset( x, y);
		}

		else if (IsSameTag(child, MINT_XML_TAG_TITLESHADOW))
		{
			bool bShadow = false;
			_Contents( &bShadow, child, false);

			pFrameLook->SetTitleShadow( bShadow);
		}

		else if (IsSameTag(child, MINT_XML_TAG_WINRECTOFFSET))
		{
			int x = 0, y = 0, w = 0, h = 0;

			_Attribute( &x, child, MINT_XML_ATTR_X);
			_Attribute( &y, child, MINT_XML_ATTR_Y);
			_Attribute( &w, child, MINT_XML_ATTR_W);
			_Attribute( &h, child, MINT_XML_ATTR_H);

			pFrameLook->SetWinRectOffset( x, y, w, h);
		}
		
		else if (IsSameTag(child, MINT_XML_TAG_TITLEBARHEIGHT))
		{
			int h = MTITLEBAR_HEIGHT;
			_Contents( &h, child, MTITLEBAR_HEIGHT);

			pFrameLook->SetTitlebarHeight( h);
		}

		// 에디트박스 상태에 따른 비트맵/폰트 색상 등을 입력받음
		else if ( IsSameTag( child, MINT_XML_ATTR_STAT_NORMAL))
			ParseFrameLookState( child, pFrameLook, 0);

		else if ( IsSameTag( child, MINT_XML_ATTR_STAT_OVER))
			ParseFrameLookState( child, pFrameLook, 1);

		else if ( IsSameTag( child, MINT_XML_ATTR_STAT_FOCUS))
			ParseFrameLookState( child, pFrameLook, 1);

		else if ( IsSameTag( child, MINT_XML_ATTR_STAT_PRESS))
			ParseFrameLookState( child, pFrameLook, 1);

		else if ( IsSameTag( child, MINT_XML_ATTR_STAT_DISABLE))
			ParseFrameLookState( child, pFrameLook, 2);
	}
}

void MintLoader::ParseFrameLookState(MXmlElement* pElement, MBFrameLook* pFrameLook, int stat)
{
	MXmlElement* child;
	for( child = pElement->FirstChildElement(); child != NULL; child = child->NextSiblingElement() )
	{
		if (IsSameTag(child, MINT_XML_TAG_ASSIGNMENT))
		{
			int   index	= 0;
			float left	= 0.0f;
			float right = 1.0f;
			float top	= 0.0f;
			float bottom= 1.0f;

			_Attribute(&index	, child, MINT_XML_ATTR_TYPE		, 0);
			_Attribute(&left	, child, MINT_XML_ATTR_LEFT		, 0.0f);
			_Attribute(&top		, child, MINT_XML_ATTR_TOP		, 0.0f);
			_Attribute(&right	, child, MINT_XML_ATTR_WIDTH	, 1.0f);
			_Attribute(&bottom	, child, MINT_XML_ATTR_HEIGHT	, 1.0f);

			pFrameLook->SetAssignRect(stat, index, left, top, right, bottom);
		}

		else if ( IsSameTag( child, MINT_XML_TAG_FONTCOLOR))
		{
			int r = 255, g = 255, b = 255;

			_Attribute( &r, child, MINT_XML_ATTR_R);
			_Attribute( &g, child, MINT_XML_ATTR_G);
			_Attribute( &b, child, MINT_XML_ATTR_B);

			pFrameLook->SetFontColor( stat, MCOLOR( r, g, b));
		}
	}
}


// madduck [4/4/2006]
void MintLoader::ParseButtonLook(MXmlElement* pElement)
{
	MLOOKBase LookHeader;
	if(ParseLookHeader(pElement, LookHeader) == false)
		return;

	MBButtonLook* pButtonLook = new MBButtonLook();
	//	if(!m_pMint->m_ButtonLookMap.insert(map<string, MBButtonLook*>::value_type(string(strName.c_str()), pButtonLook)).second)
	//		OutputDebugString("insert MBButtonLook failed.\n");
	if(!m_pMint->m_pLookManager->m_ButtonLookMap.insert(map<string, MButtonLook*>::value_type(string(LookHeader.strName.c_str()), (MButtonLook*)pButtonLook)).second)
		OutputDebugString("insert MBButtonLook failed.\n");

	pButtonLook->SetStretchBlt( LookHeader.bStretch);

	MXmlElement* child;
	for( child = pElement->FirstChildElement(); child != NULL; child = child->NextSiblingElement() )
	{
		if (IsSameTag(child, MINT_XML_TAG_IMAGE))
		{
			string strImageName;
			if (_Contents(strImageName, child, "", m_pXml))
			{
				pButtonLook->SetBitmap( strImageName);

				//흠..이름으로 검색하게 바꾸면 없앨부분.
				//				MBitmap* pBitmap = MBitmapManager::Get(strImageName.c_str());
				//				if (pBitmap)
				//				{
				//					pButtonLook->m_pMadBitmap = pBitmap;
				//				}
			}
		}

		// 폰트를 입력받음
		else if ( IsSameTag( child, MINT_XML_TAG_FONT))
		{
			string strFont;
			_Contents( strFont, child, "", m_pXml);

			MFont* pFont = MFontManager::Get( strFont.c_str());
			pButtonLook->SetFont( pFont);
		}

		// 페이드 롤오버 기능을 사용할지 여부를 입력받음
		else if ( IsSameTag( child, MINT_XML_TAG_FADEROLLOVER))
		{
			bool bFadeRollover = false;
			_Contents( &bFadeRollover, child, true);

			pButtonLook->SetFadeRollover( bFadeRollover);
		}

		// 타일 형식의 비트맵을 사용하는지 여부
		else if ( IsSameTag( child, MINT_XML_TAG_TILED))
		{
			bool bTiled = false;
			_Contents( &bTiled, child, false);

			pButtonLook->SetTiled( bTiled);
		}

		else if (IsSameTag(child, MINT_XML_TAG_TEXTOFFSET))
		{
			int x = 0, y = 0;

			_Attribute( &x, child, MINT_XML_ATTR_X);
			_Attribute( &y, child, MINT_XML_ATTR_Y);

			pButtonLook->SetTextOffset( x, y);
		}

		else if ( IsSameTag( child, MINT_XML_TAG_TEXTSHADOW))
		{
			bool bShadow = false;
			_Contents( &bShadow, child, false);

			pButtonLook->SetTextShadow( bShadow);
		}

		// 버튼 상태에 따른 비트맵/폰트 색상 등을 입력받음
		else if ( IsSameTag( child, MINT_XML_ATTR_STAT_NORMAL))
			ParseButtonLookState( child, pButtonLook, 0);

		else if ( IsSameTag( child, MINT_XML_ATTR_STAT_OVER))
			ParseButtonLookState( child, pButtonLook, 1);

		else if ( IsSameTag( child, MINT_XML_ATTR_STAT_PRESS))
			ParseButtonLookState( child, pButtonLook, 2);

		else if ( IsSameTag( child, MINT_XML_ATTR_STAT_DISABLE))
			ParseButtonLookState( child, pButtonLook, 3);
	}
}

// madduck [3/31/2006]
void MintLoader::ParseButtonLookState(MXmlElement* pElement, MBButtonLook* pButtonLook, int stat)
{
	MXmlElement* child;
	for( child = pElement->FirstChildElement(); child != NULL; child = child->NextSiblingElement() )
	{
		if (IsSameTag(child, MINT_XML_TAG_ASSIGNMENT))
		{
			int   index	= 0;
			float left	= 0.0f;
			float right = 1.0f;
			float top	= 0.0f;
			float bottom= 1.0f;

			_Attribute(&index	, child, MINT_XML_ATTR_TYPE		, 0);
			_Attribute(&left	, child, MINT_XML_ATTR_LEFT		, 0.0f);
			_Attribute(&top		, child, MINT_XML_ATTR_TOP		, 0.0f);
			_Attribute(&right	, child, MINT_XML_ATTR_WIDTH	, 1.0f);
			_Attribute(&bottom	, child, MINT_XML_ATTR_HEIGHT	, 1.0f);

			pButtonLook->SetAssignRect(stat, index, left, top, right, bottom);
		}

		else if ( IsSameTag( child, MINT_XML_TAG_FONTCOLOR))
		{
			int r = 255, g = 255, b = 255;

			_Attribute( &r, child, MINT_XML_ATTR_R);
			_Attribute( &g, child, MINT_XML_ATTR_G);
			_Attribute( &b, child, MINT_XML_ATTR_B);

			pButtonLook->SetFontColor( stat, MCOLOR( r, g, b));
		}
	}
}

void MintLoader::ParseEditLook(MXmlElement* pElement)
{
	MLOOKBase LookHeader;
	if(ParseLookHeader(pElement, LookHeader) == false)
		return;

	MBEditLook* pEditLook = new MBEditLook();
	if(!m_pMint->m_pLookManager->m_EditLookMap.insert(map<string, MBEditLook*>::value_type(string(LookHeader.strName.c_str()), (MBEditLook*)pEditLook)).second)
		OutputDebugString("insert MBEditLook failed.\n");

	pEditLook->SetStretchBlt( LookHeader.bStretch);

	MXmlElement* child;
	for( child = pElement->FirstChildElement(); child != NULL; child = child->NextSiblingElement() )
	{
		if (IsSameTag(child, MINT_XML_TAG_IMAGE))
		{
			string strImageName;
			if (_Contents(strImageName, child, "", m_pXml))
				pEditLook->SetBitmap( strImageName);
		}

		else if ( IsSameTag( child, MINT_XML_TAG_FONT))
		{
			string strFont;
			_Contents( strFont, child, "", m_pXml);

			MFont* pFont = MFontManager::Get( strFont.c_str());
			pEditLook->SetFont( pFont);
		}

		// 타일 형식의 비트맵을 사용하는지 여부
		else if ( IsSameTag( child, MINT_XML_TAG_TILED))
		{
			bool bTiled = false;
			_Contents( &bTiled, child, false);
			pEditLook->SetTiled( bTiled);
		}

		// 에디트박스 상태에 따른 비트맵/폰트 색상 등을 입력받음
		else if ( IsSameTag( child, MINT_XML_ATTR_STAT_NORMAL))
			ParseEditLookState( child, pEditLook, 0);

		else if ( IsSameTag( child, MINT_XML_ATTR_STAT_OVER))
			ParseEditLookState( child, pEditLook, 1);

		else if ( IsSameTag( child, MINT_XML_ATTR_STAT_FOCUS))
			ParseEditLookState( child, pEditLook, 1);

		else if ( IsSameTag( child, MINT_XML_ATTR_STAT_PRESS))
			ParseEditLookState( child, pEditLook, 1);

		else if ( IsSameTag( child, MINT_XML_ATTR_STAT_DISABLE))
			ParseEditLookState( child, pEditLook, 2);
	}
}

void MintLoader::ParseEditLookState(MXmlElement* pElement, MBEditLook* pEditLook, int stat)
{
	MXmlElement* child;
	for( child = pElement->FirstChildElement(); child != NULL; child = child->NextSiblingElement() )
	{
		if (IsSameTag(child, MINT_XML_TAG_ASSIGNMENT))
		{
			int   index	= 0;
			float left	= 0.0f;
			float right = 1.0f;
			float top	= 0.0f;
			float bottom= 1.0f;

			_Attribute(&index	, child, MINT_XML_ATTR_TYPE		, 0);
			_Attribute(&left	, child, MINT_XML_ATTR_LEFT		, 0.0f);
			_Attribute(&top		, child, MINT_XML_ATTR_TOP		, 0.0f);
			_Attribute(&right	, child, MINT_XML_ATTR_WIDTH	, 1.0f);
			_Attribute(&bottom	, child, MINT_XML_ATTR_HEIGHT	, 1.0f);

			pEditLook->SetAssignRect(stat, index, left, top, right, bottom);
		}

		else if ( IsSameTag( child, MINT_XML_TAG_FONTCOLOR))
		{
			int r = 255, g = 255, b = 255;

			_Attribute( &r, child, MINT_XML_ATTR_R);
			_Attribute( &g, child, MINT_XML_ATTR_G);
			_Attribute( &b, child, MINT_XML_ATTR_B);

			pEditLook->SetFontColor( stat, MCOLOR( r, g, b));
		}
	}
}


void MintLoader::ParseListBoxLook(MXmlElement* pElement)
{
	MLOOKBase LookHeader;
	if(ParseLookHeader(pElement, LookHeader) == false)
		return;

	MBListBoxLook* pListBoxLook = new MBListBoxLook();
	if(!m_pMint->m_pLookManager->m_ListBoxLookMap.insert(map<string, MBListBoxLook*>::value_type(string(LookHeader.strName.c_str()), (MBListBoxLook*)pListBoxLook)).second)
		OutputDebugString("insert MBListBoxLook failed.\n");

	pListBoxLook->SetStretchBlt( LookHeader.bStretch);

	MXmlElement* child;
	for( child = pElement->FirstChildElement(); child != NULL; child = child->NextSiblingElement() )
	{
		if (IsSameTag(child, MINT_XML_TAG_IMAGE))
		{
			string strImageName;
			if (_Contents(strImageName, child, "", m_pXml))
				pListBoxLook->SetBitmap( strImageName);
		}

		else if ( IsSameTag( child, MINT_XML_TAG_FONT))
		{
			string strFont;
			_Contents( strFont, child, "", m_pXml);

			MFont* pFont = MFontManager::Get( strFont.c_str());
			pListBoxLook->SetFont( pFont);
		}

		// 타일 형식의 비트맵을 사용하는지 여부
		else if ( IsSameTag( child, MINT_XML_TAG_TILED))
		{
			bool bTiled = false;
			_Contents( &bTiled, child, false);
			pListBoxLook->SetTiled( bTiled);
		}

		// 에디트박스 상태에 따른 비트맵/폰트 색상 등을 입력받음
		else if ( IsSameTag( child, MINT_XML_ATTR_STAT_NORMAL))
			ParseListBoxLookState( child, pListBoxLook, 0);

		else if ( IsSameTag( child, MINT_XML_ATTR_STAT_OVER))
			ParseListBoxLookState( child, pListBoxLook, 1);

		else if ( IsSameTag( child, MINT_XML_ATTR_STAT_FOCUS))
			ParseListBoxLookState( child, pListBoxLook, 1);

		else if ( IsSameTag( child, MINT_XML_ATTR_STAT_PRESS))
			ParseListBoxLookState( child, pListBoxLook, 1);

		else if ( IsSameTag( child, MINT_XML_ATTR_STAT_DISABLE))
			ParseListBoxLookState( child, pListBoxLook, 2);
	}
}

void MintLoader::ParseListBoxLookState(MXmlElement* pElement, MBListBoxLook* pListBoxLook, int stat)
{
	int type;
	_Attribute( type, pElement, MINT_XML_ATTR_TYPE, 0);


	MXmlElement* child;
	for( child = pElement->FirstChildElement(); child != NULL; child = child->NextSiblingElement() )
	{
		if (IsSameTag(child, MINT_XML_TAG_ASSIGNMENT))
		{
			int   index	= 0;
			float left	= 0.0f;
			float right = 1.0f;
			float top	= 0.0f;
			float bottom= 1.0f;

			_Attribute(&index	, child, MINT_XML_ATTR_TYPE		, 0);
			_Attribute(&left	, child, MINT_XML_ATTR_LEFT		, 0.0f);
			_Attribute(&top		, child, MINT_XML_ATTR_TOP		, 0.0f);
			_Attribute(&right	, child, MINT_XML_ATTR_WIDTH	, 1.0f);
			_Attribute(&bottom	, child, MINT_XML_ATTR_HEIGHT	, 1.0f);

			pListBoxLook->SetAssignRect( type, stat, index, left, top, right, bottom);
		}

		else if ( IsSameTag( child, MINT_XML_TAG_FONTCOLOR))
		{
			int r = 255, g = 255, b = 255;

			_Attribute( &r, child, MINT_XML_ATTR_R);
			_Attribute( &g, child, MINT_XML_ATTR_G);
			_Attribute( &b, child, MINT_XML_ATTR_B);

			pListBoxLook->SetFontColor( type, stat, MCOLOR( r, g, b));
		}
	}
}

void MintLoader::ParseScrollBarLook(MXmlElement* pElement)
{
	MLOOKBase LookHeader;
	if(ParseLookHeader(pElement, LookHeader) == false)
		return;

	MBScrollBarLook* pScrollBarLook = new MBScrollBarLook();
	if(!m_pMint->m_pLookManager->m_ScrollBarLookMap.insert(map<string, MBScrollBarLook*>::value_type(string(LookHeader.strName.c_str()), (MBScrollBarLook*)pScrollBarLook)).second)
		OutputDebugString("insert MBScrollBarLook failed.\n");

	pScrollBarLook->SetStretchBlt( LookHeader.bStretch);

	MXmlElement* child;
	for( child = pElement->FirstChildElement(); child != NULL; child = child->NextSiblingElement() )
	{
		if (IsSameTag(child, MINT_XML_TAG_IMAGE))
		{
			string strImageName;
			if (_Contents(strImageName, child, "", m_pXml))
				pScrollBarLook->SetBitmap( strImageName);
		}

		// 타일 형식의 비트맵을 사용하는지 여부
		else if ( IsSameTag( child, MINT_XML_TAG_TILED))
		{
			bool bTiled = false;
			_Contents( &bTiled, child, false);
			pScrollBarLook->SetTiled( bTiled);
		}

		// 에디트박스 상태에 따른 비트맵을 입력받음
		else if ( IsSameTag( child, MINT_XML_ATTR_STAT_NORMAL))
			ParseScrollBarLookState( child, pScrollBarLook, 0);

		else if ( IsSameTag( child, MINT_XML_ATTR_STAT_OVER))
			ParseScrollBarLookState( child, pScrollBarLook, 1);

		else if ( IsSameTag( child, MINT_XML_ATTR_STAT_FOCUS))
			ParseScrollBarLookState( child, pScrollBarLook, 1);

		else if ( IsSameTag( child, MINT_XML_ATTR_STAT_PRESS))
			ParseScrollBarLookState( child, pScrollBarLook, 1);

		else if ( IsSameTag( child, MINT_XML_ATTR_STAT_DISABLE))
			ParseScrollBarLookState( child, pScrollBarLook, 2);
	}
}

void MintLoader::ParseScrollBarLookState(MXmlElement* pElement, MBScrollBarLook* pScrollBarLook, int stat)
{
	int type;
	_Attribute( type, pElement, MINT_XML_ATTR_TYPE, 0);


	MXmlElement* child;
	for( child = pElement->FirstChildElement(); child != NULL; child = child->NextSiblingElement() )
	{
		if (IsSameTag(child, MINT_XML_TAG_ASSIGNMENT))
		{
			int   index	= 0;
			float left	= 0.0f;
			float right = 1.0f;
			float top	= 0.0f;
			float bottom= 1.0f;

			_Attribute(&index	, child, MINT_XML_ATTR_TYPE		, 0);
			_Attribute(&left	, child, MINT_XML_ATTR_LEFT		, 0.0f);
			_Attribute(&top		, child, MINT_XML_ATTR_TOP		, 0.0f);
			_Attribute(&right	, child, MINT_XML_ATTR_WIDTH	, 1.0f);
			_Attribute(&bottom	, child, MINT_XML_ATTR_HEIGHT	, 1.0f);

			pScrollBarLook->SetAssignRect( type, stat, index, left, top, right, bottom);
		}
	}
}

void MintLoader::ParseSpinCtrlLook(MXmlElement* pElement)
{
	MLOOKBase LookHeader;
	if(ParseLookHeader(pElement, LookHeader) == false)
		return;

	MBSpinCtrlLook* pSpinCtrlLook = new MBSpinCtrlLook();
	if(!m_pMint->m_pLookManager->m_SpinCtrlLookMap.insert(map<string, MBSpinCtrlLook*>::value_type(string(LookHeader.strName.c_str()), (MBSpinCtrlLook*)pSpinCtrlLook)).second)
		OutputDebugString("insert MBSpinCtrlLook failed.\n");

	pSpinCtrlLook->SetStretchBlt( LookHeader.bStretch);

	MXmlElement* child;
	for( child = pElement->FirstChildElement(); child != NULL; child = child->NextSiblingElement() )
	{
		if (IsSameTag(child, MINT_XML_TAG_IMAGE))
		{
			string strImageName;
			if (_Contents(strImageName, child, "", m_pXml))
				pSpinCtrlLook->SetBitmap( strImageName);
		}

		// 타일 형식의 비트맵을 사용하는지 여부
		else if ( IsSameTag( child, MINT_XML_TAG_TILED))
		{
			bool bTiled = false;
			_Contents( &bTiled, child, false);
			pSpinCtrlLook->SetTiled( bTiled);
		}

		// 에디트박스 상태에 따른 비트맵을 입력받음
		else if ( IsSameTag( child, MINT_XML_ATTR_STAT_NORMAL))
			ParseSpinCtrlLookState( child, pSpinCtrlLook, 0);

		else if ( IsSameTag( child, MINT_XML_ATTR_STAT_OVER))
			ParseSpinCtrlLookState( child, pSpinCtrlLook, 1);

		else if ( IsSameTag( child, MINT_XML_ATTR_STAT_FOCUS))
			ParseSpinCtrlLookState( child, pSpinCtrlLook, 1);

		else if ( IsSameTag( child, MINT_XML_ATTR_STAT_PRESS))
			ParseSpinCtrlLookState( child, pSpinCtrlLook, 1);

		else if ( IsSameTag( child, MINT_XML_ATTR_STAT_DISABLE))
			ParseSpinCtrlLookState( child, pSpinCtrlLook, 2);
	}
}

void MintLoader::ParseSpinCtrlLookState(MXmlElement* pElement, MBSpinCtrlLook* pSpinCtrlLook, int stat)
{
	int type;
	_Attribute( type, pElement, MINT_XML_ATTR_TYPE, 0);


	MXmlElement* child;
	for( child = pElement->FirstChildElement(); child != NULL; child = child->NextSiblingElement() )
	{
		if (IsSameTag(child, MINT_XML_TAG_ASSIGNMENT))
		{
			int   index	= 0;
			float left	= 0.0f;
			float right = 1.0f;
			float top	= 0.0f;
			float bottom= 1.0f;

			_Attribute(&index	, child, MINT_XML_ATTR_TYPE		, 0);
			_Attribute(&left	, child, MINT_XML_ATTR_LEFT		, 0.0f);
			_Attribute(&top		, child, MINT_XML_ATTR_TOP		, 0.0f);
			_Attribute(&right	, child, MINT_XML_ATTR_WIDTH	, 1.0f);
			_Attribute(&bottom	, child, MINT_XML_ATTR_HEIGHT	, 1.0f);

			pSpinCtrlLook->SetAssignRect( type, stat, index, left, top, right, bottom);
		}
	}
}

void MintLoader::ParseTextAreaLook(MXmlElement* pElement)
{
	MLOOKBase LookHeader;
	if(ParseLookHeader(pElement, LookHeader) == false)
		return;

	MBTextAreaLook* pTextAreaLook = new MBTextAreaLook();
	if(!m_pMint->m_pLookManager->m_TextAreaLookMap.insert(map<string, MBTextAreaLook*>::value_type(string(LookHeader.strName.c_str()), (MBTextAreaLook*)pTextAreaLook)).second)
		OutputDebugString("insert MBTextAreaLook failed.\n");

	pTextAreaLook->SetStretchBlt( LookHeader.bStretch);

	MXmlElement* child;
	for( child = pElement->FirstChildElement(); child != NULL; child = child->NextSiblingElement() )
	{
		if (IsSameTag(child, MINT_XML_TAG_IMAGE))
		{
			string strImageName;
			if (_Contents(strImageName, child, "", m_pXml))
				pTextAreaLook->SetBitmap( strImageName);
		}

		else if ( IsSameTag( child, MINT_XML_TAG_FONT))
		{
			string strFont;
			_Contents( strFont, child, "", m_pXml);

			MFont* pFont = MFontManager::Get( strFont.c_str());
			pTextAreaLook->SetFont( pFont);
		}

		// 타일 형식의 비트맵을 사용하는지 여부
		else if ( IsSameTag( child, MINT_XML_TAG_TILED))
		{
			bool bTiled = false;
			_Contents( &bTiled, child, false);
			pTextAreaLook->SetTiled( bTiled);
		}

		// 텍스트 박스 상태에 따른 비트맵/폰트 색상 등을 입력받음
		else if ( IsSameTag( child, MINT_XML_ATTR_STAT_NORMAL))
			ParseTextAreaLookState( child, pTextAreaLook, 0);

		else if ( IsSameTag( child, MINT_XML_ATTR_STAT_OVER))
			ParseTextAreaLookState( child, pTextAreaLook, 1);

		else if ( IsSameTag( child, MINT_XML_ATTR_STAT_FOCUS))
			ParseTextAreaLookState( child, pTextAreaLook, 1);

		else if ( IsSameTag( child, MINT_XML_ATTR_STAT_PRESS))
			ParseTextAreaLookState( child, pTextAreaLook, 1);

		else if ( IsSameTag( child, MINT_XML_ATTR_STAT_DISABLE))
			ParseTextAreaLookState( child, pTextAreaLook, 2);
	}
}

void MintLoader::ParseTextAreaLookState(MXmlElement* pElement, MBTextAreaLook* pTextAreaLook, int stat)
{
	MXmlElement* child;
	for( child = pElement->FirstChildElement(); child != NULL; child = child->NextSiblingElement() )
	{
		if (IsSameTag(child, MINT_XML_TAG_ASSIGNMENT))
		{
			int   index	= 0;
			float left	= 0.0f;
			float right = 1.0f;
			float top	= 0.0f;
			float bottom= 1.0f;

			_Attribute(&index	, child, MINT_XML_ATTR_TYPE		, 0);
			_Attribute(&left	, child, MINT_XML_ATTR_LEFT		, 0.0f);
			_Attribute(&top		, child, MINT_XML_ATTR_TOP		, 0.0f);
			_Attribute(&right	, child, MINT_XML_ATTR_WIDTH	, 1.0f);
			_Attribute(&bottom	, child, MINT_XML_ATTR_HEIGHT	, 1.0f);

			pTextAreaLook->SetAssignRect( stat, index, left, top, right, bottom);
		}
	}
}

void MintLoader::ParseGroupLook(MXmlElement* pElement)
{
	MLOOKBase LookHeader;
	if(ParseLookHeader(pElement, LookHeader) == false)
		return;

	MBGroupLook* pGroupLook = new MBGroupLook();
	if(!m_pMint->m_pLookManager->m_GroupLookMap.insert(map<string, MBGroupLook*>::value_type(string(LookHeader.strName.c_str()), (MBGroupLook*)pGroupLook)).second)
		OutputDebugString("insert MBGroupLook failed.\n");

	pGroupLook->SetStretchBlt( LookHeader.bStretch);

	MXmlElement* child;
	for( child = pElement->FirstChildElement(); child != NULL; child = child->NextSiblingElement() )
	{
		if (IsSameTag(child, MINT_XML_TAG_IMAGE))
		{
			string strImageName;
			if (_Contents(strImageName, child, "", m_pXml))
				pGroupLook->SetBitmap( strImageName);
		}

		else if ( IsSameTag( child, MINT_XML_TAG_FONT))
		{
			string strFont;
			_Contents( strFont, child, "", m_pXml);

			MFont* pFont = MFontManager::Get( strFont.c_str());
			pGroupLook->SetFont( pFont);
		}

		// 타일 형식의 비트맵을 사용하는지 여부
		else if ( IsSameTag( child, MINT_XML_TAG_TILED))
		{
			bool bTiled = false;
			_Contents( &bTiled, child, false);
			pGroupLook->SetTiled( bTiled);
		}

		else if (IsSameTag(child, MINT_XML_TAG_TEXTOFFSET))
		{
			int x = 0, y = 0;

			_Attribute( &x, child, MINT_XML_ATTR_X);
			_Attribute( &y, child, MINT_XML_ATTR_Y);

			pGroupLook->SetTextOffset( x, y);
		}

		// 텍스트 박스 상태에 따른 비트맵/폰트 색상 등을 입력받음
		else if ( IsSameTag( child, MINT_XML_ATTR_STAT_NORMAL))
			ParseGroupLookState( child, pGroupLook, 0);

		else if ( IsSameTag( child, MINT_XML_ATTR_STAT_DISABLE))
			ParseGroupLookState( child, pGroupLook, 1);
	}
}

void MintLoader::ParseGroupLookState(MXmlElement* pElement, MBGroupLook* pGroupLook, int stat)
{
	MXmlElement* child;
	for( child = pElement->FirstChildElement(); child != NULL; child = child->NextSiblingElement() )
	{
		if (IsSameTag(child, MINT_XML_TAG_ASSIGNMENT))
		{
			int   index	= 0;
			float left	= 0.0f;
			float right = 1.0f;
			float top	= 0.0f;
			float bottom= 1.0f;

			_Attribute(&index	, child, MINT_XML_ATTR_TYPE		, 0);
			_Attribute(&left	, child, MINT_XML_ATTR_LEFT		, 0.0f);
			_Attribute(&top		, child, MINT_XML_ATTR_TOP		, 0.0f);
			_Attribute(&right	, child, MINT_XML_ATTR_WIDTH	, 1.0f);
			_Attribute(&bottom	, child, MINT_XML_ATTR_HEIGHT	, 1.0f);

			pGroupLook->SetAssignRect( stat, index, left, top, right, bottom);
		}

		else if ( IsSameTag( child, MINT_XML_TAG_FONTCOLOR))
		{
			int r = 255, g = 255, b = 255;

			_Attribute( &r, child, MINT_XML_ATTR_R);
			_Attribute( &g, child, MINT_XML_ATTR_G);
			_Attribute( &b, child, MINT_XML_ATTR_B);

			pGroupLook->SetFontColor( stat, MCOLOR( r, g, b));
		}
	}
}


void MintLoader::ParseTreeViewLook(MXmlElement* pElement)
{
	MLOOKBase LookHeader;
	if(ParseLookHeader(pElement, LookHeader) == false)
		return;

	MBTreeViewLook* pTreeViewLook = new MBTreeViewLook();
	if(!m_pMint->m_pLookManager->m_TreeViewLookMap.insert(map<string, MBTreeViewLook*>::value_type(string(LookHeader.strName.c_str()), (MBTreeViewLook*)pTreeViewLook)).second)
		OutputDebugString("insert MBTreeViewLook failed.\n");

	pTreeViewLook->SetStretchBlt( LookHeader.bStretch);

	MXmlElement* child;
	for( child = pElement->FirstChildElement(); child != NULL; child = child->NextSiblingElement() )
	{
		if (IsSameTag(child, MINT_XML_TAG_IMAGE))
		{
			string strImageName;
			if (_Contents(strImageName, child, "", m_pXml))
				pTreeViewLook->SetBitmap( strImageName);
		}

		else if ( IsSameTag( child, MINT_XML_TAG_FONT))
		{
			string strFont;
			_Contents( strFont, child, "", m_pXml);

			MFont* pFont = MFontManager::Get( strFont.c_str());
			pTreeViewLook->SetFont( pFont);
		}

		// 타일 형식의 비트맵을 사용하는지 여부
		else if ( IsSameTag( child, MINT_XML_TAG_TILED))
		{
			bool bTiled = false;
			_Contents( &bTiled, child, false);
			pTreeViewLook->SetTiled( bTiled);
		}

		// TreeView 상태에 따른 비트맵/폰트 색상 등을 입력받음
		else if ( IsSameTag( child, MINT_XML_ATTR_STAT_NORMAL))
			ParseTreeViewLookState( child, pTreeViewLook, 0);

		else if ( IsSameTag( child, MINT_XML_ATTR_STAT_OVER))
			ParseTreeViewLookState( child, pTreeViewLook, 1);

		else if ( IsSameTag( child, MINT_XML_ATTR_STAT_FOCUS))
			ParseTreeViewLookState( child, pTreeViewLook, 1);

		else if ( IsSameTag( child, MINT_XML_ATTR_STAT_PRESS))
			ParseTreeViewLookState( child, pTreeViewLook, 1);

		else if ( IsSameTag( child, MINT_XML_ATTR_STAT_DISABLE))
			ParseTreeViewLookState( child, pTreeViewLook, 2);
	}
}

void MintLoader::ParseTreeViewLookState(MXmlElement* pElement, MBTreeViewLook* pTreeViewLook, int stat)
{
	int type;
	_Attribute( type, pElement, MINT_XML_ATTR_TYPE, 0);


	MXmlElement* child;
	for( child = pElement->FirstChildElement(); child != NULL; child = child->NextSiblingElement() )
	{
		if (IsSameTag(child, MINT_XML_TAG_ASSIGNMENT))
		{
			int   index	= 0;
			float left	= 0.0f;
			float right = 1.0f;
			float top	= 0.0f;
			float bottom= 1.0f;

			_Attribute(&index	, child, MINT_XML_ATTR_TYPE		, 0);
			_Attribute(&left	, child, MINT_XML_ATTR_LEFT		, 0.0f);
			_Attribute(&top		, child, MINT_XML_ATTR_TOP		, 0.0f);
			_Attribute(&right	, child, MINT_XML_ATTR_WIDTH	, 1.0f);
			_Attribute(&bottom	, child, MINT_XML_ATTR_HEIGHT	, 1.0f);

			pTreeViewLook->SetAssignRect( type, stat, index, left, top, right, bottom);
		}

		else if ( IsSameTag( child, MINT_XML_TAG_FONTCOLOR))
		{
			int r = 255, g = 255, b = 255;

			_Attribute( &r, child, MINT_XML_ATTR_R);
			_Attribute( &g, child, MINT_XML_ATTR_G);
			_Attribute( &b, child, MINT_XML_ATTR_B);

			pTreeViewLook->SetFontColor( type, stat, MCOLOR( r, g, b));
		}
	}
}

MBPopupBoxLook* MintLoader::ParsePopupBoxLook(MXmlElement* pElement)
{
	MLOOKBase LookHeader;
	if(ParseLookHeader(pElement, LookHeader) == false)
		return NULL;

	MBPopupBoxLook* pPopupBoxLook = new MBPopupBoxLook();
	if(!m_pMint->m_pLookManager->m_PopupBoxLookMap.insert(map<string, MBPopupBoxLook*>::value_type(string(LookHeader.strName.c_str()), (MBPopupBoxLook*)pPopupBoxLook)).second)
		OutputDebugString("insert MBPopupBoxLook failed.\n");

	pPopupBoxLook->SetStretchBlt( LookHeader.bStretch);

	MXmlElement* child;
	for( child = pElement->FirstChildElement(); child != NULL; child = child->NextSiblingElement() )
	{
		if (IsSameTag(child, MINT_XML_TAG_IMAGE))
		{
			string strImageName;
			if (_Contents(strImageName, child, "", m_pXml))
				pPopupBoxLook->SetBitmap( strImageName);
		}

		// 타일 형식의 비트맵을 사용하는지 여부
		else if ( IsSameTag( child, MINT_XML_TAG_TILED))
		{
			bool bTiled = false;
			_Contents( &bTiled, child, false);
			pPopupBoxLook->SetTiled( bTiled);
		}

		else if ( IsSameTag( child, "Offset"))
		{
			int x = 0, y = 0;
			_Attribute( &x, child, MINT_XML_ATTR_X, 0);
			_Attribute( &y, child, MINT_XML_ATTR_Y, 0);
			pPopupBoxLook->SetOffset( x, y);
		}

		// 상태에 따른 비트맵/폰트 색상 등을 입력받음
		else if ( IsSameTag( child, MINT_XML_ATTR_STAT_NORMAL))
			ParsePopupBoxLookState( child, pPopupBoxLook);
	}

	return pPopupBoxLook;
}

void MintLoader::ParsePopupBoxLookState(MXmlElement* pElement, MBPopupBoxLook* pPopupBoxLook)
{
	MXmlElement* child;
	for( child = pElement->FirstChildElement(); child != NULL; child = child->NextSiblingElement() )
	{
		if (IsSameTag(child, MINT_XML_TAG_ASSIGNMENT))
		{
			int   index	= 0;
			float left	= 0.0f;
			float right = 1.0f;
			float top	= 0.0f;
			float bottom= 1.0f;

			_Attribute(&index	, child, MINT_XML_ATTR_TYPE		, 0);
			_Attribute(&left	, child, MINT_XML_ATTR_LEFT		, 0.0f);
			_Attribute(&top		, child, MINT_XML_ATTR_TOP		, 0.0f);
			_Attribute(&right	, child, MINT_XML_ATTR_WIDTH	, 1.0f);
			_Attribute(&bottom	, child, MINT_XML_ATTR_HEIGHT	, 1.0f);

			pPopupBoxLook->SetAssignRect( index, left, top, right, bottom);
		}
	}
}

void MintLoader::ParseTooltipLook(MXmlElement* pElement)
{
	MBPopupBoxLook* pLook = ParsePopupBoxLook( pElement);
	if ( pLook)
		MGetMint()->SetTooltipLook( (MTooltipLook*)pLook);
}

void MintLoader::ParseBitmap(MXmlElement* pElement)
{
	string strName, strFile;

	_Attribute(strName, pElement, MINT_XML_ATTR_NAME, "");
	if (strName.empty()) return;

	_Attribute(strFile, pElement, MINT_XML_ATTR_FILE, "");
	if (strFile.empty()) return;

	if (!MGetMint()->GetDC()) return;

	// Bitmap 생성
	MBitmap* pNewBitmap = MGetMint()->NewBitmap(strName.c_str(), strFile.c_str());
	if (pNewBitmap == NULL) return;

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
	_Attribute(strAliasName, pElement, MINT_XML_ATTR_NAME, m_pXml);
	if ( strAliasName.empty())
		return;

	// Get font name
	string strFontName;
	_Attribute(strFontName, pElement, MINT_XML_ATTR_FONT, m_pXml);

	// Get file name
	string strFile;
	_Attribute( strFile, pElement, MINT_XML_ATTR_FILE, m_pXml);

	// Get height
	int nHeight = 10;
	_Attribute( &nHeight, pElement, MINT_XML_ATTR_HEIGHT);

	// Get outline
	bool bOutline = false;
	_Attribute( &bOutline, pElement, MINT_XML_ATTR_OUTLINE);

	// Get outline opacity
	float fOutlineOpacity = 1.0f;
	_Attribute( &fOutlineOpacity, pElement, MINT_XML_ATTR_OUTLINEOPACITY, 1.0f);

	// Get antialias
	bool bAntiAlias = false;
	_Attribute( &bAntiAlias, pElement, MINT_XML_ATTR_ANTIALIAS);

	// Create font
//	CreateFont( strAliasName.c_str(), strFontName.c_str(), strFile.c_str(), nHeight);

	MFont* pFont = MGetMint()->NewFont( strAliasName.c_str(), strFontName.c_str(), strFile.c_str(), nHeight, bOutline, fOutlineOpacity, bAntiAlias );
	MFontManager::Add( pFont);
}

void MintLoader::ParseCursor(MXmlElement* pElement)
{
	// Get name
	string strAliasName;
	_Attribute(strAliasName, pElement, MINT_XML_ATTR_NAME, m_pXml);
	if ( strAliasName.empty())
		return;

	MCursor* pCursor = MGetMint()->NewCursor( strAliasName.c_str() );
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
	}

}

void MintLoader::ParseAnimationTemplate(MXmlElement* pElement)
{
	// Get name
	string strAliasName;
	_Attribute(strAliasName, pElement, MINT_XML_ATTR_NAME, m_pXml);
	if ( strAliasName.empty())
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


void MintLoader::ParseListBox(MXmlElement* pElement, MWidget* pWidget)
{
	ParseWidget(pElement, pWidget);

	MListBox* pListBox = (MListBox*)pWidget;

	MXmlElement* child;
	for( child = pElement->FirstChildElement(); child != NULL; child = child->NextSiblingElement() )
	{
		if (IsSameTag(child, MINT_XML_TAG_LOOK))
		{
			string strType;
			_Attribute( strType, child, MINT_XML_ATTR_TYPE, "");

			string strSkinName;
			_Contents( strSkinName, child, "", m_pXml);

			if (!_stricmp(strType.c_str(), MINT_XML_TAG_LISTBOX))
			{
				map<string, MBListBoxLook*>::iterator itor = m_pMint->m_pLookManager->m_ListBoxLookMap.find(strSkinName.c_str());
				if (itor != m_pMint->m_pLookManager->m_ListBoxLookMap.end())
				{
					((MBListBoxLook*)(*itor).second)->SetCustomLook(0);
					pListBox->ChangeCustomLook((MBListBoxLook*)(*itor).second);					
				}
				else
				{
					if (!_stricmp(strSkinName.c_str(), MINT_XML_LOOK_VALUE_NULL))
					{
						pListBox->ChangeLookToNull();
					}
				}
			}

			else if (!_stricmp(strType.c_str(), MINT_XML_TAG_SCROLLBAR))
			{
				map<string, MBScrollBarLook*>::iterator itor = m_pMint->m_pLookManager->m_ScrollBarLookMap.find(strSkinName.c_str());
				if (itor != m_pMint->m_pLookManager->m_ScrollBarLookMap.end())
					pListBox->SetScrollBarLook( (MBScrollBarLook*)(*itor).second);
			}
		}

		else if (IsSameTag(child, MINT_XML_TAG_SHAPE))
		{
			MXmlElement* pShape;
			for( pShape = child->FirstChildElement(); pShape != NULL; pShape = pShape->NextSiblingElement() )
			{
				if(IsSameTag(pShape, MINT_XML_TAG_SCROLLBAR))
				{
					string strScrollType;
					_Attribute(strScrollType, pShape, MINT_XML_ATTR_TYPE, "vertical");
					if(strScrollType == string("vertical")) pListBox->SetScrollBarType(MSBT_VERTICAL);
					// 일단 수평 스크롤바는 제외한다.
					//else if(strScrollType == string("horizontal")) pListBox->SetScrollBarType(MSBT_HORIZONTAL);

					string strScrollSide;
					_Attribute(strScrollSide, pShape, MINT_XML_ATTR_SIDE, MINT_XML_ATTR_RIGHT);
					if(strScrollSide == string(MINT_XML_ATTR_LEFT)) pListBox->SetScrollBarBase(MSB_LEFT);
					else if(strScrollSide == string(MINT_XML_ATTR_RIGHT)) pListBox->SetScrollBarBase(MSB_RIGHT);
					//else if(strScrollSide == string(MINT_XML_ATTR_TOP)) pListBox->SetScrollBarBase(MSB_TOP);
					//else if(strScrollSide == string(MINT_XML_ATTR_BOTTOM)) pListBox->SetScrollBarBase(MSB_BOTTOM);


					// 스크롤바 두께
					int width;
					_Attribute( &width, pShape, MINT_XML_ATTR_WIDTH, 15);
					pListBox->SetScrollBarWidth( width);

					// 스크롤바 항상 보이는지 여부
					bool bAlwaysShow = false;
					_Attribute( &bAlwaysShow, pShape, "alwaysshow", false);
					pListBox->SetAlwaysVisibleScrollbar( bAlwaysShow);
				}

				else if(IsSameTag(pShape, MINT_XML_TAG_VISIBLEHEAD))
				{
					bool bVisibleHead;
					_Contents(&bVisibleHead, pShape, false);
					pListBox->SetVisibleHeader( bVisibleHead);
				}

				else if(IsSameTag(pShape, MINT_XML_TAG_FIELD))
				{
					string strFieldName;
					int nFieldTabSize;
					string strFieldType;
					MListFieldType nFieldType;

					_Attribute(strFieldName,	pShape, MINT_XML_ATTR_NAME,	"", m_pXml);
					_Attribute(&nFieldTabSize,	pShape, MINT_XML_ATTR_WIDTH, 0);
					_Attribute(strFieldType,	pShape, MINT_XML_ATTR_TYPE, MINT_XML_ATTR_STRING, m_pXml);
					if(strFieldType == string(MINT_XML_ATTR_STRING)) nFieldType = MLFT_STRING;
					else if(strFieldType == string(MINT_XML_ATTR_ICON)) nFieldType = MLFT_BITMAP;

					pListBox->AddField( strFieldName.c_str(), nFieldTabSize, nFieldType);
				}

				else if(IsSameTag(pShape, MINT_XML_TAG_ITEMHEIGHT))
				{
					int nItemHeight;
					_Contents(&nItemHeight, pShape, false);
					pListBox->SetItemHeight( nItemHeight);
				}
				else if(IsSameTag(pShape, MINT_XML_TAG_DROPABLE))
				{
					bool bDragDrop;
					_Contents(&bDragDrop, pShape, false);
					pListBox->EnableDragAndDrop(bDragDrop);
				}
			}
		}
	}
}

void MintLoader::ParseComboBox(MXmlElement* pElement, MWidget* pWidget)
{
	ParseWidget(pElement, pWidget);

	MComboBox* pCombo = (MComboBox*)pWidget;

	MXmlElement* child;
	for( child = pElement->FirstChildElement(); child != NULL; child = child->NextSiblingElement() )
	{
		if (IsSameTag(child, MINT_XML_TAG_LOOK))
		{
			string strType;
			_Attribute( strType, child, MINT_XML_ATTR_TYPE, "");

			string strSkinName;
			_Contents( strSkinName, child, "", m_pXml);

			if ( strType == string( MINT_XML_TAG_BUTTON))
			{
				map<string, MButtonLook*>::iterator itor = m_pMint->m_pLookManager->m_ButtonLookMap.find(strSkinName.c_str());
				if (itor != m_pMint->m_pLookManager->m_ButtonLookMap.end())
					pCombo->ChangeCustomLook( (MButtonLook*)(*itor).second);
			}
			
			else if ( strType == string( MINT_XML_TAG_LISTBOX))
			{
				map<string, MBListBoxLook*>::iterator itor = m_pMint->m_pLookManager->m_ListBoxLookMap.find(strSkinName.c_str());
				if (itor != m_pMint->m_pLookManager->m_ListBoxLookMap.end())
					pCombo->SetListBoxLook( (MBListBoxLook*)(*itor).second);
			}

			else if ( strType == string( MINT_XML_TAG_SCROLLBAR))
			{
				map<string, MBScrollBarLook*>::iterator itor = m_pMint->m_pLookManager->m_ScrollBarLookMap.find(strSkinName.c_str());
				if (itor != m_pMint->m_pLookManager->m_ScrollBarLookMap.end())
					pCombo->SetScrollBarLook( (MBScrollBarLook*)(*itor).second);
			}
		}

		else if (IsSameTag(child, MINT_XML_TAG_SHAPE))
		{
			MXmlElement* pShape;
			for( pShape = child->FirstChildElement(); pShape != NULL; pShape = pShape->NextSiblingElement() )
			{
				if(IsSameTag(pShape, MINT_XML_TAG_DROPSIZE))
				{
					int nDropSize;
					_Contents( &nDropSize, pShape, 30);
					pCombo->SetDropSize( nDropSize);
				}

				else if(IsSameTag(pShape, MINT_XML_TAG_ITEMHEIGHT))
				{
					int nItemHeight;
					_Contents(&nItemHeight, pShape, false);
					pCombo->SetItemHeight( nItemHeight);
				}

				else if(IsSameTag(pShape, MINT_XML_TAG_ITEM))
				{
					string strItem;
					_Contents( strItem, pShape, "", m_pXml);

					if ( strItem.empty() == false)
						pCombo->Add( strItem.c_str());
				}
				
				else if(IsSameTag(pShape, MINT_XML_TAG_SCROLLBAR))
				{
					// 스크롤바 두께
					int width;
					_Attribute( &width, pShape, MINT_XML_ATTR_WIDTH, 15);
					pCombo->SetScrollBarWidth( width);
				}
			}
		}
	}
}

void MintLoader::ParseScrollBar(MXmlElement* pElement, MWidget* pWidget)
{
	ParseWidget(pElement, pWidget);

	MScrollBar* pScroll = (MScrollBar*)pWidget;

	MXmlElement* child;
	for( child = pElement->FirstChildElement(); child != NULL; child = child->NextSiblingElement() )
	{
		if (IsSameTag(child, MINT_XML_TAG_LOOK))
		{
			string strSkinName;
			if (_Contents(strSkinName, child, "", m_pXml))
			{
				MFrame* pFrame = (MFrame*)pWidget;
				map<string, MBScrollBarLook*>::iterator itor = m_pMint->m_pLookManager->m_ScrollBarLookMap.find(strSkinName.c_str());
				if (itor != m_pMint->m_pLookManager->m_ScrollBarLookMap.end())
				{
					((MBScrollBarLook*)(*itor).second)->SetCustomLook(0);
					pScroll->ChangeCustomLook((MBScrollBarLook*)(*itor).second);				
				}
			}
		}

		else if (IsSameTag(child, MINT_XML_TAG_SHAPE))
		{
			MXmlElement* pShape;
			for( pShape = child->FirstChildElement(); pShape != NULL; pShape = pShape->NextSiblingElement() )
			{
				if ( IsSameTag(pShape, MINT_XML_TAG_ORIENTATION))
				{
					string dir;
					_Contents( dir, pShape, "", m_pXml);

					if ( dir == string("vertical"))
						pScroll->SetType( MSBT_VERTICAL);
					else if ( dir == string("horizontal"))
						pScroll->SetType( MSBT_HORIZONTAL);
				}

				else if ( IsSameTag(pShape, MINT_XML_TAG_RANGE))
				{
					// 최대/최소
					int min = 0, max = 100;
					_Attribute( &min, pShape, MINT_XML_ATTR_MIN, 0);
					_Attribute( &max, pShape, MINT_XML_ATTR_MAX, 100);
					pScroll->SetRange( min, max);
				}

				else if ( IsSameTag(pShape, MINT_XML_TAG_VALUE))
				{
					int value;
					_Contents( &value, pShape, 0);

					pScroll->SetValue( value);
				}
			}
		}
	}
}

void MintLoader::ParseSpinCtrl(MXmlElement* pElement, MWidget* pWidget)
{
	ParseWidget(pElement, pWidget);

	MSpinCtrl* pSpinCtrl = (MSpinCtrl*)pWidget;

	MXmlElement* child;
	for( child = pElement->FirstChildElement(); child != NULL; child = child->NextSiblingElement() )
	{
		if (IsSameTag(child, MINT_XML_TAG_LOOK))
		{
			string strSkinName;
			if (_Contents(strSkinName, child, "", m_pXml))
			{
				MFrame* pFrame = (MFrame*)pWidget;
				map<string, MBSpinCtrlLook*>::iterator itor = m_pMint->m_pLookManager->m_SpinCtrlLookMap.find(strSkinName.c_str());
				if (itor != m_pMint->m_pLookManager->m_SpinCtrlLookMap.end())
				{
					((MBSpinCtrlLook*)(*itor).second)->SetCustomLook(0);
					pSpinCtrl->ChangeCustomLook((MBSpinCtrlLook*)(*itor).second);				
				}
			}
		}

		else if (IsSameTag(child, MINT_XML_TAG_SHAPE))
		{
			MXmlElement* pShape;
			for( pShape = child->FirstChildElement(); pShape != NULL; pShape = pShape->NextSiblingElement() )
			{
				if ( IsSameTag(pShape, MINT_XML_TAG_ORIENTATION))
				{
					string dir;
					_Contents( dir, pShape, "", m_pXml);

					if ( dir == string("vertical"))
						pSpinCtrl->SetType( MSPT_VERTICAL);
					else if ( dir == string("horizontal"))
						pSpinCtrl->SetType( MSPT_HORIZONTAL);
				}

				else if ( IsSameTag(pShape, MINT_XML_TAG_BUDDY))
				{
					string buddy;
					_Contents( buddy, pShape, "", m_pXml);

					MWidget* pBuddy = m_pMint->FindWidget( buddy);
					if ( pBuddy)
						pSpinCtrl->SetBuddy( pBuddy);
				}
			}
		}
	}
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

				// 무한 재귀호출이 될 수 있음. 조심.
				ParseWidgets(pWidgetElement, pWidget, szWidgetName);
			}
		}
		else if(IsSameTag(child, MINT_XML_TAG_SHAPE))
		{
			MXmlElement* pShapeElement;
			for( pShapeElement = child->FirstChildElement(); pShapeElement != NULL; pShapeElement=pShapeElement->NextSiblingElement() )
			{
				if(IsSameTag(pShapeElement, MINT_XML_TAG_STYLE))
				{
					string strType;
					_Attribute(strType, pShapeElement, MINT_XML_ATTR_TYPE, "");
					if(strType == string("none"))
					{
						pPanel->SetBorderStyle(MBS_NONE);
					}
				}
				else if(IsSameTag(pShapeElement, MINT_XML_TAG_BACKGROUNDCOLOR))
				{
					int a = 0xFF, r = 0xFF,g = 0xFF, b = 0xFF;

					_Attribute(&a,	pShapeElement, MINT_XML_ATTR_A);
					_Attribute(&r,	pShapeElement, MINT_XML_ATTR_R);
					_Attribute(&g,	pShapeElement, MINT_XML_ATTR_G);
					_Attribute(&b,	pShapeElement, MINT_XML_ATTR_B);

					pPanel->SetBackgroundColor(MCOLOR(r,g,b,a));
				}
				else if(IsSameTag(pShapeElement, MINT_XML_TAG_BORDERCOLOR))
				{
					int a = 0xFF, r = 0xFF,g = 0xFF, b = 0xFF;

					_Attribute(&a,	pShapeElement, MINT_XML_ATTR_A);
					_Attribute(&r,	pShapeElement, MINT_XML_ATTR_R);
					_Attribute(&g,	pShapeElement, MINT_XML_ATTR_G);
					_Attribute(&b,	pShapeElement, MINT_XML_ATTR_B);

					pPanel->SetBorderColor(MCOLOR(r,g,b,a));
				}
			}
		}
	}
}

void MintLoader::ParsePaint(MXmlElement* pElement, MWidget* pWidget)
{
	ParseWidget(pElement, pWidget);
}

void MintLoader::ParseTabCtrl(MXmlElement* pElement, MWidget* pWidget)
{
	ParseWidget(pElement, pWidget);

	MXmlElement* child = pElement->FirstChildElement();
	for( child; child != NULL; child = child->NextSiblingElement() )
	{
		if(IsSameTag(child, MINT_XML_TAG_LOOK))
		{
			string strSkinName;
			if (_Contents(strSkinName, child, "", m_pXml))
			{
				if (!_stricmp(strSkinName.c_str(), MINT_XML_LOOK_VALUE_NULL))
				{
					MTabCtrl* pTabCtrl = (MTabCtrl*)pWidget;
					pTabCtrl->ChangeLookToNull();
				}
			}
		}
		else if(IsSameTag(child, MINT_XML_TAG_TABPAGES))
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

				// 무한 재귀호출이 될 수 있음. 조심.
				ParseWidgets(pWidgetElement, pWidget, szWidgetName);
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
		{
			char szTabPage[256] = "";
			ParseWidgetName(szTabPage, child, pWidget);

			ParseTabPage(child, pWidget, szTabPage);
		}
	}
}

void MintLoader::ParseTabPage( MXmlElement* pElement, MWidget* pWidget, char* chTabPageName )
{
	MWidget* pTabWidget		= NULL;
	MWidget* pPageWidget	= NULL;

	MXmlElement* child;
	for( child = pElement->FirstChildElement(); child != NULL; child = child->NextSiblingElement() )
	{

		if(IsSameTag(child, MINT_XML_TAG_TAB))
		{
			char szTabName[512] = "";	
			sprintf(szTabName, "%sTab", chTabPageName);

			pTabWidget = NewWidget(MINT_BUTTON, szTabName, pWidget);
			ParseButton(child, pTabWidget);
		}
		else if(IsSameTag(child, MINT_XML_TAG_PAGE))
		{
			char szPageName[512] = "";
			sprintf(szPageName, "%sPage", chTabPageName);

			pPageWidget = NewWidget(MINT_PANEL, szPageName, pWidget);
			((MPanel*)pPageWidget)->SetBorderStyle(MBS_NONE);
			ParsePanel(child, pPageWidget);
		}
	}

	MTabCtrl* pTabCtrl	= (MTabCtrl*)pWidget;

	if(pTabWidget && pPageWidget)
	{
		pTabCtrl->Add((MButton*)pTabWidget, (MPanel*)pPageWidget);
	}
}


void MintLoader::ParseView(MXmlElement* pElement, MWidget* pWidget)
{
	ParseWidget(pElement, pWidget);

	MView* pView = (MView*)pWidget;

	MXmlElement* child;
	for( child = pElement->FirstChildElement(); child != NULL; child = child->NextSiblingElement() )
	{
		if (IsSameTag(child, MINT_XML_TAG_LOOK))
		{
			string strType;
			_Attribute( strType, child, MINT_XML_ATTR_TYPE, "");

			string strSkinName;
			_Contents( strSkinName, child, "", m_pXml);

			if (!_stricmp(strType.c_str(), MINT_XML_TAG_SCROLLBAR))
			{
				map<string, MBScrollBarLook*>::iterator itor = m_pMint->m_pLookManager->m_ScrollBarLookMap.find(strSkinName.c_str());
				if (itor != m_pMint->m_pLookManager->m_ScrollBarLookMap.end())
					pView->SetScrollBarLook( (MBScrollBarLook*)(*itor).second);
			}
		}

		else if ( IsSameTag( child, MINT_XML_TAG_SHAPE))
		{
			MXmlElement* pShape;
			for( pShape = child->FirstChildElement(); pShape != NULL; pShape=pShape->NextSiblingElement() )
			{
				if(IsSameTag(pShape, MINT_XML_TAG_SCROLLBAR))
				{
					// 스크롤바 두께
					int width;
					_Attribute( &width, pShape, MINT_XML_ATTR_WIDTH, 15);
					pView->SetScrollBarWidth( width);
				}
			}
		}

		else if (IsSameTag(child, MINT_XML_TAG_CONTROLS))
		{
			MXmlElement* pWidgetElement = child->FirstChildElement();
			for ( pWidgetElement; pWidgetElement != NULL; pWidgetElement=pWidgetElement->NextSiblingElement() )
			{
				char szWidgetName[ 256];
				ParseWidgetName( szWidgetName, pWidgetElement, pView->GetPanel());

				// 무한 재귀호출이 될 수 있음. 조심.
				ParseWidgets( pWidgetElement, pView->GetPanel(), szWidgetName);
			}
		}
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
				ParseWidgets( pWidgetElement, pWidget, szWidgetName);
			}
		}
	}
}

void MintLoader::ParseTextView(MXmlElement* pElement, MWidget* pWidget)
{
	ParseWidget(pElement, pWidget);

	MTextView* pTextView = (MTextView*)pWidget;

	MXmlElement* child;
	for( child = pElement->FirstChildElement(); child != NULL; child = child->NextSiblingElement() )
	{
		if (IsSameTag(child, MINT_XML_TAG_LOOK))
		{
			string strType;
			_Attribute( strType, child, MINT_XML_ATTR_TYPE, "");

			string strSkinName;
			_Contents( strSkinName, child, "", m_pXml);

			if (!_stricmp(strType.c_str(), MINT_XML_TAG_SCROLLBAR))
			{
				map<string, MBScrollBarLook*>::iterator itor = m_pMint->m_pLookManager->m_ScrollBarLookMap.find(strSkinName.c_str());
				if (itor != m_pMint->m_pLookManager->m_ScrollBarLookMap.end())
					pTextView->SetScrollbarLook( (MBScrollBarLook*)(*itor).second);
			}
		}

		else if (IsSameTag( child, MINT_XML_TAG_SHAPE))
		{
			MXmlElement* pShape;
			for( pShape = child->FirstChildElement(); pShape != NULL; pShape = pShape->NextSiblingElement() )
			{
				if ( IsSameTag( pShape, MINT_XML_TAG_SCROLLBAR))
				{
					// 스크롤바 보이는 상태
					string strVisibleMode = "auto";
					_Attribute( strVisibleMode, pShape, "visible", "auto");
					if ( strVisibleMode == "true")			pTextView->ShowScrollbar( MSB_SHOW);
					else if ( strVisibleMode == "show")		pTextView->ShowScrollbar( MSB_SHOW);
					else if ( strVisibleMode == "false")	pTextView->ShowScrollbar( MSB_HIDE);
					else if ( strVisibleMode == "hide"	)	pTextView->ShowScrollbar( MSB_HIDE);
					else									pTextView->ShowScrollbar( MSB_AUTO);

					// 스크롤바 두께
					int width;
					_Attribute( &width, pShape, MINT_XML_ATTR_WIDTH, 15);
					pTextView->SetScrollbarWidth( width);

				}
				else if ( IsSameTag( pShape, "Wrap"))
				{
					bool bWrap = true;
					_Contents( &bWrap, pShape, true);

					pTextView->SetWrapping( bWrap);
				}
			}
		}
	}
}

void MintLoader::ParseTreeView(MXmlElement* pElement, MWidget* pWidget)
{
	ParseWidget(pElement, pWidget);

	MTreeView* pTreeView = (MTreeView*)pWidget;

	MXmlElement* child;
	for( child = pElement->FirstChildElement(); child != NULL; child = child->NextSiblingElement() )
	{
		if (IsSameTag(child, MINT_XML_TAG_LOOK))
		{
			string strType;
			_Attribute( strType, child, MINT_XML_ATTR_TYPE, "");

			string strSkinName;
			_Contents( strSkinName, child, "", m_pXml);

			if (!_stricmp(strType.c_str(), MINT_XML_TAG_TREEVIEW))
			{
				map<string, MBTreeViewLook*>::iterator itor = m_pMint->m_pLookManager->m_TreeViewLookMap.find(strSkinName.c_str());
				if (itor != m_pMint->m_pLookManager->m_TreeViewLookMap.end())
				{
					((MBTreeViewLook*)(*itor).second)->SetCustomLook(0);
					pTreeView->ChangeCustomLook((MBTreeViewLook*)(*itor).second);					
				}
				else
				{
					if (!_stricmp(strSkinName.c_str(), MINT_XML_LOOK_VALUE_NULL))
					{
						pTreeView->ChangeLookToNull();
					}
				}
			}
			else if (!_stricmp(strType.c_str(), MINT_XML_TAG_SCROLLBAR))
			{
				map<string, MBScrollBarLook*>::iterator itor = m_pMint->m_pLookManager->m_ScrollBarLookMap.find(strSkinName.c_str());
				if (itor != m_pMint->m_pLookManager->m_ScrollBarLookMap.end())
					pTreeView->SetScrollbarLook( (MBScrollBarLook*)(*itor).second);
			}
		}

		else if (IsSameTag( child, MINT_XML_TAG_SHAPE))
		{
			MXmlElement* pShape;
			for( pShape = child->FirstChildElement(); pShape != NULL; pShape = pShape->NextSiblingElement() )
			{
				if ( IsSameTag( pShape, MINT_XML_TAG_ITEMHEIGHT))
				{
					int nItemHeight;
					_Contents( &nItemHeight, pShape, 17);
					pTreeView->SetItemHeight( nItemHeight);
				}
				
				else if ( IsSameTag( pShape, "CollapsedImage"))
				{
					string strImage;
					_Contents( strImage, pShape, "", m_pXml);
					if ( strImage.empty() == false)
						pTreeView->SetCollapsedImage( strImage.c_str());
				}

				else if ( IsSameTag( pShape, "ExpandedImage"))
				{
					string strImage;
					_Contents( strImage, pShape, "", m_pXml);
					if ( strImage.empty() == false)
						pTreeView->SetExpandedImage( strImage.c_str());
				}

				else if ( IsSameTag( pShape, "AddIcon"))
				{
					string strIcon;
					_Contents( strIcon, pShape, "", m_pXml);
					if ( strIcon.empty() == false)
						pTreeView->AddIcon( strIcon.c_str());
				}

				else if ( IsSameTag( pShape, MINT_XML_TAG_SCROLLBAR))
				{
					// 스크롤바 보이는 상태
					string strVisibleMode = "auto";
					_Attribute( strVisibleMode, pShape, "visible", "auto");
					if ( strVisibleMode == "true")			pTreeView->ShowScrollbar( MSB_SHOW);
					else if ( strVisibleMode == "show")		pTreeView->ShowScrollbar( MSB_SHOW);
					else if ( strVisibleMode == "false")	pTreeView->ShowScrollbar( MSB_HIDE);
					else if ( strVisibleMode == "hide"	)	pTreeView->ShowScrollbar( MSB_HIDE);
					else									pTreeView->ShowScrollbar( MSB_AUTO);

					// 스크롤바 두께
					int width;
					_Attribute( &width, pShape, MINT_XML_ATTR_WIDTH, 15);
					pTreeView->SetScrollbarWidth( width);
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

		else if(IsSameTag(child, MINT_XML_TAG_SHAPE))
		{
			MXmlElement* pShapeElement;
			for( pShapeElement = child->FirstChildElement(); pShapeElement != NULL; pShapeElement=pShapeElement->NextSiblingElement() )
			{
				if(IsSameTag(pShapeElement, MINT_XML_TAG_DROPABLE))
				{
					bool bDragDrop;
					_Contents(&bDragDrop, pShapeElement, false);
					pSheet->EnableDragAndDrop(bDragDrop);
				}
			}
		}

	}

}

} // namespace mint3