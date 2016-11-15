#pragma once

#include "SServerStatusUpdater.h"

class LDBManager;

class LServerStatusUpdater : public SServerStatusUpdater
{
public:
	LServerStatusUpdater(LDBManager* pDBManager);
	~LServerStatusUpdater();

	void	Start(const std::wstring& strServerVersion = L"") override;

protected:
	void	OnUpdate(const float fDelta) override;

private:
	LDBManager* m_prefDBManager;

};
