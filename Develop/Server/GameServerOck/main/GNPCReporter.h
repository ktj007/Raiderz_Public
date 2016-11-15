#pragma once


///////////////////////////////////////////////////////////////////////////////////////////
// NPC Reporter //////////

class GEntityNPC;

class GNPCReporter
{
private:
	wstring GetHateTableString(GEntityNPC* pNPC);
public:
	GNPCReporter();
	~GNPCReporter();

	bool Report( MUID uidField, MUID uidNPC );
};

