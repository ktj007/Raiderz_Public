#pragma once

class SFieldChecksum
{
private:
	uint32		m_nValue;
public:
	SFieldChecksum() : m_nValue(0) {}
	SFieldChecksum(uint32 nValue) : m_nValue(nValue) {}
	~SFieldChecksum() {}

	void Reset() { m_nValue = 0; }
	void Add(MUID& uidField)
	{
		m_nValue += uidField.High;
		m_nValue += (uidField.Low + uidField.Low);
	}
	uint32 GetValue() { return m_nValue; }
};
