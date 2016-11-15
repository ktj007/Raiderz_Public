#pragma once

class GGameGuard : public MTestMemPool<GGameGuard>
{
public:
	GGameGuard(void);
	~GGameGuard(void);

	bool Init();
	void Fini();

	void Enable(bool bEnable);
	bool IsEnable() const;

private:
	bool m_bInit;
	bool m_bEnable;			///< 인증하는지 여부
};
