#include "StdAfx.h"
#include "XDef.h"
#include "XBPartCharacter.h"
#include "XCharacterBParts.h"

MImplementRTTI(XBPartCharacter, XCharacter);



XBPartCharacter::XBPartCharacter()
: XCharacter()
{
	
}

void XBPartCharacter::CreateCharacterParts()
{
	m_pParts = new XCharacterBParts(this);
}
