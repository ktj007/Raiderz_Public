#pragma once

#include "MWidget.h"
#include "MScrollBar.h"
#include "MTextView.h"
#include "MEdit.h"
#include "MPanel.h"
#include "MButton.h"
#include "MPopupMenu.h"



using namespace std;
using namespace mint3;


class XChatTab;
class XChatView;
class XChatCtrl;
class XChatBox;


enum CHATTEXTSIZE { CTS_SMALL = 0, CTS_NORMAL, CTS_LARGE, CTS_MAX };




/// class : XChatButton
class XChatButton : public MButton
{
/// Member variables
protected:


/// Member functions
public:
	XChatButton( const char* szName =NULL, MWidget* pParent =NULL, MListener* pListener =NULL);

	virtual bool GetAlwaysTop() const override		{ return true;				}
};




/// class : XChatScrollBar
class XChatScrollBar : public MScrollBar
{
/// Member variables
protected:

	
/// Member functions
public:
	XChatScrollBar( const char* szName =NULL, MWidget* pParent =NULL, MListener* pListener =NULL);

	virtual void CalculateAlignRect() override;
};








/// class : XChatTab
class XChatTab : public MButton
{
/// Member variables
protected:


/// Member functions
public:
	XChatTab( const char* szName =NULL, MWidget* pParent =NULL, MListener* pListener =NULL);
	virtual ~XChatTab();

	virtual bool IsEnableDragAndDrop() const override		{ return true;								}



/// Event handler
protected:
	virtual void OnDrawBorder( MDrawContext* pDC) override;
	virtual void OnDraw( MDrawContext* pDC) override;
	virtual void OnDblClick( MEvent* pEvent) override;
	virtual bool OnDragBegin( MDragData* pDragData, MPOINT& pt) override;
	virtual bool OnDragEnd( MDragData* pDragData, MPOINT& pt, bool bDropped) override;
	virtual bool OnDrop( MWidget* pSender, MDragData* pDragData, MPOINT& pt) override;



#define MINT_CHATTINGBOXTAB		"ChattingBoxTab"
	virtual const char* GetClassName() override				{ return MINT_CHATTINGBOXTAB;				}
};








/// class : XChatView
class XChatView : public MWidget
{
public:
	struct _CHILD
	{
		MTextView*	m_pTextView;

		_CHILD( MTextView* pTextView)		{ m_pTextView = pTextView;		}
		void _delete()						{ delete m_pTextView;			}
	};


/// Member variables
protected:
	list< _CHILD>		m_Children;
	XChatScrollBar*		m_pScrollBar;
	MButton*			m_pEndLine;
	int					m_nTextSize;
	MTimer				m_tMove;
	bool				m_bUpdateScrollBar;
	bool				m_bArrangeChildren;



// Member functions
public:
	XChatView( const char* szName =NULL, MWidget* pParent =NULL, MListener* pListener =NULL);
	virtual ~XChatView();

	bool AddChat( const char* szString);
	void ClearChat();

	void ChangeTextLook( MLookBase* pLook);

	virtual MScrollBar* GetScrollBar() override				{ return m_pScrollBar;						}
	void ScrollPage( int delta);

	void SetTextSize( int nSize);
	int GetTextSize() const									{ return m_nTextSize;						}

	MButton* GetEndLineBtn()								{ return m_pEndLine;						}


/// Event handler
protected:
	bool OnCommand( MWidget* pWidget, MEventMsg nMsg, const char* szArgs) override;
	virtual bool OnEvent( MEvent* pEvent, MListener* pListener) override;
	virtual void OnRun( DWORD dwCurrTime) override;
	virtual void OnSize( int w, int h) override;
	virtual void OnScrollBarChanged( int nPos) override;
};










/// class : XChatCtrl
class XChatCtrl
{
/// Member variables
protected:
	XChatTab*			m_pChatTab;
	XChatView*			m_pChatView;
	vector< int>		m_TypeFilter;



/// Member functions
public:
	XChatCtrl( MWidget* pParent);
	virtual ~XChatCtrl();

	void SetID( int nID)									{ m_pChatTab->SetUserData( nID);			}
	int GetID()												{ return m_pChatTab->GetUserData();			}

	XChatBox* GetChatBox();

	bool Delete();

	XChatTab* GetTab()										{ return m_pChatTab;						}
	XChatView* GetView()									{ return m_pChatView;						}

	void AddChat( int nType, const char* szChat);
	void ClearChat()										{ m_pChatView->ClearChat();					}

	void AddTypeFilter( int nType)							{ m_TypeFilter.push_back( nType);			}
	void ClearTypeFilter()									{ m_TypeFilter.clear();						}
	size_t GetTypeFilterCount()								{ return m_TypeFilter.size();				}
	int GetTypeFilter( int i)								{ return m_TypeFilter[ i];					}

