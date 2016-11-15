#pragma once

#include "MButton.h"
#include "MListBox.h"

namespace mint3 {

/// ComboBox에서 드롭되는 ListBox
class MComboListBox : public MListBox{
protected:
	virtual bool OnEvent(MEvent* pEvent, MListener* pListener);
public:
	MComboListBox(MWidget* pParent=NULL, MListener* pListener=NULL);
	virtual ~MComboListBox(void){}

	DECLARE_LOOK(MListBoxLook)
	DECLARE_LOOK_CLIENT()

#define MINT_COMBOLISTBOX	"ComboListBox"
	virtual const char* GetClassName(void){ return MINT_COMBOLISTBOX; }
};



class MComboBox : public MButton
{
protected:
	// Listener
	MListener*		m_pComboBoxListener;

	// ListBox
	MComboListBox*	m_pListBox;

	// 드롭박스 높이
	int				m_nDropHeight;

	// 페이드 타이머
	DWORD			m_dwFadeTimer;



public:
	MComboBox( const char* szName=NULL, MWidget* pParent=NULL, MListener* pListener=NULL);
	virtual ~MComboBox();

	// Interface functions
	void Add( const char* szItem);
	void Add( MListItem* pItem);
	const char* GetString(int i);
	MListItem* Get(int i);
	void Remove(int i);
	void RemoveAll();
	int GetCount();
	int GetSelIndex();
	bool SetSelIndex(int i);
	const char* GetSelItemString();
	MListItem* GetSelItem();

	void SetNextSel();
	void SetPrevSel();

	void SetDropSize( int nHeight);
	void SetItemHeight( int nItemHeight)						{ m_pListBox->SetItemHeight( nItemHeight);		}

	// Field Support
	void AddField( const char* szFieldName, int nTabSize)		{ m_pListBox->AddField(szFieldName, nTabSize, MLFT_STRING);	}
	void RemoveField( const char* szFieldName)					{ m_pListBox->RemoveField( szFieldName);		}
	MLISTFIELD* GetField( int nField)							{ return m_pListBox->GetField( nField);			}
	int GetFieldCount()											{ return m_pListBox->GetFieldCount();			}

	// Header
	void SetVisibleHeader( bool bVisible)						{ m_pListBox->SetVisibleHeader( bVisible);		}
	bool IsVisibleHeader()										{ return m_pListBox->IsVisibleHeader();			}

	// ListBox
	void SetListBoxLook( MListBoxLook* pLook)					{ m_pListBox->ChangeCustomLook( pLook);			}
	void SetScrollBarLook( MScrollBarLook* pLook)				{ m_pListBox->SetScrollBarLook( pLook);			}
	void SetListBoxAlignment( MAlignmentMode am)				{ m_pListBox->SetTextAlign( am);				}
	void SetScrollBarWidth( int nWidth)						{ m_pListBox->SetScrollBarWidth( nWidth);		}

	// Listener
	virtual void SetListener(MListener* pListener);
	virtual MListener* GetListener();

	// Sort
	void Sort();


	// glue 함수
	void glueAdd(const char* szItem);
	void glueRemove(int i);
	void glueRemoveAll();
	int glueGetCount();
	const char* glueGetSelectedText();
	int glueGetSelelectedIndex();
	void glueSetSelectedIndex(int nIndex);


#define MINT_COMBOBOX	"ComboBox"

protected:
	virtual bool OnCommand( MWidget* pWidget, MEventMsg nMsg, const char* szArgs=NULL);
	// OnRun
	virtual void OnRun( void);

	// OnReaeaseFocus
	virtual void OnReleaseFocus();

	void OnSelectedItemChanged();

	virtual const char* GetClassName( void)					{ return MINT_COMBOBOX;							}
};

} // namespace mint3