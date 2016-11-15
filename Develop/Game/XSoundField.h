#pragma once

class XSoundField
{
protected:
	int			m_nMuteAmbient;
	DWORD		m_dwLastTime;


public:
	XSoundField();

	void Clear();

	bool IsMutedAmbient() const;

	static XSoundField* GetInstance();

	void Update( XFieldInfo* pInfo);
};




// inline : XGetSoundField
inline XSoundField* XGetSoundField()
{
	return XSoundField::GetInstance();
}
