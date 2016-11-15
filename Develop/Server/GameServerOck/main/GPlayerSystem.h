#ifndef _GCHAR_SYSTEM_H_
#define _GCHAR_SYSTEM_H_

#include "CSTriggerInfo.h"
#include "CSChrInfo.h"

class GEntityPlayer;
class GPlayerObject;

class GPlayerSystem : public MTestMemPool<GPlayerSystem>
{
public:
	bool AddTP(GEntityPlayer* pPlayer, int nTP);
	bool WriteDieLogToDB(GEntityPlayer* pPlayer, int nCode, int KillerNpcID, const MUID& uidKillerPlayer, int nKillerCode);

	bool Sit(GEntityPlayer* pPlayer);
	bool SitRise(GEntityPlayer* pPlayer);
	
	bool SaveSoulBinding(GEntityPlayer* pEntityPlayer, SBID nSoulBindingID);

	const MARKER_INFO* GetSoulBindingMarker(SBID nSoulBindingID);
	SBID GetSoulBindingFieldID(GEntityPlayer* pPlayer);

	bool MoveToSelectChar(GPlayerObject* pPlayerObject, bool bForce=false);

private:	
	bool SaveSoulBinding_Apply(GEntityPlayer* pEntityPlayer, SBID nSoulBindingID);
	void SaveSoulBinding_Route(GEntityPlayer* pEntityPlayer, int nFieldID, int nMarkerID);

	bool MoveToSelectChar_Check(GPlayerObject* pPlayerObject);
	void MoveToSelectChar_Apply(GPlayerObject* pPlayerObject);
};

#endif//_GCHAR_SYSTEM_H_
