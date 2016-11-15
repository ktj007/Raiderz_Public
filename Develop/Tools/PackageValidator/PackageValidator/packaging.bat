@echo off
rem publish.bat [대상 폴더명]


if "%1" == "" goto :err_no_target_path
if not exist "pkzip25.exe" goto :err_not_exist_pkzip
if not exist "conv_zip.exe" goto :err_not_exist_convzip


SET TARGETPATH=%1\%DATE%_%TIME:~0,2%_%TIME:~3,2%


:add_files

echo .
echo ---------------------------------------------------------------------
echo  대상 폴더 생성
echo  Path = %TARGETPATH%
echo ---------------------------------------------------------------------
echo .
mkdir "%TARGETPATH%"



echo .
echo ---------------------------------------------------------------------
echo  실행 관련 파일
echo ---------------------------------------------------------------------
echo .
copy "Raiderz.exe"						"%TARGETPATH%\Raiderz.exe"
copy "Raiderz Launcher.exe"				"%TARGETPATH%"
copy "BAReport.exe"						"%TARGETPATH%"
copy "bdcap32.dll"						"%TARGETPATH%"
copy "D3DCompiler_42.dll"				"%TARGETPATH%"
copy "d3dx9_42.dll"						"%TARGETPATH%"
copy "dbghelp.dll"						"%TARGETPATH%"
copy "fmodex.dll"						"%TARGETPATH%"
copy "lua5.1.dll"						"%TARGETPATH%"
copy "msvcp80.dll"						"%TARGETPATH%"
copy "msvcr80.dll"						"%TARGETPATH%"
copy "NxCharacter.dll"					"%TARGETPATH%"
copy "NxCooking.dll"					"%TARGETPATH%"
copy "NxExtensions.dll"					"%TARGETPATH%"
copy "pathengine.dll"					"%TARGETPATH%"
copy "PhysXLoader.dll"					"%TARGETPATH%"
copy "SpeedTreeRT.dll"					"%TARGETPATH%"
copy "vcomp90.dll"						"%TARGETPATH%"
copy "config_rel.xml"					"%TARGETPATH%\config.xml"
echo Read only MRS files mode. >> "%TARGETPATH%\READ_MRS.sys"



echo .
echo ---------------------------------------------------------------------
echo  압축 파일 생성
echo ---------------------------------------------------------------------
echo .
cd "Data"
mkdir "%TARGETPATH%\Data"

echo 임시 파일 삭제중...
del PKZ*.tmp /s /q /f



echo .
echo ---------------------------------------------------------------------
echo  시스템
echo ---------------------------------------------------------------------
echo .
..\pkzip25 -add -path=specify "%TARGETPATH%\Data\system.zip"							-include="system\*.xml"
..\pkzip25 -add -path=specify "%TARGETPATH%\Data\system.zip"							"system\Strings\*.*"



echo .
echo ---------------------------------------------------------------------
echo  컷씬
echo ---------------------------------------------------------------------
echo .
..\pkzip25 -add -path=specify "%TARGETPATH%\Data\CutScene.zip"							"CutScene\*.*"



echo .
echo ---------------------------------------------------------------------
echo  필드
echo ---------------------------------------------------------------------
echo .
..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Field.zip"								"Field\*.*"



echo .
echo ---------------------------------------------------------------------
echo  인터페이스
echo ---------------------------------------------------------------------
echo .
..\pkzip25 -add -path=specify "%TARGETPATH%\Data\system.zip"							"Interface\ui_main.xml"
cd "Interface"
mkdir "%TARGETPATH%\Data\Interface"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Interface\common.zip"				"common\*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Interface\login.zip"				"login\*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Interface\character.zip"			"character\*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Interface\character.zip"			"character\resource\*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Interface\game.zip"					"game\*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Interface\game.zip"					"game\resource\*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Interface\game.zip"					"game\resource\fieldillust\*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Interface\game.zip"					"game\resource\Quest\*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Interface\game.zip"					"game\tray\*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Interface\cutscene.zip"				"cutscene\*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Interface\icon.zip"					"icon\*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Interface\minimap.zip"				"minimap\*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Interface\screeneffects.zip"		"screeneffects\*.*"
cd ..



