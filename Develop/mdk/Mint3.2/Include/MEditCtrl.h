#pragma once

#include "MWidget.h"
#include "MLookBase.h"
#include <vector>
using namespace std;



namespace mint3
{


#define MEDITCTRLBLINK_TIME				500

class MEditCtrl;
class MScrollBar;




/// class : MEditCtrlLook
class MEditCtrlLook : public MLookBase
{
/// Member functions
public:
	MEditCtrlLook();

	virtual void OnDraw( MWidget* pWidget, MDrawContext* pDC) override;


protected:
	virtual void OnDrawText( MEditCtrl* pEdit, MDrawContext* pDC);
	virtual void OnDrawCaret( MEditCtrl* pEdit, MRECT& r, MDrawContext* pDC);
	virtual void OnDrawIMECaret( MEditCtrl* pEdit, MRECT& r, MDrawContext* pDC);
	virtual void OnDrawSelectbar( MEditCtrl* pEdit, MRECT& r, MDrawContext* pDC);
};









/// class : MEditCtrl
class MEditCtrl : public MWidget
{
/// Member variables
protected:
	wstring							m_strString;			///< wchar_t형 스트링
	vector< pair< size_t, size_t>>	m_vLineInfo;			///< Line info
	size_t							m_nCaret;				///< 캐럿 위치
	size_t							m_nStartSel;			///< 선택 영역 시작
	size_t							m_nEndSel;				///< 선택 영역 끝
	bool							m_bEditable;			///< 에디트 가능 여부
	MScrollBar*						m_pScrollBar;
	bool							m_bEnsureCaretVisible;
	bool							m_bUpdateScrollBar;



/// Member functions
public:
	MEditCtrl( const char* szName =NULL, MWidget* pParent =NULL, MListener* pListener =NULL);
	virtual ~MEditCtrl();

	virtual void SetText( const char* szText) override;
	virtual const char* GetText() override;

	size_t GetLength() const						{ return m_strString.length();		}

	void SetCaretPos( size_t pos)					{ m_nCaret = pos;					}
	size_t GetCaretPos() const						{ return m_nCaret;					}

	size_t GetCurLine() const;

	void SetSel( size_t nStartSel, size_t nEndSel);
	void GetSel( size_t& nStartSel, size_t& nEndSel);
	bool IsSelected() const							{ return ( m_nStartSel != m_nEndSel) ? true : false;	}
	void ReleaseSelect();

	void Insert( const char* str);
	void Insert( const wstring& str);

	void ReplaceSel( const char* str);
	void ReplaceSel( const wstring& str);

	void Clear();

	void SetEditable( bool bEditable)				{ m_bEditable = bEditable;			}
	bool IsEditable()								{ return m_bEditable;				}

	// Scrollbar
	virtual MScrollBar* GetScrollBar() override		{ return m_pScrollBar;				}
	void ScrollPage( int delta);


protected:
	int CaretFromPoint( const MPOINT& pt, bool bFindStartCaret);
	void UpdateLineInfo();
	void EnsureCaretVisible()						{ m_bEnsureCaretVisible = true;		}
	void UpdateScrollBar()							{ m_bUpdateScrollBar = true;		}


protected:
	struct LINEINFO { wstring str;	int st, ed; };
	enum LINEPOS { LINE_PREV = 0, LINE_CUR = 1, LINE_NEXT =2 };
	void _get_line_info( LINEINFO* pLineInfo);



/// Event handler
protected:
	virtual bool OnEvent( MEvent* pEvent, MListener* pListener) override;
	virtual void OnRun( DWORD dwCurrTime) override;
	virtual void OnSetFocus() override;
	virtual void OnKillFocus() override;
	virtual void OnSize( int w, int h) override;




/// LUA script
public:
	void			glueSetText( const char* szText)				{ SetText( szText);					}
	const char*		glueGetText()									{ return GetText();					}
	int				glueGetLength() const							{ return (int)GetLength();			}
	void			glueSetCaretPos( int pos)						{ SetCaretPos( pos);				}
	int				glueGetCaretPos() const							{ return (int)GetCaretPos();		}
	void			glueSetSel( int nStartSel, int nEndSel)			{ SetSel( nStartSel, nEndSel);		}
	MWLua::ret_int2	glueGetSel();
	bool			glueIsSelected() const							{ return IsSelected();				}
	void			glueReleaseSelect()								{ ReleaseSelect();					}
	void			glueInsert( const char* str)					{ Insert( str);						}
	void			glueReplaceSel( const char* str)				{ ReplaceSel( str);					}
	void			glueClear()										{ Clear();							}
	void			glueSetEditable( bool bEditable)				{ SetEditable( bEditable);			}



#define MINT_EDITCTRL	"EditCtrl"
	virtual const char* GetClassName() override		{ return MINT_EDITCTRL;					}
};


} // namespace mint3