	void SetTextSize( int nSize)							{ m_pChatView->SetTextSize( nSize);			}
	int GetTextSize() const									{ return m_pChatView->GetTextSize();		}

	void Show( bool bShow);
	bool IsShow()											{ return m_pChatTab->GetCheck();			}
};







/// class : XChatBox
class XChatBox : public MWidget
{
/// Member variables
protected:
	int					m_nID;							///< ID
	list< XChatCtrl*>	m_ChatCtrls;					///< Controls
	XChatCtrl*			m_pCurSel;						///< Current selected
	XChatBox*			m_pChainedParent;				///< 체인 연결 방식
	XChatBox*			m_pChainedChild;				///< 체인 연결 방식
	XChatButton*		m_pAddChatCtrl;					///< Button to add chat control
	XChatButton*		m_pRemoveChatCtrl;				///< Button to remove chat control
	XChatButton*		m_pShowProperty;				///< Button to show property dialog
	MWidget*			m_pFiltered;					///< Event hooker
	MTimer				m_tFade;						///< Fade in/out
	bool				m_bMoving;						///< Is moving widget
	int					m_nResizing;					///< Is resizing widget
	MPOINT				m_ptLastCursor;					///< Last cursor position
	bool				m_bArrangeChatCtrlTab;



/// Member functions
public:
	XChatBox( const char* szName =NULL, MWidget* pParent =NULL, MListener* pListener =NULL);
	virtual ~XChatBox();

	virtual bool IsEnableDragAndDrop() const override		{ return true;								}
	virtual int GetDrawState() override;

	void SetID( int nID)									{ m_nID = nID;								}
	int GetID()												{ return m_nID;								}

	bool IsEmpty()											{ return m_ChatCtrls.empty();				}

	double GetFadeRatio()									{ return m_tFade.GetRatio();				}

	void SetChainedParent( XChatBox* pParent)				{ m_pChainedParent = pParent;				}
	XChatBox* GetChainedParent()							{ return m_pChainedParent;					}

	void SetChainedChild( XChatBox* pChild)					{ m_pChainedChild = pChild;					}
	XChatBox* GetChainedChild()								{ return m_pChainedChild;					}

	XChatCtrl* GetChatCtrl( int nID);
	XChatCtrl* GetChatCtrlIndex( int nIndex);
	size_t GetChatCtrlCountInBox()							{ return m_ChatCtrls.size();				}

	XChatBox* CreateNewChatBox( int nID, MRECT& r);
	XChatCtrl* CreateNewChatCtrl( int nChatBoxID, int nChatCtrlID, const char* szName, MBitmap* pIcon);
	bool DeleteChatCtrl( int nChatCtrlID);

	void Delete();

	bool AttachChatCtrl( XChatCtrl* pChatCtrl, XChatCtrl* pInsert =NULL);
	bool DetachChatCtrl( XChatCtrl* pChatCtrl);

	void SetChatTabName( int nChatCtrlID, const char* szName);
	const char* GetChatTabName( int nChatCtrlID);

	void AddChat( int nType, const char* szChat);
	void ClearChat();

	void SetTabCurSel( int nChatBoxID, int nChatCtrlID);
	int GetTabCurSel( int nChatBoxID);

	void AddTypeFilter( int nChatCtrlID, int nType);
	void ClearTypeFilter( int nChatCtrlID);
	int GetTypeFilterCount( int nChatCtrlID);
	int GetTypeFilter( int nChatCtrlID, int i);

	void SetTextSize( int nChatCtrlID, int nSize);
	int GetTextSize( int nChatCtrlID);

	void SelChangedChatCtrlTab( XChatCtrl* pChatCtrl);

	virtual bool IsMovable() override;
	virtual bool IsResizable() override;

	XChatBox* FindChatBox( int nID);
	XChatCtrl* FindChatCtrl( int nID);


protected:
	void EventRefresh();
	virtual void InitMouseState() override;


/// Event handler
protected:
	virtual bool OnEvent( MEvent* pEvent, MListener* pListener) override;
	virtual bool OnCommand( MWidget* pWidget, MEventMsg nMsg, const char* szArgs = NULL) override;
	virtual void OnRun( DWORD dwCurrTime) override;
	virtual void OnDrawBorder( MDrawContext* pDC) override;
	virtual void OnDraw( MDrawContext* pDC) override;
	virtual void OnSetFocus() override;
	virtual void OnSize( int w, int h) override;
	virtual void OnRClick( MEvent* pEvent) override;
	virtual bool OnDrop( MWidget* pSender, MDragData* pDragData, MPOINT& pt) override;
	virtual int OnNcHitTest( MEvent* pEvent) override;
};






