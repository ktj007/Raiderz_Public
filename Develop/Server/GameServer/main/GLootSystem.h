#ifndef _GLoot_System_H_
#define _GLoot_System_H_

class GLootBeginner;
class GLootShower;
class GLootLooter;
class GLootMasterLooter;
class GLootEnder;
class GLootBPart;

class GLootModifyHandler;
class GLootItemGetter;

class GLootSystemForMasterServer;

class GEntityPlayer;
class GEntityNPC;

class GLootSystemSetterForTest;

class GLootSystem : public MTestMemPool<GLootSystem>
{
friend class GLootSystemSetterForTest;

public:
	GLootSystem();
	~GLootSystem();
	
	GLootBeginner&		GetBeginner();
	GLootShower&		GetShower();
	GLootLooter&		GetLooter();
	GLootMasterLooter&	GetMasterLooter();
	GLootEnder&			GetEnder();
	GLootBPart&			GetBPart();

	GLootModifyHandler&	GetModifyHandler();
	GLootItemGetter&	GetItemGetter();

	GLootSystemForMasterServer& ForMasterServer();

	GLootSystemSetterForTest& GetSetterForTest();
	
	TEST_VIRTUAL bool PrepareGather(GEntityPlayer* pPlayer, GEntityNPC* pLootingNPC, int nLootID);
	
private:	
	GLootBeginner*		m_pBeginner;
	GLootShower*		m_pShower;
	GLootLooter*		m_pLooter;
	GLootMasterLooter*	m_pMasterLooter;
	GLootEnder*			m_pEnder;
	GLootBPart*			m_pBPart;

	GLootModifyHandler*	m_pModifyHandler;
	GLootItemGetter*	m_pItemGetter;

	GLootSystemForMasterServer* m_pForMasterServer;
	GLootSystemSetterForTest*	m_pSetterForTest;

	
};

#endif //_GLoot_System_H_
