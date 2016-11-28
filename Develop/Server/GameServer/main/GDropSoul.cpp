#include "StdAfx.h"
#include "GDropSoul.h"

GDropSoul::GDropSoul()
: nSoulType(SOULT_NONE)
, nAmount(0)
, nAuthorizedCID(0)
{

}

void GDropSoul::Clear()
{
	nSoulType = SOULT_NONE;
	nAmount = 0;
	nAuthorizedCID = 0;
}

bool GDropSoul::IsAuthorizedCID(CID nCID)
{
	if (nCID == nAuthorizedCID) return true;

	return false;
}