echo .
echo ---------------------------------------------------------------------
echo  모델
echo ---------------------------------------------------------------------
echo .
cd "Model"
mkdir "%TARGETPATH%\Data\Model"

echo .
echo ---------------------------------------------------------------------
echo  맵 오브젝트
echo ---------------------------------------------------------------------
echo .
cd "MapObject"
mkdir "%TARGETPATH%\Data\Model\MapObject"
..\..\..\pkzip25 -add "%TARGETPATH%\Data\Model\MapObject\MapObject01.zip"				"article\*.*"
..\..\..\pkzip25 -add "%TARGETPATH%\Data\Model\MapObject\MapObject01.zip"				"broken-mast\*.*"
..\..\..\pkzip25 -add "%TARGETPATH%\Data\Model\MapObject\MapObject01.zip"				"catacomb_of_damned\*.*"
..\..\..\pkzip25 -add "%TARGETPATH%\Data\Model\MapObject\MapObject01.zip"				"contaminated-garden\*.*"
..\..\..\pkzip25 -add "%TARGETPATH%\Data\Model\MapObject\MapObject01.zip"				"effect\*.*"
..\..\..\pkzip25 -add "%TARGETPATH%\Data\Model\MapObject\MapObject01.zip"				"Grand Wall of the Silent\*.*"
..\..\..\pkzip25 -add "%TARGETPATH%\Data\Model\MapObject\MapObject01.zip"				"hero's-lane\*.*"
..\..\..\pkzip25 -add "%TARGETPATH%\Data\Model\MapObject\MapObject01.zip"				"ingen-freeport\*.*"
..\..\..\pkzip25 -add "%TARGETPATH%\Data\Model\MapObject\MapObject01.zip"				"light-templates\*.*"
..\..\..\pkzip25 -add "%TARGETPATH%\Data\Model\MapObject\MapObject01.zip"				"login\*.*"
..\..\..\pkzip25 -add "%TARGETPATH%\Data\Model\MapObject\MapObject01.zip"				"makot-village\*.*"
..\..\..\pkzip25 -add "%TARGETPATH%\Data\Model\MapObject\MapObject01.zip"				"mt-eda\*.*"

..\..\..\pkzip25 -add "%TARGETPATH%\Data\Model\MapObject\MapObject01.zip"				"npc-obj\*.*"
..\..\..\pkzip25 -add "%TARGETPATH%\Data\Model\MapObject\MapObject02.zip"				"rengot-village\*.*"
..\..\..\pkzip25 -add "%TARGETPATH%\Data\Model\MapObject\MapObject02.zip"				"riode\*.*"
..\..\..\pkzip25 -add "%TARGETPATH%\Data\Model\MapObject\MapObject02.zip"				"ritz\*.*"
..\..\..\pkzip25 -add "%TARGETPATH%\Data\Model\MapObject\MapObject02.zip"				"ritz-sewer\*.*"
..\..\..\pkzip25 -add "%TARGETPATH%\Data\Model\MapObject\MapObject02.zip"				"ritz-town\*.*"
..\..\..\pkzip25 -add "%TARGETPATH%\Data\Model\MapObject\MapObject02.zip"				"saint-fhannel-monastery\*.*"
..\..\..\pkzip25 -add "%TARGETPATH%\Data\Model\MapObject\MapObject02.zip"				"teress-plain\*.*"
..\..\..\pkzip25 -add "%TARGETPATH%\Data\Model\MapObject\MapObject02.zip"				"trial-field\*.*"
cd ..

echo .
echo ---------------------------------------------------------------------
echo  몬스터
echo ---------------------------------------------------------------------
echo .
cd "Monster"
mkdir "%TARGETPATH%\Data\Model\Monster"

rem ######## 테레스 평원 ##################################################
rem 거미
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster01.zip"		"spider01\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster01.zip"		"spider02\*.*"

rem 테레시스
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster01.zip"		"spider_boss\*.*"

rem 늑대
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster01.zip"		"wolf\*.*"

rem 우르크
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster01.zip"		"uruk\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster01.zip"		"uruk_dead\*.*"

