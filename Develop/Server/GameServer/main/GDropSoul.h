#pragma once

class GDropSoul : public MTestMemPool<GDropSoul>
{
public:
	GDropSoul();

	void Clear();
	bool IsAuthorizedCID(int nCID);

public:
	SOUL_TYPE nSoulType;
	int nAmount;
	int nAuthorizedCID;	// 획득 권한이 있는 사람의 CID
};
