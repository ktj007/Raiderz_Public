#pragma once

namespace rs3 {
	class REffectInstanceSceneNode;
}

class XEffectPreLoaderResourceMgr
{
private:
	vector<REffectInstanceSceneNode*>	m_vecPreLoadResource;

public:
	XEffectPreLoaderResourceMgr(void);
	~XEffectPreLoaderResourceMgr(void);

	void						AddXEffectPreLoader(const wstring& strEffectName);

	void						ReleaseXEffectPreLoader(const wstring& strEffectName);
	void						AllReleaseXEffectPreLoader();
};
