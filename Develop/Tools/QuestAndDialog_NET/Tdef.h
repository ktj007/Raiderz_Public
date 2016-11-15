#pragma once

#define FILENAME_NPCINFO			"npc.xml"
#define FILENAME_QUEST_INFO			"quest.xml"
#define FILENAME_QUEST_CONFIG		"./quest.ini"

#define FILENAME_DIALOG_ONSCREEN	".dialogonscreen.xml"
#define FILENAME_DIALOG_OFFSCREEN	".dialogoffscreen.xml"
#define FILENAME_STRINGS_ONSCREEN	"strings_onscreendialog.xml"
#define FILENAME_STRINGS_OFFSCREEN	"strings_offscreendialog.xml"
#define FILENAME_STRINGS_QUEST		"strings_quest.xml"
#define FILENAME_STRINGS_QUESTTOOL	"strings_questtool.xml"

#define PATH_GAMESERVER				"../GameServer/"
#define PATH_GAMECLIENT				"../../Data/"
#define PATH_SYSTEM					"system/"
#define PATH_DIALOG					"npc/dialog/"
#define PATH_STRINGS				"system/Strings/"

// DB에 저장될때 비순서형 목적은 +100된다.
#define OFFSET_UOBJECTIVE 100 

#define MStringToCharPointer(stringname) \
	(const char*) System::Runtime::InteropServices::Marshal \
	::StringToHGlobalAnsi(stringname).ToPointer()

#define MStringToInt(stringname) \
	atoi(MStringToCharPointer(stringname))

#define MStringToBool(stringname) \
	(0 == MStringToInt(stringname)) ? false : true


#define MAX_TREENODE_TEXT_LEN 1024
#define MAX_TREENODE_TAG_LEN 1024