#pragma once

#include "SServerStatusUpdater.h"

class ZDBManager;

class ZServerStatusUpdater : public SServerStatusUpdater
{
public:
	ZServerStatusUpdater(ZDBManager* pDBManager);
	~ZServerStatusUpdater();

	void	Start(const std::wstring& strServerVersion = L"") override;

protected:
	void	OnUpdate(const float fDelta) override;

private:
	ZDBManager* m_prefDBManager;
};