rem 무스탕
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster01.zip"		"tyranis\*.*"

rem 아루
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster01.zip"		"walking_tree\*.*"

rem 렌고트
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster01.zip"		"goblin_archer\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster01.zip"		"goblin_chief\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster01.zip"		"goblin_commander\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster01.zip"		"goblin_mage\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster01.zip"		"goblin_pawn\*.*"

rem 플릿풋
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster01.zip"		"wolf_boss\*.*"


rem ######## 렌고트 부락 ##################################################
rem 벌
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster01.zip"		"bee\*.*"

rem 사슴
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster01.zip"		"crescentdeer\*.*"

rem 하운터
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster01.zip"		"hountor\*.*"

rem 호저
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster01.zip"		"porcupine\*.*"

rem 렌고트
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster01.zip"		"goblin_dead\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster01.zip"		"goblin_fisher\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster01.zip"		"goblin_laborer\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster01.zip"		"goblin_worker\*.*"

rem 마리옹
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster01.zip"		"merion_ancient\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster01.zip"		"merion_assassin\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster01.zip"		"merion_chaos\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster01.zip"		"merion_chief\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster01.zip"		"merion_pawn\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster01.zip"		"merion_priest\*.*"

rem 고블린 골렘
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster01.zip"		"goblin_golem\*.*"

rem 보르고
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster01.zip"		"Ogre_B\*.*"


rem ######## 영웅의 오솔길 ################################################
rem 시체
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster01.zip"		"rottendead\*.*"

rem 구울
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster01.zip"		"Ghoul_assassin\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster01.zip"		"Ghoul_berserker\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster01.zip"		"Ghoul_fanatic\*.*"

rem 스켈레톤
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster01.zip"		"Skeleton_archer\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster01.zip"		"skeleton_armor_01\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster01.zip"		"skeleton_armor_02\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster01.zip"		"skeleton_evos\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster01.zip"		"Skeleton_fighter\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster01.zip"		"skeleton_goblin\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster01.zip"		"skeleton_kobold\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster01.zip"		"Skeleton_mage\*.*"

rem 유령
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster01.zip"		"ghost_archer\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster01.zip"		"Ghost_horse\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster01.zip"		"Ghost_knight\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster01.zip"		"Ghost_servant\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster01.zip"		"Ghost_soldier\*.*"

rem 미이라
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster01.zip"		"mummy_lord\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster01.zip"		"mummy_mage\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster01.zip"		"mummy_pawn\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster01.zip"		"mummy_priest\*.*"

rem 봉봉
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster01.zip"		"bonbon_acc_01\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster01.zip"		"bonbon_acc_02\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster01.zip"		"bonbon_acc_03\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster01.zip"		"bonbon_acc_04\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster01.zip"		"bonbon_acc_05\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster01.zip"		"bonbon_acc_06\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster01.zip"		"bonbon_acc_07\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster01.zip"		"bonbon_chief\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster01.zip"		"bonbon_hunter\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster01.zip"		"bonbon_mage\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster01.zip"		"bonbon_pawn\*.*"

rem 트롤
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster01.zip"		"troll\*.*"

rem 타일런트 좀비
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster01.zip"		"giant_zombie\*.*"

rem 고스트 라이더
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster01.zip"		"ghost_rider\*.*"

rem 장로 카르노스
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster01.zip"		"walking_tree_boss\*.*"


rem ######## 오염된 화원 ##################################################
rem 봉봉
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster01.zip"		"bonbon_chief_01\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster01.zip"		"bonbon_hunter_01\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster01.zip"		"bonbon_hunter_02\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster01.zip"		"bonbon_mage_01\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster01.zip"		"bonbon_pawn_01\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster01.zip"		"bonbon_pawn_02\*.*"

rem 화원의 주인
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster01.zip"		"gardener\*.*"


rem ######## 에다산 #######################################################
rem 산양
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster01.zip"		"goral\*.*"

rem 오우거
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster01.zip"		"Ogre_W\*.*"

rem 하피
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster01.zip"		"harpy\*.*"

