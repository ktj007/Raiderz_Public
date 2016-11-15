#pragma once

#include "MButton.h"
#include "MPopupMenu.h"
#include "MListBox.h"


namespace mint3
{

class MComboListBox;
class MComboBox;



/// class : MComboListBox
class MComboListBox : public MListBox
{
/// Member variables
protected:
	MComboBox*		m_pComboBox;
	int				m_nLastSel;
	MTimer			m_tFade;



/// Member functions
public:
	MComboListBox( const char* szName, MWidget* pParent=NULL, MListener* pListener=NULL);
	virtual ~MComboListBox();

protected:
	virtual int GetDrawState() override;



/// Event handler
protected:
	virtual void OnPreDraw( MDrawContext* pDC) override;
	virtual void OnShow( bool bShow) override;
	virtual void OnRun( DWORD dwCurrTime) override;
	virtual void OnSelChanged() override;
};






/// class : MComboBox
class MComboBox : public MButton
{
friend MComboListBox;


/// Member variables
protected:
	MComboListBox*		m_pListBox;				///< List box
	int					m_nDroppedWidth;		///< Drop size



/// Member functions
public:
	MComboBox( const char* szName=NULL, MWidget* pParent=NULL, MListener* pListener=NULL);
	virtual ~MComboBox();

	int AddString( const char* szString)					{ return GetListBox()->AddString( szString);			}
	int DeleteString( size_t nIndex);
	int InsertString( size_t nIndex, const char* szString);
	void ResetContent();
	size_t GetItemCount()									{ return GetListBox()->GetItemCount();					}

	void SetItemData( size_t nIndex, DWORD dwItemData)		{ GetListBox()->SetItemData( nIndex, dwItemData);		}
	DWORD GetItemData( size_t nIndex)						{ return GetListBox()->GetItemData( nIndex);			}

	int SetCurSel( int nIndex);
	int GetCurSel()											{ return GetListBox()->GetCurSel();						}

	void SetDroppedWidth( int width)						{ m_nDroppedWidth = max( width, 0);						}
	int GetDroppedWidth() const								{ return m_nDroppedWidth;								}

	void ShowDropDown( bool bShow =true);

	MListBox* GetListBox()									{ return m_pListBox;									}
	virtual MScrollBar* GetScrollBar() override				{ return m_pListBox->GetScrollBar();					}



/// Event handler
protected:
	virtual bool OnEvent( MEvent* pEvent, MListener* pListener) override;



/// LUA script
public:
	int			glueAddString( const char* szString)					{ return AddString( szString);				}
	int			glueDeleteString( int nIndex)							{ return DeleteString( nIndex);				}
	int			glueInsertString( int nIndex, const char* szString)		{ return InsertString( nIndex, szString);	}
	void		glueResetContent()										{ ResetContent();							}
	int			glueGetItemCount()										{ return (int)GetItemCount();				}
	void		glueSetItemText( int nIndex, const char* szText)		{ m_pListBox->SetItemText( nIndex, szText);	}
	const char*	glueGetItemText( int nIndex)							{ return m_pListBox->GetItemText( nIndex);	}
	void		glueSetItemData( int nIndex, DWORD dwItemData)			{ SetItemData( nIndex, dwItemData);			}
	DWORD		glueGetItemData( int nIndex)							{ return GetItemData( nIndex);				}
	void		glueSetCurSel( int nIndex)								{ SetCurSel( nIndex);						}
	int			glueGetCurSel()											{ return GetCurSel();						}
	void		glueSetDroppedWidth( int width)							{ SetDroppedWidth( width);					}




#define MINT_COMBOBOX	"ComboBox"
	virtual const char* GetClassName() override				{ return MINT_COMBOBOX;									}
};

} // namespace mint3