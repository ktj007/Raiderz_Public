#pragma once


class GField;


class GAIRoomMgr : public MTestMemPool<GAIRoomMgr>
{
public:
	struct Item
	{
		MOBox	boxArea;
		int		nGroup;

		Item()
		{
			nGroup = INVALID_ID;
		}
	};
public:
	GAIRoomMgr(void);
	~GAIRoomMgr(void);

	void AddItem(int nGroup, Item item);
	bool IsValid( int nOwnerGroup, vec3 vTargetPos );
	bool HasRoom(int nGroup) const;

	bool Load(wstring strFielname);

private:
	map<int, Item> m_mapItems;
};