rem 웬디고
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster01.zip"		"wendigo\*.*"

rem 플릿풋 유령
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster01.zip"		"wolf_boss_ghost\*.*"


rem ######## 부러진 돛대 ##################################################
rem 앵무새
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster01.zip"		"bird\*.*"

rem 병정게
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster01.zip"		"soldiercrab\*.*"

rem 에보스
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster01.zip"		"evos_chief\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster01.zip"		"evos_commander\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster01.zip"		"evos_mage\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster01.zip"		"evos_pawn\*.*"

rem 크라울러
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster01.zip"		"crawler\*.*"


rem ######## 리츠 지하 수로 ###############################################
rem 벌레
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster01.zip"		"bug\*.*"

rem 폴루토
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster01.zip"		"poluto\*.*"

rem 퀸
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster01.zip"		"queen\*.*"


rem ######## 리모스 사막 ##################################################
rem 토나델로
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster01.zip"		"giant_turtle\*.*"

rem 소레나
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster01.zip"		"sorena\*.*"


rem ######## 금단의 성지 ##################################################
rem 베루스
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster01.zip"		"cerberus\*.*"


rem ######## 폐허의 숲 ####################################################
rem 아레크
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster01.zip"		"arek\*.*"


rem ######## 마코트 부락 ##################################################
rem 코볼드
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster01.zip"		"kobold_chief\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster01.zip"		"kobold_commander\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster01.zip"		"kobold_mage\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster01.zip"		"kobold_pawn\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster01.zip"		"kobold_ranger\*.*"


rem ######## 미추가 NPC ###################################################
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster02.zip"		"bats\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster02.zip"		"bat_boss\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster02.zip"		"bomb\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster02.zip"		"buffalo\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster02.zip"		"evil_eye\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster02.zip"		"executioner\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster02.zip"		"izenique\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster02.zip"		"myconid_fungus\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster02.zip"		"myconid_hatcher\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster02.zip"		"myconid_minion\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster02.zip"		"myconid_spawn\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster02.zip"		"Pumpkin\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster02.zip"		"Pumpkin_boss\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster02.zip"		"puppet_B\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster02.zip"		"puppet_G\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster02.zip"		"puppet_R\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster02.zip"		"puppet_Y\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster02.zip"		"Sprite_B\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster02.zip"		"Sprite_G\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster02.zip"		"Sprite_R\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster02.zip"		"Sprite_W\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster02.zip"		"Sprite_Y\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster02.zip"		"the_shackle\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster02.zip"		"valkyrie\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster02.zip"		"will_o_the_wisp\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster02.zip"		"chimera\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster02.zip"		"giant_bats\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster02.zip"		"golem\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster02.zip"		"manelloth\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Monster\Monster02.zip"		"Ogre_R\*.*"
cd ..