/// class XUIChattingEdit
class XUIChattingEdit : public MEdit
{
protected:
	int					m_nPrefix;
	string				m_strPrefixName;
	MPopupMenu*			m_pPrefixSelector;

public:
	XUIChattingEdit( const char* szName =NULL, MWidget* pParent =NULL, MListener* pListener =NULL);
	virtual ~XUIChattingEdit();

	bool IsActive() const;

	void SetPrefix( int nPrefix, const char* szPreFixName)		{ m_nPrefix = nPrefix;		m_strPrefixName = szPreFixName;		}
	int GetPrefix() const										{ return m_nPrefix;												}
	const char* GetPrefixName() const							{ return m_strPrefixName.c_str();								}
	void SetPrefixSelector( MPopupMenu* pSelector);


protected:
	virtual bool OnCommand( MWidget* pWidget, MEventMsg nMsg, const char* szArgs =NULL) override;
	virtual bool OnEvent( MEvent* pEvent, MListener* pListener) override;
	virtual void OnDrawBorder( MDrawContext* pDC) override;
	virtual void OnShow( bool bShow) override;
};






/// class : XUIChattingBox
class XUIChattingBox : public XChatBox
{
/// Member variables
public:
	static XUIChattingBox*	m_pInstance;

protected:
	XUIChattingEdit*	m_pEdit;
	MLookBase*			m_pLookTabBtn;
	MLookBase*			m_pLookAddBtn;
	MLookBase*			m_pLookRemoveBtn;
	MLookBase*			m_pLookPropBtn;
	MLookBase*			m_pLookText[ CTS_MAX];
	MLookBase*			m_pLookScrollBar;
	MLookBase*			m_pLookEndLineBtn;
	int					m_nHeaderHeight;
	int					m_nThumbLength;
	MSize				m_AdderSize;
	MSize				m_RemoverSize;
	MSize				m_PropertySize;
	int					m_nForcedShowState;
	bool				m_bFixed;
	MBitmap*			m_pIcon;
	MRECT				m_IconRect;
	DWORD				m_dwLazySendTimer;



/// Member functions
public:
	XUIChattingBox( const char* szName =NULL, MWidget* pParent =NULL, MListener* pListener =NULL);
	virtual ~XUIChattingBox();

	void SetEditLook( MLookBase* pLook)						{ m_pEdit->SetLookAndFill( pLook);			}
	void SetTabBtnLook( MLookBase* pLook)					{ m_pLookTabBtn = pLook;					}
	void SetAddBtnLook( MLookBase* pLook)					{ m_pLookAddBtn = pLook;					}
	void SetRemoveBtnLook( MLookBase* pLook)				{ m_pLookRemoveBtn = pLook;					}
	void SetPropBtnLook( MLookBase* pLook)					{ m_pLookPropBtn = pLook;					}
	void SetTextLook( int nSize, MLookBase* pLook)			{ m_pLookText[ nSize] = pLook;				}
	void SetScrollBarLook( MLookBase* pLook)				{ m_pLookScrollBar = pLook;					}
	void SetEndLineBtnLook( MLookBase* pLook)				{ m_pLookEndLineBtn = pLook;				}

	MLookBase* GetTabBtnLook()								{ return m_pLookTabBtn;						}
	MLookBase* GetAddBtnLook()								{ return m_pLookAddBtn;						}
	MLookBase* GetRemoveBtnLook()							{ return m_pLookRemoveBtn;					}
	MLookBase* GetPropBtnLook()								{ return m_pLookPropBtn;					}
	MLookBase* GetTextLook( int nSize)						{ return m_pLookText[ nSize];				}
	MLookBase* GetScrollBarLook()							{ return m_pLookScrollBar;					}
	MLookBase* GetEndLineBtnLook()							{ return m_pLookEndLineBtn;					}

	void SetPrefix( int nPrefix, const char* szPrefixName)	{ m_pEdit->SetPrefix( nPrefix, szPrefixName);	}
	int GetPrefix() const									{ return m_pEdit->GetPrefix();					}
	const char* GetPrefixName() const						{ return m_pEdit->GetPrefixName();				}
	void SetPrefixSelector( const char* szPopupMenu);

	void SetHeaderHeight( int nHeight)						{ m_nHeaderHeight = nHeight;				}
	int GetHeaderHeight()									{ return m_nHeaderHeight;					}

	void SetThumbLength( int nLength)						{ m_nThumbLength = nLength;					}
	int GetThumbLength()									{ return m_nThumbLength;					}

	void SetAdderSize( MSize& _size)						{ m_AdderSize = _size;						}
	MSize GetAdderSize()									{ return m_AdderSize;						}

