#pragma once

class GPlayerSit : public MTestMemPool<GPlayerSit>
{
public:
	GPlayerSit(void);
	~GPlayerSit(void);
	
	bool IsNowSit() const;

	void Sit();
	void Rise();

private:
	bool m_bNowSit;
};
