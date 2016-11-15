#pragma once

class GPlayerSerializer
{
	struct Info
	{
		CID cid;
		MUID uidField;
		vec3 vPos;
		vec3 vDir;
	};
public:
	GPlayerSerializer(void);
	~GPlayerSerializer(void);

	void SaveFromManager();
	void RestoreAll();

private:
	map<MUID,Info> m_mapInfos;
	vector<MUID> m_vecPlayers;
};
