#pragma once


class CCSAuth2;
typedef struct _GG_AUTH_DATA GG_AUTH_DATA;
class GEntityPlayer;


class GPlayerGameGuard : public MTestMemPool<GPlayerGameGuard>
{
public:
	GPlayerGameGuard(GEntityPlayer* pOwner);
	~GPlayerGameGuard(void);

	void AuthAnswer(GG_AUTH_DATA* paaData);

	void Update(float fDelta);
private:
	void Disconnect();
	bool IsAuthEnable();
private:
	GEntityPlayer* m_pOwner;
	CCSAuth2*	m_pCSA;				///< 게임가드 인증 객체
	MRegulator	m_rgrTick; 
};
