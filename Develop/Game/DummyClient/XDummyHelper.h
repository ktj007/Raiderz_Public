#pragma once

class XBirdDummyAgent;


class XDummyHelper
{
private:
	static wstring GetComputerName();
public:
	static int GetRandomFieldID(int nIgnoreFieldID);
	static int GetRandomIDFromList(vector<int>& vecIDList, int nIgnoreID = 0);

	static void PostMoveRandomPosition(XBirdDummyAgent* pAgent);

	static wstring MakeAutoID();

	static wstring MakeCharacterName(wstring strLoginID, int nAgentID);
};
