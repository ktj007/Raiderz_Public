#pragma once

class GWorld : public MTestMemPool<GWorld>
{
public:
	GWorld() {}
	virtual ~GWorld() {}
	void Update(float fDelta);
	void SendMyInfo(MUID& uidPlayer);
	void Create();
	void Destroy();
};