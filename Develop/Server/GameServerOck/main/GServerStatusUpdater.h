#pragma once

#include "SServerStatusUpdater.h"

class GDBManager;

class GServerStatusUpdater : public SServerStatusUpdater, public MTestMemPool<GServerStatusUpdater>
{
public:
	GServerStatusUpdater(GDBManager* pDBManager);
	~GServerStatusUpdater();

	void	Start(const std::wstring& strServerVersion = L"") override;

protected:
	void	OnUpdate(const float fDelta) override;

private:
	GDBManager* m_prefDBManager;

};
