-- revision
-- 0.1  : 초기버젼
-- 0.2  : Spawn 정보 추가 (Property가 아닌 edt.text 값으로 prop을 출력할때 기록된다)
-- 0.2a : Spawn 정보 xyy -> xyz
-- 0.2b : Pivot Position 정보 추가
-- 0.2c : Pivot Pos -> TM 으로 변경 TM = WorldTM * InvParentWorldTM
-- 0.2d : Spawn Pos / Dir 초기값 설정 (0,0,0) / (0,1,0)
-- 0.3  : 대부분의 기능이 CoreTool로 이동하여서 제거후 일괄익스포트 기능 추가

macroScript Gunz2Property category:"MAIET" toolTip:"Gunz2 Property"
(
	----------------------------------------------------------------------------------------------------
	-- GLOBAL
	----------------------------------------------------------------------------------------------------
	global routGunz2Property	-- Dialog
	
	-- Define
	Global DEF_ELUNAME 		= "EluName" as string
		
	----------------------------------------------------------------------------------------------------
	-- STRUCT
	----------------------------------------------------------------------------------------------------
	struct ZPROPERTIES (
		prop_strEluName 		-- Elu file name
	)
	
	rollout routGunz2Property "Gunz2 Property 0.3" width:272 height:197
	(
		GroupBox ui_grpCommon "Common" pos:[7,8] width:257 height:123
		label ui_lblSelected "Selected :" pos:[16,24] width:80 height:16
		edittext ui_edtSelected "" pos:[24,45] width:168 height:16 readOnly:true
		label ui_lblEluname "Elu name :" pos:[15,69] width:80 height:16 
 
 
		edittext ui_edtEluname "" pos:[23,90] width:168 height:16
			
			
		
		GroupBox ui_grpExportTo "Export to" pos:[6,137] width:257 height:48
		button ui_btnExportAll "Export All" pos:[95,153] width:88 height:21
		
		----------------------------------------------------------------------------------------------------
		-- Update UI	
		----------------------------------------------------------------------------------------------------
		function UpdateUI  Properties = (
	
			-- UI 업데이트
			ui_edtEluName.text 				= Properties.prop_strEluName as string
		)
		
		----------------------------------------------------------------------------------------------------
		-- Get Properties from node
		----------------------------------------------------------------------------------------------------
		function GetProperties anode = (
			
			thisProperties = ZPROPERTIES()
			
			thisProperties.prop_strEluName 				= GetUserProp anode DEF_ELUNAME
			
			-- UI 초기화
			--if thisProperties.prop_strEluName == undefined then
			--	thisProperties.prop_strEluName = ""
					
			thisProperties
		)
		
		----------------------------------------------------------------------------------------------------
		-- Select Changed
		----------------------------------------------------------------------------------------------------
		function OnSelectionChanged = (
			
			if not ( selection.count > 0 ) then 
			(
				return()
			)
			
			-- 여러개의 Selection이 있을때 가장 첫번째만 읽어 들인다.			
			local selectProperties = GetProperties selection[1]
			-- 디버깅용
			ui_edtSelected.text = selection[1].name
	
			UpdateUI selectProperties
		)
		
		----------------------------------------------------------------------------------------------------
		-- Set Prop
		----------------------------------------------------------------------------------------------------


		on ui_edtEluname entered text do
		(			
			if ui_edtEluName != undefined then ( SetUserProp selection[1] DEF_ELUNAME ui_edtEluName.text 
		)
		)
		on ui_btnExportAll pressed do
		(
			local format_ext = "xml";
			local format_name = "Scene XML";
		
			local filename = (getFilenameFile maxFilename);
			local searchmask = format_name + "(*." + format_ext + ")|*." + format_ext;
			filename = getsavefilename filename:filename types:searchmask;
			if( filename != undefined ) then
			(
				-- export realspace scene
				local eluname = getFilenamePath filename + getFilenameFile filename;
				eluname = eluname + ".elu"
				print eluname
				exportFile( eluname )
				
				-- export physx
				
				-- Physx Control Panel "Reset"
				px.pxrestart();
				try( px.pxremoveall(); ) catch()
				px_control.addPhysicsObjects $objects;

				local physx_format_ext = "nxb";
				-- local format_name = "NxuStream XML";		-- for xml
				local format_name = "NxuStream BINARY";    -- for binary
				local physxname = getFilenamePath filename + getFilenameFile filename;
				physxname = physxname + ".physx." + physx_format_ext;

				local result = px.pxnxuexport physxname physx_format_ext;
				if(result == 0) then
					( messagebox "Save Physx.nxb Failed!!" )
					
			)				
		)
	)
	
	----------------------------------------------------------------------------------------------------
	-- execute
	----------------------------------------------------------------------------------------------------
	on execute do (
		createDialog routGunz2Property
		callbacks.removeScripts #selectionSetChanged id:#Gunz2PropertyCallbacks 
		callbacks.addScript #selectionSetChanged "routGunz2Property.OnSelectionChanged()"  id:#Gunz2PropertyCallbacks
		routGunz2Property.OnSelectionChanged()
	)

)