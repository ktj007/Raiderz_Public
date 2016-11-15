#pragma once

class GServerFieldRouter : public MTestMemPool<GServerFieldRouter>
{
public:
	GServerFieldRouter();
	virtual ~GServerFieldRouter();

	void RequestAllFieldInfo(void);
	void RequestAllFieldGroupInfo(void);
	void RequestDelDynamicFieldGroup(MUID uidFieldGroup);
};