	void SetRemoverSize( MSize& _size)						{ m_RemoverSize = _size;					}
	MSize GetRemoverSize()									{ return m_RemoverSize;						}

	void SetPropertySize( MSize& _size)						{ m_PropertySize = _size;					}
	MSize GetPropertySize()									{ return m_PropertySize;					}

	int GetChatBoxCount();
	int GetChatBoxID( int nIndex);
	MRECT GetChatBoxRect( int nIndex);

	int GetChatCtrlCount( int nChatBoxID);
	int GetChatCtrlID( int nChatBoxID, int nIndex);
	const char* GetChatCtrlName( int nChatBoxID, int nIndex);

	void ActivateChatInput( bool bActivate);
	bool IsChatInputActivated();

	int GetNewChatBoxID();
	int GetNewChatCtrlID();

	void SetForcedShowState( int nShow)						{ m_nForcedShowState = nShow;				}
	int GetForcedShowState()								{ return m_nForcedShowState;				}

	void SetIcon( MBitmap* pIcon, MRECT& r)					{ m_pIcon = pIcon;		m_IconRect = r;		}
	void GetIcon( MBitmap** pIcon, MRECT* r)				{ *pIcon = m_pIcon;		*r = m_IconRect;	}

	void SetFixed( bool bFixed);
	bool IsFixed()											{ return m_bFixed;							}

	static XUIChattingBox* GetInstance();



/// Event caller
public:
	bool EventClick( int nChatCtrlID, int nClickItem);
	bool EventRClick( int nChatCtrlID, int nClickItem);



/// Event handler
protected:
	virtual bool OnCommand( MWidget* pWidget, MEventMsg nMsg, const char* szArgs =NULL) override;
	virtual void OnRun( DWORD dwCurrTime) override;
	virtual void OnLoaded() override;
	virtual void OnDrawBorder( MDrawContext* pDC) override;



/// LUA script
public:
	void glueCreateChatBox( int nBoxID, int x, int y, int w, int h)		{ CreateNewChatBox( nBoxID, MRECT( x, y, w, h));	}
	int glueGetChatBoxCount()											{ return GetChatBoxCount();							}
	int glueGetChatBoxID( int i)										{ return GetChatBoxID( i);							}
	MWLua::ret_int4 glueGetChatBoxRect( int i);
	void glueCreateChatTab( int nBoxID, int nTabID, const char* szName, const char* szIcon);
	void glueDeleteChatTab( int nTabID)									{ DeleteChatCtrl( nTabID);							}
	void glueSetChatTabName( int nTabID, const char* szName)			{ SetChatTabName( nTabID, szName);					}
	const char* glueGetChatTabName( int nTabID)							{ return GetChatTabName( nTabID);					}
	int glueGetChatBoxTabCount( int nBoxID)								{ return GetChatCtrlCount( nBoxID);					}
	int glueGetChatBoxTabID( int nBoxID, int i)							{ return GetChatCtrlID( nBoxID, i);					}
	void glueAddChat( int nType, const char* szChat)					{ AddChat( nType, szChat);							}
	void glueClearChat()												{ ClearChat();										}
	void glueSetTabCurSel( int nBoxID, int nTabID)						{ SetTabCurSel( nBoxID, nTabID);					}
	int glueGetTabCurSel( int nBoxID)									{ return GetTabCurSel( nBoxID);						}
	void glueAddTypeFilter( int nTabID, int nType)						{ AddTypeFilter( nTabID, nType);					}
	void glueClearTypeFilter( int nTabID)								{ ClearTypeFilter( nTabID);							}
	int glueGetTypeFilterCount( int nTabID)								{ return GetTypeFilterCount( nTabID);				}
	int glueGetTypeFilter( int nTabID, int i)							{ return GetTypeFilter( nTabID, i);					}
	void glueSetTextSize( int nTabID, int nSize)						{ SetTextSize( nTabID, nSize);						}
	int glueGetTextSize( int nTabID)									{ return GetTextSize( nTabID);						}
	void glueActivateChatInput( bool bActivate)							{ ActivateChatInput( bActivate);					}
	bool glueIsChatInputActivated()										{ return IsChatInputActivated();					}
	void glueSetFixed( bool bFixed)										{ SetFixed( bFixed);								}
	bool glueIsFixed()													{ return IsFixed();									}
	void glueSetEditText(const char* szText);
	void glueSetPrefix( int nPrefix, const char* szPrefixName)			{ SetPrefix( nPrefix, szPrefixName);				}
	int glueGetPrefix()													{ return GetPrefix();								}
	const char* glueGetPrefixName()										{ return GetPrefixName();							}


#define MINT_CHATTINGBOX		"ChattingBox"
	virtual const char* GetClassName() override				{ return MINT_CHATTINGBOX;					}
};
