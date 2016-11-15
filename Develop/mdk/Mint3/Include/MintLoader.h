#ifndef _MMINT_PARSER_H
#define _MMINT_PARSER_H

#include <string>
#include <list>
#include <map>
#include <stack>
#include <set>
#include <algorithm>

using namespace std;

class TiXmlElement;
class MXml;
typedef TiXmlElement		MXmlElement;

namespace mint3 {

class Mint;
class MWidget;

// Look&Feel
class MBFrameLook;
class MBButtonLook;
class MBEditLook;
class MBListBoxLook;
class MBScrollBarLook;
class MBSpinCtrlLook;
class MBTextAreaLook;
class MBGroupLook;
class MBTreeViewLook;
class MBPopupBoxLook;


struct MLOOKBase
{
	string		strName;					// Look Name
	string		strFile;					// Look FileName
	bool		bStretch;					// Look Stretch
};

class MintLoader
{
private:
protected:
	Mint*			m_pMint;
	MXml*			m_pXml;
	string			m_strBasePath;
	
	list<string>	m_SheetNameStack;
	list<MXml*>		m_XmlStack;
	set<string>		m_IncludedFileNames;

	bool			Load(const char* szFileName, bool bInclude);
	bool			Parse();

	void			ParseInclude(MXmlElement* pElement);

	bool CheckExistIncludedFile( string strFileName );

	MWidget*		NewWidget(const char* szClass, const char* szName, MWidget* pParent);
	bool			IsSameTag(MXmlElement* pElement, const char* szTagName);

	void			PushXml(MXml* pXml);
	void			PopXml();
	MWidget*		FindSheet(string& strSheet);

	//////////////////////////////////////////////////////////////////////////
	// Resources
	void			ParseResource(MXmlElement* pElement);
	void			ParseBitmap(MXmlElement* pElement);
	void			ParseFont(MXmlElement* pElement);
	void			ParseCursor(MXmlElement* pElement);
	void			ParseAnimationTemplate(MXmlElement* pElement);

	//////////////////////////////////////////////////////////////////////////
	// Widgets
	virtual void	ParseWidgets(MXmlElement* pElement, MWidget* pParent, const char* szWidgetName);
	void			ParseWidget(MXmlElement* pElement, MWidget* pWidget);
	void			ParseSheet(MXmlElement* pElement, MWidget* pWidget);
	void			ParseButton(MXmlElement* pElement, MWidget* pWidget);
	void			ParseFrame(MXmlElement* pElement, MWidget* pWidget);
	void			ParseTextArea(MXmlElement* pElement, MWidget* pWidget);
	void			ParseTimer(MXmlElement* pElement, MWidget* pWidget);
	void			ParseLabel(MXmlElement* pElement, MWidget* pWidget);
	void			ParseEdit(MXmlElement* pElement, MWidget* pWidget);
	void			ParsePicture(MXmlElement* pElement, MWidget* pWidget);
	void			ParseAnimation(MXmlElement* pElement, MWidget* pWidget);
	void			ParseListBox(MXmlElement* pElement, MWidget* pWidget);
	void			ParsePanel(MXmlElement* pElement, MWidget* pWidget);
	void			ParsePaint(MXmlElement* pElement, MWidget* pWidget);
	void			ParseTabCtrl(MXmlElement* pElement, MWidget* pWidget);
	void			ParseTabPages(MXmlElement* pElement, MWidget* pWidget);
	void			ParseTabPage(MXmlElement* pElement, MWidget* pWidget, char* chTabPageName);
	void			ParseComboBox(MXmlElement* pElement, MWidget* pWidget);
	void			ParseScrollBar(MXmlElement* pElement, MWidget* pWidget);
	void			ParseSpinCtrl(MXmlElement* pElement, MWidget* pWidget);
	void			ParseGroup(MXmlElement* pElement, MWidget* pWidget);
	void			ParseView(MXmlElement* pElement, MWidget* pWidget);
	void			ParseTextView(MXmlElement* pElement, MWidget* pWidget);
	void			ParseTreeView(MXmlElement* pElement, MWidget* pWidget);
	void			ParseLayer(MXmlElement* pElement, MWidget* pWidget);

	//////////////////////////////////////////////////////////////////////////
	// Looks
	virtual void	ParseLook(MXmlElement* pElement);
	bool			ParseLookHeader(MXmlElement* pElement, MLOOKBase& LookHeader);
	void			ParseFrameLook(MXmlElement* pElement);
	void			ParseFrameLookState(MXmlElement* pElement, MBFrameLook* pFrameLook, int stat);
	void			ParseButtonLook(MXmlElement* pElement);
	void			ParseButtonLookState(MXmlElement* pElement, MBButtonLook* pButtonLook, int stat);
	void			ParseEditLook(MXmlElement* pElement);
	void			ParseEditLookState(MXmlElement* pElement, MBEditLook* pEditLook, int stat);
	void			ParseListBoxLook(MXmlElement* pElement);
	void			ParseListBoxLookState(MXmlElement* pElement, MBListBoxLook* pEditLook, int stat);
	void			ParseScrollBarLook(MXmlElement* pElement);
	void			ParseScrollBarLookState(MXmlElement* pElement, MBScrollBarLook* pScrollBarLook, int stat);
	void			ParseSpinCtrlLook(MXmlElement* pElement);
	void			ParseSpinCtrlLookState(MXmlElement* pElement, MBSpinCtrlLook* pSpinCtrlLook, int stat);
	void			ParseTextAreaLook(MXmlElement* pElement);
	void			ParseTextAreaLookState(MXmlElement* pElement, MBTextAreaLook* pEditLook, int stat);
	void			ParseGroupLook(MXmlElement* pElement);
	void			ParseGroupLookState(MXmlElement* pElement, MBGroupLook* pGroupLook, int stat);
	void			ParseTreeViewLook(MXmlElement* pElement);
	void			ParseTreeViewLookState(MXmlElement* pElement, MBTreeViewLook* pTreeViewLook, int stat);
	MBPopupBoxLook*	ParsePopupBoxLook(MXmlElement* pElement);
	void			ParsePopupBoxLookState(MXmlElement* pElement, MBPopupBoxLook* pPopupBoxLook);
	void			ParseTooltipLook(MXmlElement* pElement);

	//////////////////////////////////////////////////////////////////////////
	// 困连 技何沥焊 颇教
	void			ParseLayout(MXmlElement* pElement, MWidget* pWidget);
	void			ParseShape(MXmlElement* pElement, MWidget* pWidget);
	void			ParseAction(MXmlElement* pElement, MWidget* pWidget);
	void			ParseScript(MXmlElement* pElement);
	void			ParseEvent(MXmlElement* pElement, MWidget* pWidget);
	MWidget*		ParseWidgetName(char* szOutName, MXmlElement* pElement, MWidget* pParent);

public:
	MintLoader();
	virtual ~MintLoader();

	bool			LoadFromFile(const char* szFileName);
	bool			LoadFromStream(const char* szStream);
	void			ClearIncludedFileNames();
};


} // namespace mint3
#endif