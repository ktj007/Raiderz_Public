macroScript MaietProperty category:"MAIET" toolTip:"MAIET Properties"
(
	global MaietProperties
	
	struct MPROPERTIES
	(
		prop_bVertexcolor, 
		prop_bShade, 				-- 쉐이딩(라이트맵)을 키고 끈다
		prop_bVisible,
		prop_bVisible_at_night,	-- 밤에만 보이게
		prop_bStatic,					-- Static Mesh (*.rs) Export 시 Static Mesh가 체크 되어 있을때만 rs로 출력한다.
		prop_bCollision,
		prop_bCollisionOnly,		-- 충돌 전용 메쉬 Box, Sphere, Capsule 만 가능하고, Export 되지 않고 Xml 에 저장된다.
		prop_bExportIgnore,		-- 출력하지 않는다
		prop_bAlign, 					-- (-y)를 카메라를 바라보게 정렬   
		prop_bAlign_z,		  			-- z축을 고정하고 나머지축을 돌려 -y를 카메라쪽으로 정렬   
		prop_bAddeffect, 			-- <문자열>에 해당하는 이펙트를 이 노드에 붙인다.  
		prop_strAddeffect_name,	-- 노드에 붙일 문자열 set_addeffect_name = dust.Effect.xml  
		prop_fEffectScale,			-- 이펙트 스케일
		prop_bWater,
		prop_bCloth,
		prop_strCloth_Anchor_name,	-- 옷감 Anchor가 참조할 액터 노드 
		prop_bClothcollision,				-- 옷감 충돌용 모델
		prop_strCloth_Related_name,	-- 옷감 충돌체가 참조하는 Cloth 노드
		
		-- F/X
		prop_bGhostFX,				-- 유령 효과
		
		-- Sound
		prop_bSound,					-- 사운드 정보
		prop_strSound_name,		-- 사운드 이름
		prop_nMinRadius,			-- 사운드 시작 반지름
		prop_nMaxRadius,			-- 사운드 끝 반지름
		
		--Occluder
		prop_bOccluder                -- 해당노드가 오클루더인가
	)
	
	-- revision
	-- 0.1  : 초기버젼
	-- 0.2  : collision default = true로 변경 되고 충돌 전용 메쉬는 없어짐
	-- 0.2a: Light 'Ignore at export' 제대로 안되던 문제 수정
	-- 0.2b: Add_Effect 기본 상태 조정 및 두개 이상의 메쉬가 선택되고 Add_Effect의 이름이 다를때 'different'로 표기
	-- 0.2c: 중복 선택된 상태에서 Prop 변경 되었을때 나머지 값까지 현재 선택된 Prop로 되던 문제 수정
	-- 0.2d: 옷감 관련 피쳐 추가 prop_strCloth_Anchor_name, prop_strCloth_Related_name
	-- 0.3  : prop_bCollisionOnly 추가 충돌 전용 메쉬 Box, Sphere, Capsule 만 가능하고, Export 되지 않고 Xml 에 저장된다.
	-- 0.4  : Sound Property 추가 사운드 이름 / Min, Max Radius
	-- 0.5  : Occluder정보가 추가되었음
	-- 0.5a : Effect Scale 값 추가. Uniform Scale 값이다.
	-- 0.5b : 셀렉트시 Scale 값 제대로 표시 안되던 버그 수정
	-- 0.5c : GlobalProperty 제대로 갱신 안되던 버그 수정
	rollout MaietProperties "MAIET 0.5c" width:328 height:566
	(
		GroupBox grp1 "Property" pos:[8,8] width:152 height:104
		checkbox ui_chkVertexcolor "Vertex Color" pos:[16,24] width:136 height:16
		checkbox ui_chkShade "Shade" pos:[16,40] width:130 height:16
		checkbox ui_chkVisible "Visible" pos:[16,56] width:130 height:16
		checkbox ui_chkVisible_at_night "Visible at night" pos:[32,72] width:120 height:16
		checkbox ui_chkStatic "Static Mesh(.rs)" pos:[16,88] width:130 height:16
		
		GroupBox grp2 "Collision" pos:[8,120] width:152 height:54
		checkbox ui_chkCollision "Collision" pos:[16,136] width:130 height:16
		checkbox ui_chkCollisionOnly "Collision Only" pos:[32,154] width:120 height:16
		
		GroupBox grp3 "Export" pos:[8,183] width:152 height:40
		checkbox ui_chkExportIgnore "Ignore at export" pos:[16,199] width:130 height:16
		
		GroupBox grp4 "Align" pos:[8,231] width:152 height:129
		checkbox ui_chkAlign "Align" pos:[16,247] width:130 height:16
		checkbox ui_chkAlign_z "Align_z" pos:[16,263] width:130 height:16
		checkbox ui_chkAddeffect "Add Effect" pos:[16,279] width:130 height:16
		edittext ui_edtAddeffect_name "" pos:[23,295] width:120 height:16 enabled:true 
		label ui_labEffectScale "Effect Scale( Uniform )" pos:[16,313] width:130 height:16
		spinner spin_EffectScale "" pos:[25,331] width:120 height:16 type:#float range:[0,100,1] scale:0.1
  		
		GroupBox grp5 "Water" pos:[8,370] width:152 height:40
		checkbox ui_chkWater "Water" pos:[16,386] width:130 height:16
		
		GroupBox grp6 "Cloth" pos:[8,418] width:152 height:88
		checkbox ui_chkCloth "Cloth" pos:[16,434] width:130 height:16
		checkbox ui_chkClothCollision "Collision" pos:[16,468] width:130 height:16
		edittext ui_edtCloth_Anchor_name "" pos:[23,450] width:120 height:16 enabled:true	
		edittext ui_edtCloth_Related_name "" pos:[23,484] width:120 height:16 enabled:true
		
		GroupBox grp7 "F/X" pos:[8,514] width:152 height:40
		checkbox ui_chkGhostFX "Ghost F/X" pos:[16,530] width:136 height:16
		
		GroupBox grp8 "Global Property" pos:[168,8] width:152 height:56
		checkbox ui_chkAlpha2Pass "Alpha2Pass" pos:[176,24] width:136 height:16
		checkbox ui_chkAutoAnimation "AutoAnimation" pos:[176,44] width:136 height:16
		
		GroupBox grp25 "Sound" pos:[168,136] width:152 height:135
		checkbox ui_chkSound "Sound Name" pos:[176,152] width:130 height:16
		edittext ui_edtSound_Soundname "" pos:[183,168] width:120 height:16 enabled:true
		edittext ui_edtSound_MinRadius "" pos:[183,206] width:120 height:16 enabled:true
		label lbl1 "Min Radius" pos:[176,191] width:130 height:16
		edittext ui_edtSound_MaxRadius "" pos:[183,240] width:120 height:16 enabled:true
		label lbl2 "Max Radius" pos:[176,225] width:130 height:16
		
		GroupBox ui_grpLOD "LOD" pos:[168,72] width:152 height:56
		checkbox ui_chkFogCulling "Fog Culling" pos:[176,88] width:96 height:16 enabled:true checked:true
		checkbox ui_chkEnableDistanceCulling "Enable Distance Culling" pos:[176,104] width:136 height:16 enabled:true checked:true
		GroupBox grp11 "Occluder" pos:[169,280] width:152 height:35
		checkbox ui_chkOccluder "Occluder" pos:[179,296] width:109 height:13
		
-----------------------------------------------------------------------------------------------
-- enable/disable UI
-----------------------------------------------------------------------------------------------
		function EnableControls bEnable = (
			
			ui_chkVertexcolor.enabled			= bEnable
			ui_chkShade.enabled					= bEnable
			ui_chkVisible.enabled					= bEnable
			ui_chkVisible_at_night.enabled	= bEnable
			ui_chkStatic.enabled					= bEnable
			ui_chkCollision.enabled 				= bEnable
			ui_chkCollisionOnly.enabled 		= bEnable
			ui_chkExportIgnore.enabled		= bEnable
			ui_chkAlign.enabled						= bEnable
			ui_chkAlign_z.enabled					= bEnable
			ui_chkAddeffect.enabled				= bEnable
			ui_chkWater.enabled 					= bEnable
			ui_chkCloth.enabled					= bEnable
			ui_chkClothCollision.enabled		= bEnable
			ui_chkGhostFX.enabled				= bEnable
			ui_chkOccluder.enabled				= bEnable
			
			if (ui_chkAddeffect.enabled == false ) then
			(
				ui_edtAddeffect_name.readonly = true
				ui_edtAddeffect_name.text = ""
			)

			if (ui_chkCloth.enabled == false ) then
			(
				ui_edtCloth_Anchor_name.readonly = true
				ui_edtCloth_Anchor_name.text = ""
			)

			if (ui_chkClothCollision.enabled == false ) then
			(
				ui_edtCloth_Related_name.readonly = true
				ui_edtCloth_Related_name.text = ""
			)	
			
		)

-----------------------------------------------------------------------------------------------
-- Get Properties from node
-----------------------------------------------------------------------------------------------		
	-- version1. 항목만 있으면 참이다.
	-- bDef 는 해당 키가 있으면 가질 값을 선언해 둔다. 키가 있으면 false 인 값이 있어서 이렇게 사용함
		function GetUserProperty_bool_v1 &refProp aNode strProp bDef = (
			
			if (  (GetUserProp aNode strProp) != undefined ) then
			(
				-- <key_string>만 있으면 undefined, true, false 이 아닌값이 들어간다. 디버깅에서는 undefined 로 표기됨
				if (GetUserProp aNode strProp) != true and (GetUserProp aNode strProp) != false then
				(
					refProp = bDef
				)
				
			)
		)
		
	-- version2. 항목이 있고 값이 true 이어야 참이다. 
		function GetUserProperty_bool_v2 &refProp aNode strProp = (
			
			if (  (GetUserProp aNode strProp) != undefined ) then
			(
				if refProp != true  then
					refProp = ( (GetUserProp aNode strProp) == true )
			)
							
		)
		
		function GetUserProperty_int_v2 &refProp aNode strProp = (
			
			if ( refProp == undefined ) then 
				refProp  = GetUserProp aNode strProp
			
		)


		function GetProperties aNode = (
			thisProperties = MPROPERTIES()
			
		-- prev version format
			GetUserProperty_bool_v1 &thisProperties.prop_bVertexcolor 		aNode "vertexcolor" true
			GetUserProperty_bool_v1 &thisProperties.prop_bShade 			aNode "noshade" false
			GetUserProperty_bool_v1 &thisProperties.prop_bCollision 			aNode "nocollision" false
			GetUserProperty_bool_v1 &thisProperties.prop_bAlign 				aNode "set_align" true
			GetUserProperty_bool_v1 &thisProperties.prop_bAlign_z 			aNode "set_align_z" true
			GetUserProperty_bool_v1 &thisProperties.prop_bAddeffect 		aNode "add_effect" true
			GetUserProperty_bool_v1 &thisProperties.prop_bCloth 				aNode "set_cloth_mesh" true
			GetUserProperty_bool_v1 &thisProperties.prop_bClothcollision 	aNode "set_collision_cloth_mesh" true
			-- Addeffect name
			if ( thisProperties.prop_bAddeffect == true ) then thisProperties.prop_strAddeffect_name = GetUserProp aNode "add_effect"
						
		-- new version format
			GetUserProperty_bool_v2 &thisProperties.prop_bVertexcolor 		aNode "set_vertexcolor"
			GetUserProperty_bool_v2 &thisProperties.prop_bShade 			aNode "set_shade"
			GetUserProperty_bool_v2 &thisProperties.prop_bVisible 			aNode "set_visible"
			GetUserProperty_bool_v2 &thisProperties.prop_bVisible_at_night aNode "set_visible_at_night"
			GetUserProperty_bool_v2 &thisProperties.prop_bStatic	 			aNode "set_static"
			GetUserProperty_bool_v2 &thisProperties.prop_bCollision 			aNode "set_collision"
			GetUserProperty_bool_v2 &thisProperties.prop_bCollisionOnly 	aNode "set_collisiononly"
			GetUserProperty_bool_v2 &thisProperties.prop_bExportIgnore	aNode "ignore_at_export"
			GetUserProperty_bool_v2 &thisProperties.prop_bExportIgnore	aNode "set_export_ignore"
			GetUserProperty_bool_v2 &thisProperties.prop_bAlign 				aNode "set_align"
			GetUserProperty_bool_v2 &thisProperties.prop_bAlign_z 			aNode "set_align_z"
			GetUserProperty_bool_v2 &thisProperties.prop_bAddeffect 		aNode "set_addeffect"
			GetUserProperty_bool_v2 &thisProperties.prop_bWater 				aNode "set_water"
			GetUserProperty_bool_v2 &thisProperties.prop_bCloth 				aNode "set_cloth"
			GetUserProperty_bool_v2 &thisProperties.prop_bClothcollision 	aNode "set_cloth_collision"
			GetUserProperty_bool_v2 &thisProperties.prop_bGhostFX 			aNode "set_fx_ghost"
			GetUserProperty_bool_v2 &thisProperties.prop_bSound 			aNode "set_sound"
			GetUserProperty_bool_v2 &thisProperties.prop_bOccluder 		aNode "set_occluder"
			
			-- Addeffect name
			if ( thisProperties.prop_bAddeffect == true ) then 
			(
				thisProperties.prop_strAddeffect_name 	= GetUserProp aNode "set_effect_name"
				thisProperties.prop_fEffectScale 				= GetUserProp aNode "set_effect_scale"
			)
			
			-- Cloth Anchor name
			if ( thisProperties.prop_bCloth == true ) then thisProperties.prop_strCloth_Anchor_name = GetUserProp aNode "set_cloth_anchor_name"
			-- Cloth Anchor name
			if ( thisProperties.prop_bClothcollision == true ) then thisProperties.prop_strCloth_Related_name = GetUserProp aNode "set_cloth_related_name"
			
			if ( thisProperties.prop_bSound == true ) then 
			(
				thisProperties.prop_strSound_name = GetUserProp aNode "set_sound_name"
				thisProperties.prop_nMinRadius 		= GetUserProp aNode "set_sound_minradius"
				thisProperties.prop_nMaxRadius 	= GetUserProp aNode "set_sound_maxradius"
			)
			
			
			
		
		-- Init : default
			if ( thisProperties.prop_bVertexcolor == undefined ) then thisProperties.prop_bVertexcolor 	= false
			if ( thisProperties.prop_bShade == undefined ) then thisProperties.prop_bShade 				= true
			if ( thisProperties.prop_bVisible == undefined ) then thisProperties.prop_bVisible 				= true
			if ( thisProperties.prop_bVisible_at_night == undefined ) then thisProperties.prop_bVisible_at_night	= false
			if ( thisProperties.prop_bStatic == undefined ) then thisProperties.prop_bStatic 					= false
			if ( thisProperties.prop_bCollision == undefined ) then thisProperties.prop_bCollision 			= true
			if ( thisProperties.prop_bCollisionOnly == undefined ) then thisProperties.prop_bCollisionOnly		= false
			if ( thisProperties.prop_bExportIgnore == undefined ) then thisProperties.prop_bExportIgnore 	= false
			if ( thisProperties.prop_bAlign == undefined ) then thisProperties.prop_bAlign 						= false
			if ( thisProperties.prop_bAlign_z == undefined ) then thisProperties.prop_bAlign_z 				= false
			if ( thisProperties.prop_bAddeffect == undefined ) then thisProperties.prop_bAddeffect 		= false
			if ( thisProperties.prop_bWater == undefined ) then thisProperties.prop_bWater 					= false
			if ( thisProperties.prop_bCloth == undefined ) then thisProperties.prop_bCloth 						= false
			if ( thisProperties.prop_bClothcollision == undefined ) then thisProperties.prop_bClothcollision = false
			if ( thisProperties.prop_bGhostFX == undefined ) then thisProperties.prop_bGhostFX 			= false
			if ( thisProperties.prop_strAddeffect_name == undefined ) then thisProperties.prop_strAddeffect_name = ""
			-- 0.2d
			if ( thisProperties.prop_strCloth_Anchor_name == undefined ) then thisProperties.prop_strCloth_Anchor_name = ""
			if ( thisProperties.prop_strCloth_Related_name == undefined ) then thisProperties.prop_strCloth_Related_name = ""
			-- 0.4
			if ( thisProperties.prop_bSound == undefined ) then thisProperties.prop_bSound 				= false
			if ( thisProperties.prop_strSound_name == undefined ) then thisProperties.prop_strSound_name = ""
			if ( thisProperties.prop_nMinRadius == undefined ) then thisProperties.prop_nMinRadius 	= 0
			if ( thisProperties.prop_nMaxRadius == undefined ) then thisProperties.prop_nMaxRadius 	= 0
			-- 0.5
			if ( thisProperties.prop_bOccluder == undefined ) then thisProperties.prop_bOccluder 	= false
			
		
			thisProperties
		)

-----------------------------------------------------------------------------------------------
-- Update UI		
-----------------------------------------------------------------------------------------------
		function UpdateUI_checkbox refProp &ui_chk = (
			
			if refProp ==undefined then
				ui_chk.triState = 2
			else
				ui_chk.checked = refProp
			
		)
		
		function UpdateUI_radiobuttons refProp &ui_rdo = (
			
			if refProp== undefined then 
				ui_rdo.state = 1
			else
				ui_rdo.state = refProp
		)
		
		function UpdateUI_spin refProp &ui_spin = (
			
			if refProp ==undefined then
				ui_spin.value = 1
			else
				ui_spin.value = refProp
		)
		
		function UpdateUI  allProperties = (
		
			UpdateUI_checkbox allProperties.prop_bVertexcolor 		&ui_chkVertexcolor
			UpdateUI_checkbox allProperties.prop_bShade 			 	&ui_chkShade
			UpdateUI_checkbox allProperties.prop_bVisible 			 	&ui_chkVisible
			UpdateUI_checkbox allProperties.prop_bStatic			 	&ui_chkStatic
			UpdateUI_checkbox allProperties.prop_bCollision 			&ui_chkCollision
			UpdateUI_checkbox allProperties.prop_bCollisionOnly	&ui_chkCollisionOnly
			UpdateUI_checkbox allProperties.prop_bExportIgnore 	&ui_chkExportIgnore
			UpdateUI_checkbox allProperties.prop_bAlign 				&ui_chkAlign
			UpdateUI_checkbox allProperties.prop_bAlign_z 			&ui_chkAlign_z
			UpdateUI_checkbox allProperties.prop_bAddeffect 		 	&ui_chkAddeffect
			UpdateUI_checkbox allProperties.prop_bWater 				&ui_chkWater
			UpdateUI_checkbox allProperties.prop_bCloth 				&ui_chkCloth
			UpdateUI_checkbox allProperties.prop_bClothcollision 	&ui_chkClothCollision
			UpdateUI_checkbox allProperties.prop_bGhostFX 			&ui_chkGhostFX
			UpdateUI_checkbox allProperties.prop_bSound 				&ui_chkSound
			UpdateUI_checkbox allProperties.prop_bOccluder 			&ui_chkOccluder
			
			if ui_chkVisible.triState == 1 then (
				UpdateUI_checkbox allProperties.prop_bVisible_at_night &ui_chkVisible_at_night
				ui_chkVisible_at_night.enabled = true
			)
			else
				ui_chkVisible_at_night.enabled = false
			
           
			if ( allProperties.prop_bAddeffect ==true ) then
			(
				ui_edtAddeffect_name.readonly = false
				if (allProperties.prop_strAddeffect_name == undefined ) then
					ui_edtAddeffect_name.text = "different" -- 둘다 존재하나 참조 Effect가 다를때
				else
					ui_edtAddeffect_name.text = allProperties.prop_strAddeffect_name as string
				
				UpdateUI_spin allProperties.prop_fEffectScale 			&spin_EffectScale
			)
			else
			(
				ui_edtAddeffect_name.readonly =true
				ui_edtAddeffect_name.text = ""
			)
			
			-- Cloth Anchor
			if ( allProperties.prop_bCloth == true ) then
			(
				ui_edtCloth_Anchor_name.readonly = false
				if (allProperties.prop_strCloth_Anchor_name == undefined ) then
					ui_edtCloth_Anchor_name.text = "different" -- 둘다 존재하나 참조가 다를때
				else
					ui_edtCloth_Anchor_name.text = allProperties.prop_strCloth_Anchor_name as string
			)
			else
			(
				ui_edtCloth_Anchor_name.readonly =true
				ui_edtCloth_Anchor_name.text = ""
				allProperties.prop_strCloth_Anchor_name = ""
			)
			
			-- Cloth Collision
			if ( allProperties.prop_bClothcollision == true ) then
			(
				ui_edtCloth_Related_name.readonly = false
				if (allProperties.prop_strCloth_Related_name == undefined ) then
					ui_edtCloth_Related_name.text = "different" -- 둘다 존재하나 참조가 다를때
				else
					ui_edtCloth_Related_name.text = allProperties.prop_strCloth_Related_name as string
			)
			else
			(
				ui_edtCloth_Related_name.readonly =true
				ui_edtCloth_Related_name.text = ""
			)
			
			-- Sound
			if ( allProperties.prop_bSound == true ) then
			(
				ui_edtSound_Soundname.readonly = false
				ui_edtSound_MinRadius.readonly 	= false
				ui_edtSound_MaxRadius.readonly 	= false
				if (allProperties.prop_strSound_name == undefined ) then
					ui_edtSound_Soundname.text = "different" -- 둘다 존재하나 참조가 다를때
				else
					ui_edtSound_Soundname.text = allProperties.prop_strSound_name as string
				
				ui_edtSound_MinRadius.text = allProperties.prop_nMinRadius as string
				ui_edtSound_MaxRadius.text = allProperties.prop_nMaxRadius as string
			)
			else
			(
				ui_edtSound_Soundname.readonly =true
				ui_edtSound_MinRadius.readonly 	= true
				ui_edtSound_MaxRadius.readonly 	= true
				ui_edtSound_Soundname.text = ""
				ui_edtSound_MinRadius.text = ""
				ui_edtSound_MaxRadius.text = ""
				allProperties.prop_strSound_name = ""
			)
		
		)

-----------------------------------------------------------------------------------------------
-- Update Node from UI
-----------------------------------------------------------------------------------------------
		function UpdateNodes_checkbox aNode strProp ui_chk = (
			
			if ui_chk.triState != 2 then
				( SetUserProp aNode strProp (ui_chk.triState == 1) )
			else
				( SetUserProp aNode strProp false )
			
		)
		
		function UpdateNodes_spinner aNode strProp ui_spn = (
			
			SetUserProp aNode strProp ui_spn.value 
			
		)
		
		function UpdateNodes_radiobuttons aNode strProp ui_rdo = (
			
			SetUserProp aNode strProp ui_rdo.state
			
		)
		
		

		-- 이전 포멧의 항목이 있으면 먼저 찾아서 지워준다
		function UpdateProperty iNode = (
			
			local strFlushbuffer = ""	-- 새로 담을 Property 버퍼
			local sstream = (getUserPropBuffer iNode) as stringstream
					
			while ( eof sstream == false ) do
			(
				local aLine = readLine sstream as string
				if (aLine == "") or -- 실제로는 '\r\n'이 있지만 읽어내지 못한다
				(stricmp aLine "vertexcolor" == 0 ) or
				(stricmp aLine "noshade" == 0 ) or
				(stricmp aLine "nocollision" == 0 ) or
				(stricmp aLine "set_align" == 0 ) or
				(stricmp aLine "set_align_z" == 0 ) or
				(stricmp aLine "add_effect" == 0 ) or
				(stricmp aLine "set_cloth_mesh" == 0 ) or
				(stricmp aLine "set_collision_cloth_mesh" == 0 )
				then
					continue
			
				aLine = aLine + "\r\n"	-- 행의 끝에 직접 개행문자를 넣어준다
				append strFlushbuffer aLine --strFlushbuffer = strFlushbuffer + aLine
			)
			
			close sstream
			setUserPropBuffer iNode strFlushbuffer 
		)
	
		function GlobalPropertyUpdate = (
					
			index = fileProperties.findProperty #custom "Alpha2Pass"
			if index != 0 then 
				ui_chkAlpha2Pass.checked = fileProperties.getPropertyValue #custom index
			else
				ui_chkAlpha2Pass.checked = false

			index = fileProperties.findProperty #custom "AutoAnimation"
			if index != 0 then 
				ui_chkAutoAnimation.checked = fileProperties.getPropertyValue #custom index
			else
				ui_chkAutoAnimation.checked = false
			
			index = fileProperties.findProperty #custom "FogCulling"
			if index != 0 then 
				ui_chkFogCulling.checked = fileProperties.getPropertyValue #custom index
			else
				ui_chkFogCulling.checked = false
			
			index = fileProperties.findProperty #custom "EnableDistanceCulling"
			if index != 0 then 
				ui_chkEnableDistanceCulling.checked = fileProperties.getPropertyValue #custom index
			else
				ui_chkEnableDistanceCulling.checked = false
		)
	
-----------------------------------------------------------------------------------------------
-- OnSelectionChanged
-----------------------------------------------------------------------------------------------
		function OnSelectionChanged = (
			
			local bEnabled = (selection.count>0)
			EnableControls (bEnabled)
			if not bEnabled then return()
						
			local firstProperties = GetProperties selection[1]
--			print firstProperties
			for i in selection do
			(
				local currentProperties = GetProperties i

				for p in (getPropNames MPROPERTIES) do
				(
					if (GetProperty firstProperties p) != (GetProperty currentProperties p) then
						SetProperty firstProperties p undefined	-- 하나라도 같지 않으면 undefined
				)
			)			
			UpdateUI firstProperties			
		)
		
-----------------------------------------------------------------------------------------------
-- define UI
-----------------------------------------------------------------------------------------------

		on ui_chkVertexcolor changed theState do
		(
			-- UpdateNodes()
			for iNode in selection do
			(
				UpdateProperty iNode
				UpdateNodes_checkbox iNode "set_vertexcolor" ui_chkVertexcolor
			)
		)
		on ui_chkShade changed theState do
		(
			for iNode in selection do
			(
				UpdateProperty iNode
				UpdateNodes_checkbox iNode "set_shade" ui_chkShade
			)
		)
		on ui_chkVisible changed theState do
		(
			for iNode in selection do
			(
				UpdateProperty iNode
				UpdateNodes_checkbox iNode "set_visible" ui_chkVisible
				if ui_chkVisible.triState == 1 then (
					--UpdateNodes_checkbox iNode "set_visible_at_night"	ui_chkVisible_at_night
					ui_chkVisible_at_night.enabled = true
				)
				else
					ui_chkVisible_at_night.enabled = false
			)
		)
		on ui_chkVisible_at_night changed theState do
		(
			for iNode in selection do
			(
				UpdateProperty iNode
				UpdateNodes_checkbox iNode "set_visible_at_night"	ui_chkVisible_at_night
			)
		)
		on ui_chkStatic changed theState do
		(
			for iNode in selection do
			(
				UpdateProperty iNode
				UpdateNodes_checkbox iNode "set_static" ui_chkStatic
			)
		)
		on ui_chkCollision changed theState do
		(
			for iNode in selection do
			(
				UpdateProperty iNode
				UpdateNodes_checkbox iNode "set_collision" ui_chkCollision
			)
		)
		on ui_chkCollisionOnly changed theState do
		(
			for iNode in selection do
			(
				UpdateProperty iNode
				UpdateNodes_checkbox iNode "set_collisiononly"	ui_chkCollisionOnly
			)
		)
		on ui_chkExportIgnore changed theState do
		(
			for iNode in selection do
			(
				UpdateProperty iNode
				UpdateNodes_checkbox iNode "set_export_ignore" ui_chkExportIgnore
			)
		)
		on ui_chkAlign changed theState do
		(
			for iNode in selection do
			(
				UpdateProperty iNode
				UpdateNodes_checkbox iNode "set_align" ui_chkAlign
			)
		)
		on ui_chkAlign_z changed theState do
		(
			for iNode in selection do
			(
				UpdateProperty iNode
				UpdateNodes_checkbox iNode "set_align_z" ui_chkAlign_z
			)
		)
		on ui_chkAddeffect changed theState do
		(
			for iNode in selection do
			(
				UpdateProperty iNode
				UpdateNodes_checkbox iNode "set_addeffect" ui_chkAddeffect
				
				if ui_chkAddeffect.triState == 1 then (
					ui_edtAddeffect_name.readonly =false
				)
				else
					ui_edtAddeffect_name.readonly = true
			)
		)
		on spin_EffectScale changed theState do
		(
			for iNode in selection do
			(
				UpdateProperty iNode
				if (ui_edtAddeffect_name.text != "" ) then	--  있을때만
				(
					UpdateNodes_spinner iNode "set_effect_scale" spin_EffectScale
					SetUserProp iNode "set_effect_name" ui_edtAddeffect_name.text
				)
			)
		)
		on ui_edtAddeffect_name entered txt do
		(
			for iNode in selection do
			(
				UpdateProperty iNode
				UpdateNodes_spinner iNode "set_effect_scale" spin_EffectScale -- 선 조건
				SetUserProp iNode "set_effect_name" ui_edtAddeffect_name.text
			)
		)
		on ui_chkWater changed theState do
		(
			for iNode in selection do
			(
				UpdateProperty iNode
				UpdateNodes_checkbox iNode "set_water" ui_chkWater
			)
		)
		on ui_chkCloth changed theState do
		(
			for iNode in selection do
			(
				UpdateProperty iNode
				UpdateNodes_checkbox iNode "set_cloth" ui_chkCloth
				
				if ui_chkCloth.triState == 1 then (
					ui_edtCloth_Anchor_name.readonly =false
				)
				else (
					ui_edtCloth_Anchor_name.readonly = true
					ui_edtCloth_Anchor_name.text = ""
					SetUserProp iNode "set_cloth_anchor_name" ui_edtCloth_Anchor_name.text
				)
			)
		)
		on ui_chkClothCollision changed theState do
		(
			for iNode in selection do
			(
				UpdateProperty iNode
				UpdateNodes_checkbox iNode "set_cloth_collision" ui_chkClothCollision
				
				if ui_chkClothCollision.triState == 1 then (
					ui_edtCloth_Related_name.readonly =false
				)
				else (
					ui_edtCloth_Related_name.readonly = true
					ui_edtCloth_Related_name.text = ""
					SetUserProp iNode "set_cloth_related_name" ui_edtCloth_Related_name.text
				)
			)
		)
		on ui_edtCloth_Anchor_name entered txt do
		(
			for iNode in selection do
			(
				UpdateProperty iNode
				SetUserProp iNode "set_cloth_anchor_name" ui_edtCloth_Anchor_name.text
			)
		)
		on ui_edtCloth_Related_name entered txt do
		(
		
			for iNode in selection do
			(
				UpdateProperty iNode
				SetUserProp iNode "set_cloth_related_name" ui_edtCloth_Related_name.text
			)
		)
		on ui_chkGhostFX changed theState do
		(
			for iNode in selection do
			(
				UpdateProperty iNode
				UpdateNodes_checkbox iNode "set_fx_ghost" ui_chkGhostFX
			)
		)
		on ui_chkAlpha2Pass changed state do
		(
			fileProperties.addProperty #custom "Alpha2Pass" state
		)
		on ui_chkAutoAnimation changed state do
		(
			fileProperties.addProperty #custom "AutoAnimation" state
		)
		on ui_chkSound changed theState do
		(
			for iNode in selection do
			(
				UpdateProperty iNode
				UpdateNodes_checkbox iNode "set_sound" ui_chkSound
				
				if ui_chkSound.triState == 1 then (
					ui_edtSound_Soundname.readonly =false
					ui_edtSound_MinRadius.readonly = false
					ui_edtSound_MaxRadius.readonly = false
				)
				else (
					ui_edtSound_Soundname.readonly = true
					ui_edtSound_MinRadius.readonly = true
					ui_edtSound_MaxRadius.readonly = true
					ui_edtSound_Soundname.text = ""
					SetUserProp iNode "set_sound_name" ui_edtSound_Soundname.text
				)
			)
		)
		on ui_edtSound_Soundname entered txt do
		(
			for iNode in selection do
			(
				UpdateProperty iNode
				SetUserProp iNode "set_sound_name" ui_edtSound_Soundname.text
			)
		)
		on ui_edtSound_MinRadius entered txt do
		(
			for iNode in selection do
			(
				UpdateProperty iNode
				SetUserProp iNode "set_sound_minradius" ui_edtSound_MinRadius.text
			)
		)
		on ui_edtSound_MaxRadius entered txt do
		(
			for iNode in selection do
			(
				UpdateProperty iNode
				SetUserProp iNode "set_sound_maxradius" ui_edtSound_MaxRadius.text
			)
		)
		on ui_chkFogCulling changed state do
		(
			fileProperties.addProperty #custom "FogCulling" state
		)
		on ui_chkEnableDistanceCulling changed state do
		(
			fileProperties.addProperty #custom "EnableDistanceCulling" state
		)
		on ui_chkOccluder changed state do
		(
			for iNode in selection do
			(
				UpdateProperty iNode
				UpdateNodes_checkbox iNode "set_occluder"	ui_chkOccluder
			)
		)
		
	)

	on execute do (
		
	--	NDH_StartDebugging()
		createDialog MaietProperties
		callbacks.removeScripts #selectionSetChanged id:#MaietPropertyCallbacks
		callbacks.addScript #selectionSetChanged "MaietProperties.OnSelectionChanged()"  id:#MaietPropertyCallbacks
		callbacks.addScript #selectionSetChanged "MaietProperties.GlobalPropertyUpdate()"  id:#MaietPropertyCallbacks
		
		MaietProperties.OnSelectionChanged()
		MaietProperties.GlobalPropertyUpdate()
	)
	
	on close do (
	--	NDH_StopDebugging()

	)
	
)