echo .
echo ---------------------------------------------------------------------
echo  NPC
echo ---------------------------------------------------------------------
echo .
cd "NPC"
mkdir "%TARGETPATH%\Data\Model\NPC"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\NPC\NPC01.zip"				"arel\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\NPC\NPC01.zip"				"armadillo\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\NPC\NPC01.zip"				"cat\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\NPC\NPC01.zip"				"chick\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\NPC\NPC01.zip"				"chicken\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\NPC\NPC01.zip"				"con_garden_door\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\NPC\NPC01.zip"				"dog\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\NPC\NPC01.zip"				"door_house1\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\NPC\NPC01.zip"				"frog\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\NPC\NPC01.zip"				"gathering_box\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\NPC\NPC01.zip"				"gathering_ore\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\NPC\NPC01.zip"				"gathering_plant\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\NPC\NPC01.zip"				"gathering_wood\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\NPC\NPC01.zip"				"goblin_bomb\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\NPC\NPC01.zip"				"hf_adult\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\NPC\NPC01.zip"				"hf_adult_1hb\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\NPC\NPC01.zip"				"hf_adult_1hp\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\NPC\NPC01.zip"				"hf_adult_1hs\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\NPC\NPC01.zip"				"hf_adult_2hd\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\NPC\NPC01.zip"				"hf_adult_arc\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\NPC\NPC01.zip"				"hf_adult_dead\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\NPC\NPC01.zip"				"hf_adult_dwp\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\NPC\NPC01.zip"				"hf_adult_sit\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\NPC\NPC01.zip"				"hf_adult_sta\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\NPC\NPC01.zip"				"hf_fat\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\NPC\NPC01.zip"				"hf_fighter\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\NPC\NPC01.zip"				"hf_kid\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\NPC\NPC01.zip"				"hf_kid_commoner\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\NPC\NPC01.zip"				"hf_kid_noble\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\NPC\NPC01.zip"				"hf_old\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\NPC\NPC01.zip"				"hm_adult\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\NPC\NPC01.zip"				"hm_adult_1hb\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\NPC\NPC01.zip"				"hm_adult_1hp\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\NPC\NPC01.zip"				"hm_adult_1hs\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\NPC\NPC01.zip"				"hm_adult_2hd\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\NPC\NPC01.zip"				"hm_adult_arc\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\NPC\NPC01.zip"				"hm_adult_dead\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\NPC\NPC01.zip"				"hm_adult_dwp\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\NPC\NPC01.zip"				"hm_adult_sick\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\NPC\NPC01.zip"				"hm_adult_sleep\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\NPC\NPC01.zip"				"hm_adult_sta\*.*"

..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\NPC\NPC02.zip"				"hm_fat\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\NPC\NPC02.zip"				"hm_fighter\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\NPC\NPC02.zip"				"hm_kid\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\NPC\NPC02.zip"				"hm_kid_commoner\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\NPC\NPC02.zip"				"hm_kid_noble\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\NPC\NPC02.zip"				"hm_old\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\NPC\NPC02.zip"				"illuwall01_garden\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\NPC\NPC02.zip"				"mouse\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\NPC\NPC02.zip"				"npc_drum01\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\NPC\NPC02.zip"				"pig\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\NPC\NPC02.zip"				"rabbit\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\NPC\NPC02.zip"				"Rengot_campfire_k01\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\NPC\NPC02.zip"				"Rengot_sculpture_k01\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\NPC\NPC02.zip"				"temp_npc\*.*"
cd ..

echo .
echo ---------------------------------------------------------------------
echo  Player
echo ---------------------------------------------------------------------
echo .
cd "Player"
mkdir "%TARGETPATH%\Data\Model\Player"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Player\Player01.zip"		"hm\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Player\Player01.zip"		"hm\ani\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Player\Player02.zip"		"hf\*.*"
..\..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Player\Player02.zip"		"hf\ani\*.*"
cd ..

echo .
echo ---------------------------------------------------------------------
echo  Sky
echo ---------------------------------------------------------------------
echo .
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\Sky.zip"						"Sky\*.*"

echo .
echo ---------------------------------------------------------------------
echo  Speed tree
echo ---------------------------------------------------------------------
echo .
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\SpeedTree.zip"				"SpeedTree\*.*"

echo .
echo ---------------------------------------------------------------------
echo  무기
echo ---------------------------------------------------------------------
echo .
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Model\weapons.zip"					"weapons\*.*"
cd ..



echo .
echo ---------------------------------------------------------------------
echo  Script
echo ---------------------------------------------------------------------
echo .
..\pkzip25 -add -path=specify "%TARGETPATH%\Data\system.zip"							"Script\rel_const.lua"
..\pkzip25 -add -path=specify "%TARGETPATH%\Data\system.zip"							"Script\field\*.*"



echo .
echo ---------------------------------------------------------------------
echo  SFX
echo ---------------------------------------------------------------------
echo .
..\pkzip25 -add -path=specify "%TARGETPATH%\Data\SFX.zip"								"SFX\*.*"
..\pkzip25 -add -path=specify "%TARGETPATH%\Data\SFX.zip"								"SFX\Camera\*.*"
..\pkzip25 -add -path=specify "%TARGETPATH%\Data\SFX.zip"								"SFX\deprecated\*.*"



