#pragma once

namespace mint3 {

class MItem
{
protected:
	string		m_strUserData;

public:
	bool		m_bEnabled;

	string		m_szString;
	MBitmap*	m_pBitmap;
	MCOLOR		m_Color;

public:
	MItem()
	{
		m_bEnabled	= false;
		m_Color = MColorTable::MBUTTON_TEXT;
		m_pBitmap = NULL;
	}
	virtual ~MItem()	{}

	const char* GetUserData() const { return m_strUserData.c_str(); }
	void SetUserData(const char* val)
	{
		m_strUserData = val;
	}
	virtual bool IsEnabled()	{ return m_bEnabled;}
	virtual bool GetDragItem(MBitmap** ppDragBitmap, char* szDragString, char* szDragItemString)
	{
		if (m_strUserData.empty() || m_strUserData == "0")
		{
			return false;
		}

		*ppDragBitmap = m_pBitmap;
		if( *ppDragBitmap == NULL)
		{
		}

		strcpy(szDragString, m_szString.c_str());
		strcpy(szDragItemString, m_szString.c_str());

		return true;
	}

	virtual void OnDraw( MDrawContext* pDC, MRECT& rect) {}
};

}