#pragma once

#include "SServableChecker.h"

class GServableChecker : public SServableChecker, public MTestMemPool<GServableChecker>
{
protected:
	bool Check(const float fDelta) override;

private:
	void SetServerStateActive(bool bIsActive);

	bool Check_IsStandAlone();
	bool Check_IsConnectedMasterServer();
	bool Check_IsConnectedAppServer();
	bool Check_ManagedFIeldInfoCountFromMasterServer();
	bool Check_CreatingSharedFIeld();

};
