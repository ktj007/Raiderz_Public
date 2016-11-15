-- 10.03.16 : v1.0
-- 10.03.17 : v2.0 / elu Export 시에 'ground_' 이름이 포함된 오브젝트가 있으면 Ground Expoer 실행
-- 10.06.08 : v2.1 / 특정단어(a, x, m)가 포함됐을때 FileName 제대로 안나오던 문제 수정
macroScript EasyExporter
category:"MAIET" 
ButtonText:"EasyExporter"
ToolTip:"EasyExporter"
(
	exportFileType = "elu";
	typeString = exportFileType + "File(*." + exportFileType + ")|*"+ exportFileType +"|All|*.*|"
	if ( maxFilename != "" ) then
	(
		inFileName = (getFilenameFile maxFilename) + "." + exportFileType
		if (inFileName != "" ) then
		(
			f = getSaveFileName caption:"Export" filename:inFileName types:typeString
			if f != undefined then (
					exportFile(f)
			)
			
			-- 전체 오브젝트 순회
			bIsGround = false
			for i = 1 to objects.count do (
				-- 'ground_' 의 이름을 가지는 오브젝트가 있는가
				if ( matchPattern objects[i].name pattern:"ground_*" == true ) then (
					bIsGround = true
					exit 
				)
			)
			
			-- 오브젝트 이름중에 ground_ 가 있고
			if bIsGround == true then (
				for i in utilityplugin.classes do (
					-- Ground Mesh Export Plugins 가 있을때만 실행
					if i == Ground_Mesh_Export then  (
						UtilityPanel.OpenUtility Ground_Mesh_Export
					)
				)	
			)
		)
	)
)
