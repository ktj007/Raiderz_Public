#pragma once

#include "ServerCommonUnitTestLib.h"
#include "SSystem.h"
#include "MTString.h"
#include "STypes.h"

class SCOMMON_UNITTEST_API SUnitTestUtil
{
private:
	static MUID			m_UID;
	static MUID			m_nNPCUID;
	static int64		m_nIUID;
	static int			m_nCID;	
	static int			m_nID;
	static uint8		m_nUINT8ID;
	static uint32		m_nUINT32ID;
	static int			m_nNumber;
	static int			m_nGID;
	static int			m_nName;
	static double		m_nGuildName;
	static int			m_nString;
	static AID			m_nAID;

public:
	static MUID NewUID()
	{
		m_UID++;
		return m_UID;
	}

	static MUID NewNPCUID()
	{
		m_nNPCUID++;
		return m_nNPCUID;
	}

	static int64 NewIUID()
	{
		m_nIUID++;
		return m_nIUID;
	}

	static int NewCID()
	{
		m_nCID++;
		return m_nCID;
	}

	static int NewID()
	{
		m_nID++;
		return m_nID;
	}

	static int NewUINT8ID()
	{
		m_nUINT8ID++;
		return m_nUINT8ID;
	}

	static int NewUINT32ID()
	{
		m_nUINT32ID++;
		return m_nUINT32ID;
	}

	static int NewNumber()
	{
		m_nNumber++;
		return m_nNumber;
	}

	static int NewGID()
	{
		m_nGID++;
		return m_nGID;
	}

	static tstring NewName()
	{
		m_nName++;

		CString strName;
		strName.Format(_T("%d"), m_nName);

		return strName.GetString();
	}

	static tstring NewGuildName()
	{
		m_nGuildName++;

		CString strName;
		strName.Format(_T("%d"), (int) m_nGuildName);

		return strName.GetString();
	}

	static tstring NewString()
	{
		m_nString++;

		CString strName;
		strName.Format(_T("%d"), (int) m_nString);

		return strName.GetString();
	}

	static int NewID(int nID);

	static MUID NewUID(MUID UID)
	{
		if (UID.IsValid())
			return UID;

		return NewUID();
	}

	static int NewAID()
	{
		m_nAID++;
		return m_nAID;
	}
};

#define TVALID(x)		if (!(x)) { CHECK(x); return; }
#define TVALID_EQ(x,y)	if ((x) != (y)) { CHECK_EQUAL(x,y); return; }

//////////////////////////////////////////////////////////////////////////
//
// GConstRecovery
//
//////////////////////////////////////////////////////////////////////////
template<typename T>
class SConstRecovery
{
public:
	SConstRecovery(T& _ptrConst, T _dataNew): ptrConst(&_ptrConst) {
		dataOld = *ptrConst;
		*ptrConst = _dataNew;
	}

	~SConstRecovery() {
		*ptrConst = dataOld;
	}
public:
	T*	ptrConst;
	T	dataOld;
};