#pragma once

class GItemSystem;
class GItemAdder;
class GItemRemover;

class GItemSystemSetterForTest : public MTestMemPool<GItemSystemSetterForTest>
{
public:
	GItemSystemSetterForTest(GItemSystem* pOwner);

	void SetAdder(GItemAdder* pAdder);
	void SetRemover(GItemRemover* pRemover);

private:
	GItemSystem* m_pOwner;
};
