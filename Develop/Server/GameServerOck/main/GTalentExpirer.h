#pragma once

class GTalentInfo;

#define NO_EXPIRE_TALENT	(-1.0f)

class GTalentExpirer
{
public:
	GTalentExpirer(void);
	~GTalentExpirer(void);

	bool IsExpired(float fElapsedTime);
	void SetExpiredTime(float fExpiredTime);
	float GetExpiredTime()						{ return m_fExpiredTime; }

private:
	float m_fExpiredTime;
};
