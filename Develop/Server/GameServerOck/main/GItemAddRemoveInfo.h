#pragma once

class GItemFillInfo : public MTestMemPool<GItemFillInfo>
{
public:
	GItemFillInfo(int nID, int nAmount);
	int GetID() { return m_nID; }
	int GetAmount() { return m_nAmount; }
private:
	int m_nID;
	int m_nAmount;
};

class GItemAddInfo : public MTestMemPool<GItemAddInfo>
{
public:
	GItemAddInfo(int nID, int nAmount);
	int GetID() { return m_nID; }
	int GetAmount() { return m_nAmount; }
private:
	int m_nID;
	int m_nAmount;
};

class GItemRemoveInfo : public MTestMemPool<GItemRemoveInfo>
{
public:
	GItemRemoveInfo(int nID, int nAmount);
	int GetID() { return m_nID; }
	int GetAmount() { return m_nAmount; }
private:
	int m_nID;
	int m_nAmount;
};

class GItemRemoveAllInfo : public MTestMemPool<GItemRemoveAllInfo>
{
public:
	GItemRemoveAllInfo(int nID);
	int GetID() { return m_nID; }
private:
	int m_nID;
};

class GItemAddRemoveInfo : public MTestMemPool<GItemAddRemoveInfo>
{
public:
	~GItemAddRemoveInfo();

	void InsertItemFillInfo(int nID, int nAmount);
	void InsertItemAddInfo(int nID, int nAmount);
	void InsertItemRemoveInfo(int nID, int nAmount);
	void InsertItemRemoveAllInfo(int nID);

	const vector<GItemFillInfo*>&		GetItemFillInfo();
	const vector<GItemAddInfo*>&		GetItemAddInfo();
	const vector<GItemRemoveInfo*>&		GetItemRemoveInfo();
	const vector<GItemRemoveAllInfo*>&	GetItemRemoveAllInfo();
	

private:
	vector<GItemFillInfo*>		m_vecItemFillInfo;
	vector<GItemAddInfo*>		m_vecItemAddInfo;
	vector<GItemRemoveInfo*>	m_vecItemRemoveInfo;
	vector<GItemRemoveAllInfo*>	m_vecItemRemoveAllInfo;
};