echo .
echo ---------------------------------------------------------------------
echo  쉐이더
echo ---------------------------------------------------------------------
echo .
..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Shader.zip"							"Shader\*.*"
..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Shader.zip"							-include="Shader\components\*.rsc"
..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Shader.zip"							"Shader\obj\*.*"



echo .
echo ---------------------------------------------------------------------
echo  사운드
echo ---------------------------------------------------------------------
echo .
..\pkzip25 -add -path=specify "%TARGETPATH%\Data\system.zip"							"Sound\sound.xml"
cd "Sound"
mkdir "%TARGETPATH%\Data\Sound"

echo .
echo ---------------------------------------------------------------------
echo  BGM
echo ---------------------------------------------------------------------
echo .
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Sound\BGM01.zip"					"BGM\amb_*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Sound\BGM01.zip"					"BGM\part_*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Sound\BGM01.zip"					"BGM\quest_*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Sound\BGM02.zip"					"BGM\bgm_*.*"

echo .
echo ---------------------------------------------------------------------
echo  효과음
echo ---------------------------------------------------------------------
echo .
rem ######## 기타 #########################################################
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Sound\Effect01.zip"					"Effect\animal\*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Sound\Effect01.zip"					"Effect\armour\*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Sound\Effect01.zip"					"Effect\ef\*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Sound\Effect01.zip"					"Effect\footstep\*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Sound\Effect01.zip"					"Effect\hit\*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Sound\Effect01.zip"					"Effect\Item\*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Sound\Effect01.zip"					"Effect\object\*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Sound\Effect01.zip"					"Effect\pc\*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Sound\Effect01.zip"					"Effect\sd\*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Sound\Effect01.zip"					"Effect\SFX\*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Sound\Effect01.zip"					"Effect\ss\*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Sound\Effect01.zip"					"Effect\local\*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Sound\Effect01.zip"					"Effect\ui\*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Sound\Effect01.zip"					"Effect\vox\*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Sound\Effect01.zip"					"Effect\weapon\*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Sound\Effect01.zip"					"Effect\puppet\*.*"


rem ######## 테레스 평원 ##################################################
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Sound\Effect02.zip"					"Effect\spider\*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Sound\Effect02.zip"					"Effect\wolf\*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Sound\Effect02.zip"					"Effect\uruk\*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Sound\Effect02.zip"					"Effect\walking_tree\*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Sound\Effect02.zip"					"Effect\goblin\*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Sound\Effect02.zip"					"Effect\spider_boss\*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Sound\Effect02.zip"					"Effect\wolf_boss\*.*"

rem ######## 렌고트 부락 ##################################################
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Sound\Effect02.zip"					"Effect\bee\*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Sound\Effect02.zip"					"Effect\crescentdeer\*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Sound\Effect02.zip"					"Effect\porcupine\*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Sound\Effect02.zip"					"Effect\merion\*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Sound\Effect02.zip"					"Effect\goblin_golem\*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Sound\Effect02.zip"					"Effect\ogre\*.*"

rem ######## 영웅의 오솔길 ################################################
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Sound\Effect02.zip"					"Effect\rottendead\*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Sound\Effect02.zip"					"Effect\skeleton\*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Sound\Effect02.zip"					"Effect\ghost horse\*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Sound\Effect02.zip"					"Effect\ghost knight\*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Sound\Effect02.zip"					"Effect\ghost servant\*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Sound\Effect02.zip"					"Effect\ghost_archer\*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Sound\Effect02.zip"					"Effect\ghost_rider\*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Sound\Effect02.zip"					"Effect\bonbon\*.*"

rem ######## 오염된 화원 ##################################################
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Sound\Effect02.zip"					"Effect\gardener\*.*"

rem ######## 에다산 #######################################################
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Sound\Effect02.zip"					"Effect\wendigo\*.*"

rem ######## 부러진 돛대 ##################################################
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Sound\Effect02.zip"					"Effect\evos\*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Sound\Effect02.zip"					"Effect\crawler\*.*"

rem ######## 리츠 지하 수로 ###############################################
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Sound\Effect02.zip"					"Effect\poluto\*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Sound\Effect02.zip"					"Effect\queen\*.*"

rem ######## 리모스 사막 ##################################################
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Sound\Effect02.zip"					"Effect\sorena\*.*"

