#pragma once

class MockDuel;
class GEntityPlayer;

class GUTHelper_Duel
{
public:
	static MockDuel* NewMockDuel(GEntityPlayer* pPlayer1, GEntityPlayer* pPlayer2);
	static MockDuel* NewMockDuel_Fighting(GEntityPlayer* pPlayer1, GEntityPlayer* pPlayer2);
};
