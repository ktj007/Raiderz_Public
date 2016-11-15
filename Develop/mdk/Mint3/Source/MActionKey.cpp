#include "stdafx.h"
#include "MActionKey.h"
#include "Mint.h"

namespace mint3 {

ActionKeyForbidVector MActionKey::m_ForbidKey;

MEditLook MActionKey::m_DefaultLook;	
MEditLook* MActionKey::m_pStaticLook = &MActionKey::m_DefaultLook;
void MActionKey::ChangeLook(MEditLook* pLook){	
	if(pLook==NULL)
	{	
		m_pStaticLook = &m_DefaultLook;	
		return;	
	}			
	m_pStaticLook = pLook;			
}													
void MActionKey::ChangeCustomLook(MEditLook* pLook)
{
	m_pCustomLook = pLook;	
}


bool MActionKey::OnEvent(MEvent* pEvent, MListener* pListener)
{
	switch(pEvent->nMessage){
	case MWM_ACTIONKEYDOWN:
		{
		SetActionKey(pEvent->nKey);
		}
		return true;
	case MWM_ACTIONKEYUP:
		return true;
	}
	return false;
}

MActionKey::MActionKey(const char* szName, MWidget* pParent, MListener* pListener)
: MEdit(szName, pParent, pListener )
{
	LOOK_IN_CONSTRUCTOR();

	m_nKey = -1;
}

void MActionKey::OnDraw( MDrawContext* pDC )
{
	if( GetLook() != NULL )
		GetLook()->OnDraw( this, pDC, false );
}

void MActionKey::GetActionKeyName(char* szActionKeyName)
{
	const char* szKeyName = Mint::GetInstance()->GetActionKeyName(m_nKey);
	if(szKeyName!=NULL) strcpy(szActionKeyName, szKeyName);
	else strcpy(szActionKeyName, "N/A");
}

void MActionKey::GetActionKey(unsigned int* pKey)
{
	if(pKey!=NULL) *pKey = m_nKey;
}

void MActionKey::SetActionKey(unsigned int nKey)
{
	if(IsForbidKey(nKey))//등록금지키라면..
		return;

	m_nKey = nKey;
	char szActionKeyName[128] = "";
	GetActionKeyName(szActionKeyName);
	SetText(szActionKeyName);
	
	SetStartPos(0);
}

bool MActionKey::RegisterActionKey(int nActionID)
{
	return Mint::GetInstance()->RegisterActionKey(nActionID, m_nKey);
}

bool MActionKey::UnregisterActionKey(void)
{
	return Mint::GetInstance()->UnregisterActionKey(m_nKey);
}

void MActionKey::RegisterForbidKey(int nkey)
{
	m_ForbidKey.push_back(nkey);
}

bool MActionKey::IsForbidKey(int nkey)
{
	if(m_ForbidKey.empty())
		return false;

	int size = (int)m_ForbidKey.size();
	
	for(int i=0;i<size;i++) {
		if(m_ForbidKey[i]==nkey)
			return true;
	}

	return false;
}


} // namespace mint3