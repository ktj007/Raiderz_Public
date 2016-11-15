#pragma once

class GLootSystem;
class GLootBeginner;
class GLootShower;
class GLootLooter;
class GLootMasterLooter;
class GLootEnder;
class GLootModifyHandler;
class GLootItemGetter;

class GLootSystemSetterForTest : public MTestMemPool<GLootSystemSetterForTest>
{
public:
	GLootSystemSetterForTest(GLootSystem* pOwner);

	void SetStarter(GLootBeginner* pStarter);
	void SetShower(GLootShower* pShower);
	void SetLooter(GLootLooter* pLooter);
	void SetMasterLooter(GLootMasterLooter* pMasterLooter);
	void SetEnder(GLootEnder* pEnder);

	void SetModifyHandler(GLootModifyHandler* pModifyHandler);
	void SetItemGetter(GLootItemGetter* pItemGetter);

private:
	GLootSystem* m_pOwner;
};