rem ######## 금단의 성지 ##################################################
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Sound\Effect02.zip"					"Effect\cerberus\*.*"

rem ######## 폐허의 숲 ####################################################
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Sound\Effect02.zip"					"Effect\arek\*.*"

rem ######## 마코트 부락 ##################################################
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Sound\Effect02.zip"					"Effect\kobold\*.*"
cd ..



echo .
echo ---------------------------------------------------------------------
echo  텍스쳐
echo ---------------------------------------------------------------------
echo .
cd "Texture"
mkdir "%TARGETPATH%\Data\Texture"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Texture\Character01.zip"			"Character\a*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Texture\Character01.zip"			"Character\b*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Texture\Character01.zip"			"Character\c*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Texture\Character01.zip"			"Character\d*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Texture\Character01.zip"			"Character\e*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Texture\Character01.zip"			"Character\f*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Texture\Character01.zip"			"Character\g*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Texture\Character01.zip"			"Character\h*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Texture\Character01.zip"			"Character\i*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Texture\Character01.zip"			"Character\j*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Texture\Character01.zip"			"Character\k*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Texture\Character01.zip"			"Character\l*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Texture\Character01.zip"			"Character\m*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Texture\Character01.zip"			"Character\n*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Texture\Character01.zip"			"Character\o*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Texture\Character01.zip"			"Character\p*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Texture\Character01.zip"			"Character\q*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Texture\Character01.zip"			"Character\r*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Texture\Character01.zip"			"Character\s*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Texture\Character01.zip"			"Character\t*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Texture\Character02.zip"			"Character\u*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Texture\Character02.zip"			"Character\v*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Texture\Character02.zip"			"Character\w*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Texture\Character02.zip"			"Character\x*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Texture\Character02.zip"			"Character\y*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Texture\Character02.zip"			"Character\z*.*"

..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Texture\Map01.zip"					"Map\2hbp*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Texture\Map01.zip"					"Map\Distortion*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Texture\Map01.zip"					"Map\dragon*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Texture\Map01.zip"					"Map\grandwall*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Texture\Map01.zip"					"Map\shield*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Texture\Map01.zip"					"Map\sunlight.dds"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Texture\Map01.zip"					"Map\Texture_base.tga.dds"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Texture\Map01.zip"					"Map\water*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Texture\Map01.zip"					"Map\login_*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Texture\Map01.zip"					"Map\article_*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Texture\Map01.zip"					"Map\broken-mast_*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Texture\Map01.zip"					"Map\con_garden_*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Texture\Map01.zip"					"Map\gathering_*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Texture\Map01.zip"					"Map\rinden_*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Texture\Map01.zip"					"Map\hero_*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Texture\Map02.zip"					"Map\ingen_*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Texture\Map02.zip"					"Map\makot_*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Texture\Map03.zip"					"Map\ritz_*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Texture\Map04.zip"					"Map\mt_eda_*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Texture\Map04.zip"					"Map\rengot_*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Texture\Map04.zip"					"Map\riode_*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Texture\Map04.zip"					"Map\teress_*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Texture\Map04.zip"					"Map\trial_*.*"

..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Texture\SFX.zip"					"SFX\*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Texture\Sky.zip"					"Sky\*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Texture\SpeedGrass.zip"				"SpeedGrass\*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Texture\SpeedTree.zip"				"SpeedTree\*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Texture\Tile.zip"					"Tile\*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Texture\Weapon.zip"					"Weapon\*.*"
cd ..



