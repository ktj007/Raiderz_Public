#pragma once

class GDropSoul : public MTestMemPool<GDropSoul>
{
public:
	GDropSoul();

	void Clear();
	bool IsAuthorizedCID(CID nCID);

public:
	SOUL_TYPE nSoulType;
	int nAmount;
	CID nAuthorizedCID;	// 획득 권한이 있는 사람의 CID
};
