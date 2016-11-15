#include "stdafx.h"
#include "TestCommonFixture.h"
//#include "XNPCInteractionVoice.h"
/*
SUITE(SoundHelper)
{
	TEST_FIXTURE(SimpleTestFixture, TestSoundHelper)
	{
		global.sound->Create(FILENAME_SOUNDLIST);

		info.npc->CreateNPCFromString(L"<NPC id=\"107025\" Name=\"·»°íÆ® º´Á¤\" Grade=\"3\" VoiceType=\"soldiermale3\" Type=\"monster\" MeshPath=\"Monster/goblin_mage\" MeshName=\"goblin_mage\"	/>");

		const MUID uidNPC = global.system->GenerateLocalUID();
		m_Helper.InNPC(uidNPC, 107025, vec3(1000.0f, 980.0f, 0.0f));

		XNPCInteractionVoice	NPCVoice;

		wstring strSoundName;
		strSoundName = NPCVoice.GetVoiceSoundName_TEST(uidNPC, L"HELLO");
		CHECK(strSoundName == wstring(L"soldiermale3_1"));

		strSoundName = NPCVoice.GetVoiceSoundName_TEST(uidNPC, L"SHOP");
		CHECK(strSoundName == wstring(L"male3_6") || strSoundName == wstring(L"male3_7") ||
			strSoundName == wstring(L"male3_8") || strSoundName == wstring(L"male3_9"));
	}
}
*/