#pragma once

#include "MWidget.h"
#include "MLookBase.h"


namespace mint3
{

class MButton;
class MButtonGroup;


/// 버튼 타입
enum MButtonType
{
	MBT_NORMAL = 0,		///< 일반 버튼
	MBT_CHECKBTN,		///< 체크 버튼
	MBT_PUSHBTN,		///< 푸쉬 버튼
};






/// class : MButtonLook
class MButtonLook : public MLookBase
{
/// Member variables
protected:
	MPOINT				m_ptTextOffset;				///< 눌린 버튼의 텍스트가 이동하는 오프셋 값을 저장하는 변수
	bool				m_bShowHotKey;				///< 단축키 표시



/// Member functions
public:
	MButtonLook();

	void SetTextOffset( int x, int y)				{ m_ptTextOffset = MPOINT( x, y);	}
	MPOINT GetTextOffset() const					{ return m_ptTextOffset;			}

	void ShowHotKey( bool bShow)					{ m_bShowHotKey = bShow;			}
	bool IsShowHotKey() const						{ return m_bShowHotKey;				}

	virtual void OnDrawBorder( MWidget* pWidget, MDrawContext* pDC) override;
	virtual void OnDraw( MWidget* pWidget, MDrawContext* pDC) override;


protected:
	virtual void OnDrawText( MButton* pButton, MRECT& r, MDrawContext* pDC);
	virtual void OnDrawSelectbar( MButton* pButton, MRECT& r, MDrawContext* pDC);
};









/// class : MButton
class MButton : public MWidget
{
/// Member variables
protected:
	MButtonType			m_Type;					///< Type
	bool				m_bChecked;				///< MBT_CHECKBTN인 경우
	unsigned int		m_nHotKey;				///< HotKey

	MBitmap*			m_pIcon;				///< Icon image
	MSIZE				m_IconSize;				///< Icon size
	
	MButtonGroup		*m_pButtonGroup;		///< 속해있는 그룹
	int					m_nIndexInGroup;		///< 같은 그룹내에서의 index

	bool				m_bButtonDown;			///< Button down

	DWORD				m_dwUserData;			///< User data

	bool				m_bBlanking;			///< Blanking

	bool				m_bEnableEnter;			///< 포커스 있을때 Enter키로 버튼 누르기 활성화 ( Default : true )



// Member functions
public:
	MButton( const char* szName =NULL, MWidget* pParent =NULL, MListener* pListener =NULL);
	virtual ~MButton();

	// 상태 구함
	virtual const char* GetState() override;
	virtual int GetStateNum() override;
	
	/// 버튼 타입 지정
	void SetType( MButtonType nType)					{ m_Type = nType;					}
	MButtonType GetType() const							{ return m_Type;					}

	/// Push 버튼인 경우 체크
	void SetCheck( bool bCheck);
	bool GetCheck() const								{ return m_bChecked;				}

	/// HotKey 설정
	void SetHotKey( unsigned int nHotKey)				{ m_nHotKey = nHotKey;				}
	unsigned int GetHotKey() const						{ return m_nHotKey;					}

	/// Icon 설정
	void SetIcon( MBitmap* pIcon, MSIZE& size);
	void SetIcon( MBitmap* pIcon)						{ m_pIcon = pIcon;					}
	MBitmap* GetIcon() const							{ return m_pIcon;					}
	void SetIconSize( MSIZE& size)						{ m_IconSize = size;				}
	MSIZE GetIconSize() const							{ return m_IconSize;				}
	
	/// 버튼이 눌려져 있는가? ( 마우스로 누르는 상태를 얻어낼 수 있다. )
	bool IsButtonDown()									{ return ((m_bButtonDown==true && IsMouseOver()) ? true : false);	}

	// Button group
	void SetButtonGroup( MButtonGroup *pGroup);
	int GetIndexInGroup()								{ return m_nIndexInGroup;			}
	void ReleaseButtonGroup();

	// Set user data
	void SetUserData( DWORD dwUserData)					{ m_dwUserData = dwUserData;		}
	DWORD GetUserData()									{ return m_dwUserData;				}

	// Set blanking
	void SetBlanking( bool bBlanking)					{ m_bBlanking = bBlanking;			}



// Event handler
protected:
	virtual bool OnEvent( MEvent* pEvent, MListener* pListener) override;
	virtual bool OnQueryHotKey( unsigned int nHotKey) override;
	virtual void OnHotKey( unsigned int nHotKey) override;
	virtual bool OnCommand( MWidget* pWidget, MEventMsg nMsg, const char* szArgs =NULL) override;
	virtual bool OnSetCursor( MEvent* pEvent, string* pCursor) override;



// Lua glue functions
public:
	void				glueSetCheck( bool bCheck)					{ SetCheck( bCheck);					}
	bool				glueGetCheck()								{ return GetCheck();					}
	void				glueSetHotKey( int nHotKey)					{ SetHotKey( nHotKey);					}
	int					glueGetHotKey()								{ return GetHotKey();					}
	void				glueSetIcon( const char* szIcon);
	const char*			glueGetIcon();
	void				glueSetIconSize( int w, int h)				{ SetIconSize( MSIZE( w, h));			}
	MWLua::ret_int2		glueGetIconSize() const						{ return MWLua::ret_int2( m_IconSize.w, m_IconSize.h);	}
	void				glueSetUserData( DWORD dwUserData)			{ SetUserData( dwUserData);				}
	DWORD				glueGetUserData()							{ return m_dwUserData;					}



#define MINT_BUTTON	"Button"
	virtual const char* GetClassName() override			{ return MINT_BUTTON;				}
};



} // namespace mint3
