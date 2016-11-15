#include "StdAfx.h"
#include "GItemAddRemoveInfo.h"
#include "AStlUtil.h"

GItemFillInfo::GItemFillInfo(int nID, int nAmount)
: m_nID(nID)
, m_nAmount(nAmount)
{
}

GItemAddInfo::GItemAddInfo(int nID, int nAmount)
: m_nID(nID)
, m_nAmount(nAmount)
{
}

GItemRemoveInfo::GItemRemoveInfo(int nID, int nAmount)
: m_nID(nID)
, m_nAmount(nAmount)
{
}

GItemRemoveAllInfo::GItemRemoveAllInfo(int nID)
: m_nID(nID)
{
}


GItemAddRemoveInfo::~GItemAddRemoveInfo()
{
	SAFE_DELETE_VECTOR(m_vecItemFillInfo);
	SAFE_DELETE_VECTOR(m_vecItemAddInfo);
	SAFE_DELETE_VECTOR(m_vecItemRemoveInfo);
	SAFE_DELETE_VECTOR(m_vecItemRemoveAllInfo);
}

void GItemAddRemoveInfo::InsertItemFillInfo(int nID, int nAmount)
{
	m_vecItemFillInfo.push_back(new GItemFillInfo(nID, nAmount));
}

void GItemAddRemoveInfo::InsertItemAddInfo(int nID, int nAmount)
{
	m_vecItemAddInfo.push_back(new GItemAddInfo(nID, nAmount));
}

void GItemAddRemoveInfo::InsertItemRemoveInfo(int nID, int nAmount)
{
	m_vecItemRemoveInfo.push_back(new GItemRemoveInfo(nID, nAmount));
}

void GItemAddRemoveInfo::InsertItemRemoveAllInfo(int nID)
{
	m_vecItemRemoveAllInfo.push_back(new GItemRemoveAllInfo(nID));
}

const vector<GItemFillInfo*>& GItemAddRemoveInfo::GetItemFillInfo()
{
	return m_vecItemFillInfo;
}

const vector<GItemAddInfo*>& GItemAddRemoveInfo::GetItemAddInfo()
{
	return m_vecItemAddInfo;
}

const vector<GItemRemoveInfo*>& GItemAddRemoveInfo::GetItemRemoveInfo()
{
	return m_vecItemRemoveInfo;
}

const vector<GItemRemoveAllInfo*>& GItemAddRemoveInfo::GetItemRemoveAllInfo()
{
	return m_vecItemRemoveAllInfo;
}