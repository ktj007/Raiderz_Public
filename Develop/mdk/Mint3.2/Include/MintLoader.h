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
class MLookBase;
class MFont;



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
	virtual MWidget* ParseWidgets(MXmlElement* pElement, MWidget* pParent, const char* szWidgetName);
	void			ParseWidget(MXmlElement* pElement, MWidget* pWidget);
	void			ParseSheet(MXmlElement* pElement, MWidget* pWidget);
	void			ParseButton(MXmlElement* pElement, MWidget* pWidget);
	void			ParseFrame(MXmlElement* pElement, MWidget* pWidget);
	void			ParseLabel(MXmlElement* pElement, MWidget* pWidget);
	void			ParseLabelEx(MXmlElement* pElement, MWidget* pWidget);
	void			ParseEdit(MXmlElement* pElement, MWidget* pWidget);
	void			ParseEditCtrl(MXmlElement* pElement, MWidget* pWidget);
	void			ParsePicture(MXmlElement* pElement, MWidget* pWidget);
	void			ParseAnimation(MXmlElement* pElement, MWidget* pWidget);
	void			ParseListBox(MXmlElement* pElement, MWidget* pWidget);
	void			ParseLinePrinter(MXmlElement* pElement, MWidget* pWidget);
	void			ParseListCtrl(MXmlElement* pElement, MWidget* pWidget);
	void			ParsePanel(MXmlElement* pElement, MWidget* pWidget);
	void			ParsePaint(MXmlElement* pElement, MWidget* pWidget);
	void			ParseTabCtrl(MXmlElement* pElement, MWidget* pWidget);
	void			ParseTabPages(MXmlElement* pElement, MWidget* pWidget);
	void			ParseTabPage(MXmlElement* pElement, MWidget* pWidget);
	void			ParseComboBox(MXmlElement* pElement, MWidget* pWidget);
	void			ParseScrollBar(MXmlElement* pElement, MWidget* pWidget);
	void			ParseSpinCtrl(MXmlElement* pElement, MWidget* pWidget);
	void			ParseGroup(MXmlElement* pElement, MWidget* pWidget);
	void			ParseTextView(MXmlElement* pElement, MWidget* pWidget);
	void			ParseTreeCtrl(MXmlElement* pElement, MWidget* pWidget);
	void			ParseCategoryCtrl(MXmlElement* pElement, MWidget* pWidget);
	void			ParseLayer(MXmlElement* pElement, MWidget* pWidget);
	void			ParseViewCtrl(MXmlElement* pElement, MWidget* pWidget);
	void			ParseSlideShow(MXmlElement* pElement, MWidget* pWidget);
	void			ParseTextFrame(MXmlElement* pElement, MWidget* pWidget);
	void			ParseProgressCtrl(MXmlElement* pElement, MWidget* pWidget);
	void			ParseSelector(MXmlElement* pElement, MWidget* pWidget);
	void			ParsePopupMenu(MXmlElement* pElement, MWidget* pWidget);

	//////////////////////////////////////////////////////////////////////////
	// Looks
	virtual void	ParseLook( MXmlElement* pElement);
	bool			ParseLookDefault( MXmlElement* pElement, MLookBase* pLook, const char* szFixedName =NULL);
	void			ParseLookParts( MXmlElement* pElement, MLookBase* pLook);
	void			ParseLookStates( MXmlElement* pElement, MLookBase* pLook, const string& part);
	void			ParseLookCommon( MXmlElement* pElement);
	void			ParseLookLabel( MXmlElement* pElement);
	void			ParseLookFrame( MXmlElement* pElement);
	void			ParseLookButton( MXmlElement* pElement);
	void			ParseLookEdit( MXmlElement* pElement);
	void			ParseLookEditCtrl( MXmlElement* pElement);
	void			ParseLookListBox( MXmlElement* pElement);
	void			ParseLookListCtrl( MXmlElement* pElement);
	void			ParseLookScrollBar( MXmlElement* pElement);
	void			ParseLookSpinCtrl( MXmlElement* pElement);
	void			ParseLookGroup( MXmlElement* pElement);
	void			ParseLookTextView( MXmlElement* pElement);
	void			ParseLookProgressCtrl( MXmlElement* pElement);
	void			ParseLookTreeCtrl( MXmlElement* pElement);
	void			ParseLookCategoryCtrl( MXmlElement* pElement);
	void			ParseLookSelector( MXmlElement* pElement);
	void			ParseLookToolTip( MXmlElement* pElement);
	void			ParseLookPopupMenu( MXmlElement* pElement);

	//////////////////////////////////////////////////////////////////////////
	// 困连 技何沥焊 颇教
	void			ParseLayout(MXmlElement* pElement, MWidget* pWidget);
	void			ParseShape(MXmlElement* pElement, MWidget* pWidget);
	void			ParseAction(MXmlElement* pElement, MWidget* pWidget);
	void			ParseScript(MXmlElement* pElement);
	void			ParseEvent(MXmlElement* pElement, MWidget* pWidget);
	MWidget*		ParseWidgetName(char* szOutName, MXmlElement* pElement, MWidget* pParent);

	//////////////////////////////////////////////////////////////////////////
	// 积己
	virtual MFont*	CreateNewFont( const char* szName, const char* szFontName, const char* szFileName, int nHeight, bool bBold =false, bool bItalic =false, bool bAntiAlias =false, float fOutline =0.0f, float fScale =1.0f);



public:
	MintLoader();
	virtual ~MintLoader();

	bool			LoadFromFile(const char* szFileName);
	bool			LoadFromStream(const char* szStream);
	void			ClearIncludedFileNames();
};


} // namespace mint3
#endif