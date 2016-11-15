#include "stdafx.h"
#include "MComboBox.h"
#include "Mint.h"


// Define fade factor
const float FADE_FACTOR = 0.0045f;						// 값이 클수록 페이드 시간이 짧아진다



namespace mint3 {

MComboListBox::MComboListBox(MWidget* pParent, MListener* pListener)
: MListBox(pParent, pListener)
{
	LOOK_IN_CONSTRUCTOR()
	SetClipByParent( false);
}

bool MComboListBox::OnEvent(MEvent* pEvent, MListener* pListener)
{
	if(MListBox::OnEvent(pEvent, pListener)==true) return true;
	
	switch(pEvent->nMessage){
	case MWM_LBUTTONDOWN:
	case MWM_RBUTTONDOWN:
	case MWM_LBUTTONDBLCLK:
	case MWM_RBUTTONDBLCLK:
		Show(false);
		return false;	// 다른 Widget에게 넘겨준다.
	}

	return false;
}

IMPLEMENT_LOOK(MComboListBox, MListBoxLook)


#define DEFAULT_DROP_HEIGHT	200

MComboBox::MComboBox(const char* szName, MWidget* pParent, MListener* pListener)
: MButton(szName, pParent, pListener)
{
	MButton::SetListener(this);					// Button 메세지는 한번 걸러서 보내준다.

	m_bHighlight = false;						// 콤보박스에서는 하이라이트(&)를 지원할 필요가 없다.

	m_pComboBoxListener = pListener;

	m_AlignmentMode = MAM_HCENTER|MAM_VCENTER;

	m_Type = MBT_PUSHBTN;						// 버튼은 푸쉬버튼 형태

	m_nDropHeight = DEFAULT_DROP_HEIGHT;		// 드롭박스 높이 설정

	m_bComboButton = true;						// 콤보 버튼으로 설정

	m_dwFadeTimer = timeGetTime();				// 페이트 타이머 초기화


	// 리스트 박스 생성
	m_pListBox = new MComboListBox( pParent, this);
	m_pListBox->Show( false);
	m_pListBox->SetOpacity( 0.0f);
}

MComboBox::~MComboBox()
{
	if (!IsEmptyChildWidgets())
	{
		delete m_pListBox;
	}
	m_pListBox = NULL;
}

void MComboBox::SetDropSize(int nHeight)
{
	m_nDropHeight = nHeight;
}

void MComboBox::Add(const char* szItem)
{
	m_pListBox->Add(szItem);
}

void MComboBox::Add(MListItem* pItem)
{
	m_pListBox->Add(pItem);
}

const char* MComboBox::GetString(int i)
{
	return m_pListBox->GetString(i);
}

MListItem* MComboBox::Get(int i)
{
	return m_pListBox->Get(i);
}

void MComboBox::Remove(int i)
{
	m_pListBox->Remove(i);
}

void MComboBox::RemoveAll()
{
	m_pListBox->RemoveAll();
}

int MComboBox::GetCount()
{
	return m_pListBox->GetCount();
}

int MComboBox::GetSelIndex()
{
	return m_pListBox->GetSelIndex();
}

bool MComboBox::SetSelIndex(int i)
{
	bool bReturn = m_pListBox->SetSelIndex(i);
	if(bReturn==true) SetText(GetSelItemString());
	return bReturn;
}

const char* MComboBox::GetSelItemString()
{
	return m_pListBox->GetSelItemString();
}

MListItem* MComboBox::GetSelItem()
{
	return m_pListBox->GetSelItem();
}

void MComboBox::SetNextSel()
{
	int cnt = GetCount();
	int sel = GetSelIndex();

	if(cnt > 1) {
		if(sel+1==cnt) {
			SetSelIndex(0);
		}
		else {
			SetSelIndex(sel+1);
		}

		OnSelectedItemChanged();
	}
}

void MComboBox::SetPrevSel()
{
	int cnt = GetCount();
	int sel = GetSelIndex();

	if(cnt > 1) {
		if(sel==0) {
			SetSelIndex(cnt-1);
		} else {
			SetSelIndex( sel-1 );
		}

		OnSelectedItemChanged();
	}
}

bool MComboBox::OnCommand(MWidget* pWidget, MEventMsg nMsg, const char* szArgs)
{
	// 리스트 박스가 아직 없으면 그냥 리턴
	if ( m_pListBox == NULL)
		return true;


	// 버튼 메시지일 경우...
	if ( pWidget == this)
	{
		if ( nMsg == MEVENT_BUTTON_CLICK)
		{
			// 체크보다 클릭이벤트가 먼저 오기 때문에
			// 지금 체크 상태를 false로 검사한다.
			if ( GetCheck() == false)
				m_pListBox->SetFocus();
			return true;
		}
		return false;
	}


	// 리스트박스 메시지일 경우...
	else if ( pWidget == m_pListBox)
	{
		// 마우스에 의한 선택
		if ( nMsg == MEVENT_LB_ITEM_SEL || nMsg == MEVENT_LB_ITEM_SEL2 || nMsg == MEVENT_LB_ITEM_DBLCLK)
		{
			// 버튼의 체크 상태를 해제한다
			SetCheck( false);


			// 버튼의 텍스트를 설정한다
			SetText( m_pListBox->GetSelItemString());


			// 리스너에 커맨드 전달
			OnSelectedItemChanged();
		}
		
		return true;
	}


	return false;


/* 	if(pWidget==this && nMsg == MEVENT_BUTTON_CLICK) {

		if( GetComboDropped() ) 
		{
			m_pListBox->Show(false);
			SetComboDropped(false);
			ReleaseFocus();
			return true;
		}
		
		bool bSCheck = false;

 		if(m_nComboType == 0) { // 2부분으로 나누어처리

			if( GetNextComboBoxTypeSize() > m_ClickPos.x ) {
				bSCheck = true;
			}
		}

		// DropHeight가 넉넉하면 자동으로 크기를 줄인다.

		if(bSCheck) {

			int cnt = GetCount();
			int sel = GetSelIndex();

			if(cnt > 1) {
				if(sel+1==cnt) {
					SetSelIndex(0);
				}
				else {
					SetSelIndex(sel+1);
				}

				if(m_pComboBoxListener!=NULL){
					m_pComboBoxListener->OnCommand(this, MEVENT_CB_CHANGED);
				}

			}
		}
		else {
		
			int nFrameHeight = m_pListBox->GetRect().h - m_pListBox->GetClientRect().h;
 			int nDropHeight	 = min(m_nDropHeight,m_pListBox->GetItemHeight()*m_pListBox->GetCount()+nFrameHeight);

			bool bDropUnder = true;

			MPOINT sp = MClientToScreen(this,MPOINT(r.x,r.y+r.h));

			if( ( GetRect().y + GetRect().h + nDropHeight + 10) > MGetWorkspaceHeight())
				bDropUnder = false;

			if(m_bAutoDrop)
			{
				if(bDropUnder==true)	m_pListBox->SetBounds(MRECT(r.x, r.y+r.h, r.w, nDropHeight));
				else					m_pListBox->SetBounds(MRECT(r.x, r.y-nDropHeight, r.w, nDropHeight));
			}
			else
			{
				if(m_bDropUnder)	m_pListBox->SetBounds(MRECT(r.x, r.y+r.h, r.w, nDropHeight));
				else				m_pListBox->SetBounds(MRECT(r.x, r.y-nDropHeight, r.w, nDropHeight));
			}

			m_pListBox->Show(true, true);
			SetComboDropped( true );
		}

		return true;
	}
	else if(pWidget==m_pListBox) {

		if(nMsg == MEVENT_LB_ITEM_SEL || 
		   nMsg == MEVENT_LB_ITEM_SEL2 || 
		   nMsg == MEVENT_LB_ITEM_DBLCLK)
		{
			m_pListBox->Show(false);
			SetComboDropped( false );
			SetText(m_pListBox->GetSelItemString());
			if(m_pComboBoxListener!=NULL){
				m_pComboBoxListener->OnCommand(this, MEVENT_CB_CHANGED);
			}
			return true;
		}
	}

	return false;
*/
}

void MComboBox::Sort()
{
	m_pListBox->Sort();
}

void MComboBox::SetListener(MListener* pListener)
{
	m_pComboBoxListener = pListener;
}
MListener* MComboBox::GetListener()
{
	return MWidget::GetListener();
}

void MComboBox::OnReleaseFocus()
{
//	if( GetCheck() && !m_pListBox->IsFocus())
//		SetCheck( false);

/*
	if( !m_pListBox->IsFocus() && GetComboDropped() )
	{
		SetComboDropped(false);
		m_pListBox->Show(false);
	}
*/
}


// Runtime Loop
void MComboBox::OnRun(void)
{
	// 리스트박스가 아직 없으면 종료
	if ( !m_pListBox)
		return;


	// 콤보 박스나 리스트 박스가 보이지 않으면 리턴
	// - 콤보 박스를 클릭했을 때 리스트가 보이지 않아 주석처리 합니다. - birdkr
	//if ( !IsVisible() || !m_pListBox->IsVisible())
	//{
	//	m_pListBox->SetOpacity( 1.0f);
	//	return;
	//}


	// 10ms마다 수행
	DWORD curr = timeGetTime();
	DWORD diff = curr - m_dwFadeTimer;

	if ( diff < 10)
		return;

	m_dwFadeTimer = curr;


	// 리스트 박스가 포커스를 잃으면 감춘다
	if ( GetCheck() && !m_pListBox->IsFocus())
		SetCheck( false);


	// 리스트 박스의 위치 및 사이즈를 설정 한다
	if ( m_pListBox->IsVisible())
	{
		MRECT rect = GetRect();
		if ( m_pParent && ((rect.y + rect.h + m_nDropHeight) > m_pParent->GetClientRect().h))
			rect.y -= m_nDropHeight;
		else
			rect.y += rect.h;

		rect.h = m_nDropHeight;
		m_pListBox->SetBounds( rect);
	}


	// 리스트 박스를 페이드 시킨다
	float delta  = (float)diff * FADE_FACTOR;
	float fFade = m_pListBox->GetOpacity() + ( GetCheck() ? delta : -delta);

	if ( fFade > 1.0f)			fFade = 1.0f;
	else if ( fFade < 0.0f)		fFade = 0.0f;

	m_pListBox->SetOpacity( fFade);

	bool bShow = (fFade > 0.0f) ? true : false;

	if (bShow == false)
	{
		m_pListBox->Show( false );
	}
	else
	{
		if (m_pListBox->IsVisible() == false)
		{
			m_pListBox->Show( true, true );
		}
	}
}


// 루아와 통신하는 함수
void MComboBox::glueAdd(const char* szItem)
{
	if (szItem == NULL)
	{
		return;
	}
	Add(szItem);
}

void MComboBox::glueRemove(int i)
{
	if (i > GetCount())
	{
		return;
	}
	Remove(i);
}

void MComboBox::glueRemoveAll()
{
	RemoveAll();
}

int MComboBox::glueGetCount()
{
	return GetCount();
}

const char* MComboBox::glueGetSelectedText()
{
	return GetSelItemString();
}

int MComboBox::glueGetSelelectedIndex()
{
	return GetSelIndex();
}

void MComboBox::glueSetSelectedIndex(int nIndex)
{
	SetSelIndex(nIndex);
}

void MComboBox::OnSelectedItemChanged()
{
	if(m_pComboBoxListener!=NULL)
	{
		m_pComboBoxListener->OnCommand(this, MEVENT_CB_SELECTED_INDEX_CHANGED);
	}
	m_EventHandler.OnCommand(this, MEVENT_CB_SELECTED_INDEX_CHANGED);
}

} // namespace mint3