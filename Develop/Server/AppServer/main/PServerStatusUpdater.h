#pragma once

#include "SServerStatusUpdater.h"

class PDBManager;

class PServerStatusUpdater : public SServerStatusUpdater
{
public:
	PServerStatusUpdater(PDBManager* pDBManager);
	~PServerStatusUpdater();

	void	Start(const std::wstring& strServerVersion = L"") override;

protected:
	void	OnUpdate(const float fDelta) override;

private:
	PDBManager* m_prefDBManager;

};
