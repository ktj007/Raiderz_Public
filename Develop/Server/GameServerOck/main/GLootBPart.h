#pragma once

class GLootBPart : public MTestMemPool<GLootBPart>
{
public:
	GLootBPart(void);
	~GLootBPart(void);

	bool IsBPartLoot( MUID uidNPC );
	void TakeAll( GEntityPlayer* pPlayer, MUID uidNPC );
};