echo .
echo ---------------------------------------------------------------------
echo  Zone
echo ---------------------------------------------------------------------
echo .
cd "Zone"
mkdir "%TARGETPATH%\Data\Zone"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Zone\Zone01.zip"					"Auction\*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Zone\Zone01.zip"					"Barren\*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Zone\Zone01.zip"					"Barren_Mines\*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Zone\Zone01.zip"					"BirdTown\*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Zone\Zone01.zip"					"Canyon\*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Zone\Zone01.zip"					"Burning_Woods\*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Zone\Zone01.zip"					"Castle_of_Sedah\*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Zone\Zone01.zip"					"Castle_Rendel\*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Zone\Zone01.zip"					"Catacomb_of_damned\*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Zone\Zone01.zip"					"Contaminated_Garden\*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Zone\Zone01.zip"					"Forest_of_Mirage\*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Zone\Zone01.zip"					"Forest_of_Silent\*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Zone\Zone01.zip"					"Grand_Wall_of_the_Silent\*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Zone\Zone01.zip"					"Hero's_Lane\*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Zone\Zone01.zip"					"House_of_Barghasi\*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Zone\Zone01.zip"					"House_of_Godemore\*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Zone\Zone01.zip"					"Icevale\*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Zone\Zone01.zip"					"Ingen_Freeport\*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Zone\Zone01.zip"					"Ingen_Warehouse\*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Zone\Zone01.zip"					"Lans_Barren\*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Zone\Zone01.zip"					"Lighthouse_of_the_Freedom\*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Zone\Zone01.zip"					"Limos_Desert\*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Zone\Zone01.zip"					"Login2\*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Zone\Zone01.zip"					"Makot_Village\*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Zone\Zone01.zip"					"Mt_Eda\*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Zone\Zone01.zip"					"Refuge_of_the_White_Wings\*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Zone\Zone01.zip"					"Rengot_Village\*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Zone\Zone01.zip"					"Rinden_Hills\*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Zone\Zone01.zip"					"Riode\*.*"	
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Zone\Zone02.zip"					"Ritz\*.*"	
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Zone\Zone02.zip"					"Ritz_Sewer\*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Zone\Zone02.zip"					"Ritz_town\*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Zone\Zone02.zip"					"Ruined_Forest\*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Zone\Zone02.zip"					"Saint_Fhannel_Monastery\*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Zone\Zone02.zip"					"Scale_of_The_Unbalance\*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Zone\Zone02.zip"					"Scarbrow_Cavern\*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Zone\Zone02.zip"					"shop_adventurer\*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Zone\Zone02.zip"					"Swamp_of_The_Silence\*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Zone\Zone02.zip"					"Tears_End\*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Zone\Zone02.zip"					"Teress_Plain\*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Zone\Zone02.zip"					"The_Broken_Mast\*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Zone\Zone02.zip"					"The_Forgotten_Remains\*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Zone\Zone02.zip"					"The_graveyard_of_Betrayal\*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Zone\Zone02.zip"					"The_Mansion_of_Mist\*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Zone\Zone02.zip"					"The_Temple_of_Lacinos\*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Zone\Zone02.zip"					"The_Temple_of_Renas\*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Zone\Zone02.zip"					"The_Wrath_of_Renas\*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Zone\Zone02.zip"					"Trial_Castle_01\*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Zone\Zone02.zip"					"Trial_Castle_02\*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Zone\Zone02.zip"					"Trial_Castle_03\*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Zone\Zone02.zip"					"Trial_Dungeon_01\*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Zone\Zone02.zip"					"Trial_Dungeon_02\*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Zone\Zone02.zip"					"Trial_Dungeon_03\*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Zone\Zone02.zip"					"Trial_House_01\*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Zone\Zone02.zip"					"Trial_House_02\*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Zone\Zone02.zip"					"Trial_House_03\*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Zone\Zone02.zip"					"Trial_Warehouse_01\*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Zone\Zone02.zip"					"Trial_Warehouse_02\*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Zone\Zone02.zip"					"Trial_Warehouse_03\*.*"
..\..\pkzip25 -add -path=specify "%TARGETPATH%\Data\Zone\Zone02.zip"					"1070131\*.*"
cd ..
cd ..





:convert_to_mrs

echo .
echo ---------------------------------------------------------------------
echo  MRS 컨버젼
echo ---------------------------------------------------------------------
echo .
conv_zip -path %TARGETPATH%\

goto :quit















:err_no_target_path
echo [ERROR] 대상 폴더를 지정하지 않았습니다.
pause
goto :quit


:err_not_exist_pkzip
echo [ERROR] PKZIP25.exe 파일이 없습니다.
pause
goto :quit


:err_not_exist_convzip
echo [ERROR] CONV_ZIP.exe 파일이 없습니다.
pause
goto :quit






:quit


