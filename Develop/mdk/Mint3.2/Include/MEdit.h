#pragma once

#include "MWidget.h"
#include "MLookBase.h"
#include <vector>
using namespace std;



namespace mint3
{


#define MEDITBLINK_TIME				500

class MEdit;
class MSpinCtrl;



/// class : MEditLook
class MEditLook : public MLookBase
{
/// Member functions
public:
	MEditLook();

	virtual void OnDraw( MWidget* pWidget, MDrawContext* pDC) override;


protected:
	virtual void OnDrawText( MEdit* pEdit, MDrawContext* pDC);
	virtual void OnDrawCaret( MEdit* pEdit, MRECT& r, MDrawContext* pDC);
	virtual void OnDrawIMECaret( MEdit* pEdit, MRECT& r, MDrawContext* pDC);
	virtual void OnDrawSelectbar( MEdit* pEdit, MRECT& r, MDrawContext* pDC);
};









/// class : Edit
class MEdit : public MWidget
{
friend MSpinCtrl;


/// Member variables
protected:
	wstring		m_strString;			///< wchar_t형 스트링
	size_t		m_nCaret;				///< 캐럿 위치
	size_t		m_nStartPos;			///< 표시 시작 위치
	size_t		m_nStartSel;			///< 선택 영역 시작
	size_t		m_nEndSel;				///< 선택 영역 끝
	bool		m_bEditable;			///< 에디트 가능 여부
	bool		m_bNumber;				///< 숫자만 입력 받을 수 있는지 여부
	bool		m_bPassword;			///< 패스워드 형태인지 여부
	bool		m_bLowerCase;			///< 소문자만 입력 가능한지 여부
	bool		m_bUpperCase;			///< 대문자만 입력 가능한지 여부
	size_t		m_nLimitText;			///< 글자수 제한
	bool		m_bSelectAllOnFocus;	///< 포커스를 얻을 때 전체 선택 여부
	bool		m_bEnsureCaretVisible;	///< Update때 caret 위치를 확보할지 여부

	bool		m_bEnableHistory;		///< 히스토리 사용 여부
	vector< string>		m_History;		///< 문자열 히스토리
	size_t				m_nHistoryIndex;



/// Member functions
public:
	MEdit( const char* szName =NULL, MWidget* pParent =NULL, MListener* pListener =NULL);
	virtual ~MEdit();

	virtual void SetText( const char* szText) override;
	virtual const char* GetText() override;

	size_t GetLength() const						{ return m_strString.length();		}
	wchar_t GetWChar( size_t index) const			{ return m_strString.at( index);	}

	void SetCaretPos( size_t pos)					{ m_nCaret = pos;					}
	size_t GetCaretPos() const						{ return m_nCaret;					}

	void SetStartPos( size_t pos)					{ m_nStartPos = pos;				}
	size_t GetStartPos() const						{ return m_nStartPos;				}

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

	void SetNumber( bool bNumber)					{ m_bNumber = bNumber;				}
	bool IsNumber() const							{ return m_bNumber;					}

	void SetLowerCase( bool bLower)					{ m_bLowerCase = bLower;			}
	bool IsLowerCase() const						{ return m_bLowerCase;				}

	void SetUpperCase( bool bUpper)					{ m_bUpperCase = bUpper;			}
	bool IsUpperCase() const						{ return m_bUpperCase;				}

	void SetPassword( bool bPassword)				{ m_bPassword = bPassword;			}
	bool IsPassword() const							{ return m_bPassword;				}

	void SetPasswordChar( const char* szChar);
	const string& GetPasswordChar() const;

	void SetLimitText( int nMax)					{ m_nLimitText = nMax;				}
	int GetLimitText() const						{ return m_nLimitText;				}

	void EnableHistory( bool bEnable)				{ m_bEnableHistory = bEnable;		}
	bool IsEnableHistory() const					{ return m_bEnableHistory;			}

	void PushHistory();



protected:
	int CaretFromPoint( const MPOINT& pt, bool bFindStartCaret);
	void EnsureCaretVisible()						{ m_bEnsureCaretVisible = true;		}



public:
	void EventValueChanged();


/// Event handler
protected:
	virtual bool OnEvent( MEvent* pEvent, MListener* pListener) override;
	virtual void OnRun( DWORD dwCurrTime) override;
	virtual void OnSetFocus() override;
	virtual void OnKillFocus() override;
	virtual void OnSize( int w, int h) override;
	virtual bool OnSetCursor( MEvent* pEvent, string* pCursor) override;




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
	void			glueSetNumber( bool bNumber)					{ SetNumber( bNumber);				}
	void			glueSetLowerCase( bool bLower)					{ SetLowerCase( bLower);			}
	void			glueSetUpperCase( bool bUpper)					{ SetUpperCase( bUpper);			}
	void			glueSetPassword( bool bPassword)				{ SetPassword( bPassword);			}
	void			glueSetPasswordChar( const char* szChar)		{ SetPasswordChar( szChar);			}
	void			glueSetLimitText( int nMax)						{ SetLimitText( nMax);				}
	int				glueGetLimitText()								{ return GetLimitText();			}
	void			gluePushHistory()								{ PushHistory();					}



#define MINT_EDIT	"Edit"
	virtual const char* GetClassName() override		{ return MINT_EDIT;					}
};


} // namespace mint3

